#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <string.h>

#include <GL/glew.h>
#include <GL/glx.h>

#define DEBUG 0
#define MAX_N_TOKENS 65536
#define DELIMITERS " \t\r\n\a"










float
elapsed_time_s(struct timespec start_time)
{
  struct timespec tick;
  clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
  if (tick.tv_nsec < start_time.tv_nsec)
    return (tick.tv_sec - 1.0f - start_time.tv_sec) + (1000000000.0f + tick.tv_nsec - start_time.tv_nsec)/1000000000.0f;
  else
    return (tick.tv_sec - start_time.tv_sec) + (tick.tv_nsec - start_time.tv_nsec)/1000000000.0f;
}










static GLuint
load_shaders_str(const char* vertex_shader_source, const char* fragment_shader_source)
{

  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  GLint Result = GL_FALSE;
  int InfoLogLength;

#if DEBUG
  printf("Compiling vertex shader\n");
  printf("------------------------- vertex shader -------------------------\n%s\n-----------------------------------------------------------------\n\n",vertex_shader_source);
#endif

  glShaderSource(VertexShaderID, 1, &vertex_shader_source , NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if(InfoLogLength>0)
  {
    char* VertexShaderErrorMessage;
    VertexShaderErrorMessage = (char*) malloc(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
    free(VertexShaderErrorMessage);
  }

#if DEBUG
  printf("Compiling fragment shader\n");
  printf("------------------------ fragment shader ------------------------\n%s\n-----------------------------------------------------------------\n\n",fragment_shader_source);
#endif

  glShaderSource(FragmentShaderID, 1, &fragment_shader_source , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if(InfoLogLength>0)
  {
    char* FragmentShaderErrorMessage;
    FragmentShaderErrorMessage = (char*) malloc(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
    free(FragmentShaderErrorMessage);
  }

  // Link the program
#if DEBUG
  printf("Linking program\n");
#endif
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);

  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if(InfoLogLength>0)
  {
    char* ProgramErrorMessage;
    ProgramErrorMessage = (char*) malloc(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
    free(ProgramErrorMessage);
  }

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}










static char*
read_file(const char* filename)
{
  char* buffer;
  long length = 0;
  size_t result = 0;
  FILE* fp = fopen(filename, "r");
  if(fp)
  {
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);
    buffer = (char*)malloc(length);
    if(buffer)
    {
      result = fread(buffer, 1, length, fp);
    }
    fclose(fp);
  }
  else
  {
#if DEBUG
    printf("file %s not loading\n",filename);
#endif
    exit(0);
  }
  
  if (result != length)
  {
#if DEBUG
    printf("Reading error %s\n",filename);
#endif
    exit(3);
  }
  return buffer;
}










static int
split_line (char* line, char*** p_tokens)
{
  char* temp_tokens[MAX_N_TOKENS];
  int i = 0, j = 0;
  char *token;
  int n_tokens = 0;

  token = strtok(line, DELIMITERS);
  while (token != NULL)
  {
    temp_tokens[i] = token;
    // printf("token : %s\n", temp_tokens[i]);
    i++;
    token = strtok(NULL, DELIMITERS);
  }
  n_tokens = i;

  // printf("----------\n");
  *p_tokens = (char**) malloc(n_tokens * sizeof(char*));
  for(j=0; j < n_tokens; j++)
  {
    // printf("token : %s\n", temp_tokens[j]);
    (*p_tokens)[j] = temp_tokens[j];
  }
  return n_tokens;
}










static void
shaders_defs(char* fragment_shader_source, struct shader_data* data)
{
  data->rate = 0.0f;
  char** tokens;
  int n_tokens;
  n_tokens = split_line (fragment_shader_source, &tokens);
  size_t i;
  for(i=0; i < n_tokens; i++)
  {
    if((strcmp(tokens[i],"#define")==0) && (strcmp(tokens[i+1],"RATE")==0))
    {
      data->rate = atof(tokens[i+2]);
    }
  }
#if DEBUG
  printf("shader data: rate = %f", data->rate);
#endif
  return;
}










static GLuint
load_shaders_file(const char* vertex_shader_file, const char* fragment_shader_file, struct shader_data* data)
{
  char *vertex_shader_str, *fragment_shader_str;
  GLuint ProgramID;

  vertex_shader_str = read_file(vertex_shader_file);
#if DEBUG
  printf("------------------------- vertex shader -------------------------\nfile: %s\n-----------------------------------------------------------------\n\n",vertex_shader_file);
#endif
  fragment_shader_str = read_file(fragment_shader_file);
#if DEBUG
  printf("------------------------ fragment shader ------------------------\nfile: %s\n-----------------------------------------------------------------\n\n",fragment_shader_file);
#endif
  ProgramID = load_shaders_str(vertex_shader_str, fragment_shader_str);
  shaders_defs(fragment_shader_str, data);

  free(vertex_shader_str);
  free(fragment_shader_str);
  return ProgramID;
}










Window
fullscreen_win(Display* xdisplay, Window root)
{
  GLint attrib[] =
  {
    GLX_RGBA,
    GLX_DEPTH_SIZE, 24,
    GLX_DOUBLEBUFFER,
    None
  };
  XVisualInfo* xvisual_info;
  xvisual_info = glXChooseVisual(xdisplay, 0, attrib);
  if(xvisual_info == NULL)
  {
    printf("no appropriate visual found\n");
    exit(0);
  }

  XWindowAttributes xwindow_attrib;
  XGetWindowAttributes(xdisplay, root, &xwindow_attrib);

  XSetWindowAttributes set_xwindow_attrib;
  set_xwindow_attrib.colormap = XCreateColormap(xdisplay, root, xvisual_info->visual, AllocNone);
  set_xwindow_attrib.event_mask = ExposureMask | KeyPressMask;
  set_xwindow_attrib.override_redirect = 1;

  return XCreateWindow(xdisplay, root, 0, 0, xwindow_attrib.width, xwindow_attrib.height, 0, xvisual_info->depth, InputOutput, xvisual_info->visual, CWBorderPixel|CWColormap|CWEventMask|CWOverrideRedirect, &set_xwindow_attrib );
}










static XImage*
screen_capture(Display* xdisplay, Window root)
{
  XWindowAttributes xwindow_attrib;
  XGetWindowAttributes(xdisplay, root, &xwindow_attrib);
  return XGetImage(xdisplay, root, 0, 0, xwindow_attrib.width, xwindow_attrib.height, AllPlanes, ZPixmap);
}










#define MAJOR_VERSION 3
#define MINOR_VERSION 3

typedef GLXContext (*glXCreateContextAttribsARBProc) (Display*, GLXFBConfig, GLXContext, Bool, const int*);

void
create_gl_context(Display* xdisplay, Window win)
{

  // Create_the_modern_OpenGL_context
  // --------------------------------
  static int xvisual_attribs[] =
  {
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, 
    GLX_DOUBLEBUFFER, 1,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    None
  };
  
  int num_fbc;
  GLXFBConfig* xfbc;
  xfbc = glXChooseFBConfig(xdisplay, DefaultScreen(xdisplay), xvisual_attribs, &num_fbc);
  if (!xfbc)
  {
    printf("glXChooseFBConfig() failed\n");
    exit(1);
  }
  
  XVisualInfo* xvisual_info;
  // Create old OpenGL context to get correct function pointer for glXCreateContextAttribsARB()
  xvisual_info = glXGetVisualFromFBConfig(xdisplay, xfbc[0]);
  
  GLXContext xcontext_old;
  xcontext_old = glXCreateContext(xdisplay, xvisual_info, 0, GL_TRUE);

  glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
  /* Destroy old context */
  glXMakeCurrent(xdisplay, 0, 0);
  glXDestroyContext(xdisplay, xcontext_old);
  if (!glXCreateContextAttribsARB)
  {
    printf("glXCreateContextAttribsARB() not found\n");
    exit(1);
  }

  // Set desired minimum OpenGL version
  static int context_attribs[] =
  {
    GLX_CONTEXT_MAJOR_VERSION_ARB, MAJOR_VERSION,
    GLX_CONTEXT_MINOR_VERSION_ARB, MINOR_VERSION,
    None
  };
  // Create modern OpenGL context
  GLXContext xcontext;
  xcontext = glXCreateContextAttribsARB(xdisplay, xfbc[0], NULL, 1, context_attribs);
  if (!xcontext)
  {
    printf("Failed to create OpenGL context. Exiting.\n");
    exit(1);
  }

  XMapRaised(xdisplay, win);
  glXMakeCurrent(xdisplay, win, xcontext);

  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  printf("OpenGL context created.\nVersion %d.%d\nVendor %s\nRenderer %s\n", major, minor, glGetString(GL_VENDOR), glGetString(GL_RENDERER));

  glewExperimental = GL_TRUE; // Needed for core profile
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    exit(0);
  }

  XWindowAttributes xwindow_attrib;
  XGetWindowAttributes(xdisplay, win, &xwindow_attrib);
  glViewport(0, 0, xwindow_attrib.width, xwindow_attrib.height);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}










GLuint
setup_shaders(Display* xdisplay, Window root, Window win, struct shader_data* data, const char* vertex_shader_file, const char* fragment_shader_file)
{

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  GLuint programID;
  programID = load_shaders_file(vertex_shader_file, fragment_shader_file, data); // Create and compile our GLSL program from the shaders
  glUseProgram(programID);

  GLuint imageDataID = glGetUniformLocation(programID, "imageData");
  glUniform1i(imageDataID, 0);
  glActiveTexture(GL_TEXTURE0);

  GLuint screenSizeID = glGetUniformLocation(programID, "screenSize");
  XWindowAttributes xwindow_attrib;
  XGetWindowAttributes(xdisplay, win, &xwindow_attrib);
  glUniform2f(screenSizeID, xwindow_attrib.width, xwindow_attrib.height);

  static const GLfloat g_vertex_buffer_data[] =
  {
    -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f
  };

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

  XImage* ximage;
  ximage = screen_capture(xdisplay, root);
  if(ximage == NULL)
  {
    printf("\n\tximage could not be created.\n\n");
    exit(0);
  }

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, xwindow_attrib.width, xwindow_attrib.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)(&(ximage->data[0])));
  XDestroyImage(ximage);

  data->timeID = glGetUniformLocation(programID, "time");
  data->endID = glGetUniformLocation(programID, "end");

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT ); // Clear the screen

  return programID;

}










void
animate(Display* xdisplay, Window win, Bool* condition, long* attempts, struct shader_data* data)
{
  float time_s=0, end_s=0;
  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

  glUniform1f(data->endID, end_s);
  while (1)
  {
    time_s=elapsed_time_s(start_time);
    glUniform1f(data->timeID, time_s);
    glXSwapBuffers(xdisplay, win);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    if ( (end_s>0) && ((time_s-end_s)*data->rate>=1.0) ) { // unlock fade out
      break;
    }
    // if ( (end_s==0) && (!*condition) ) { // TODO: unlock attempt fade out and in
    //   glUniform1f(data->endID, end_s);
    //   continue;
    // }
    if ( (end_s==0) && (!*condition) ) { // lock fade in and hold
      end_s = time_s;
      glUniform1f(data->endID, end_s);
      continue;
    }
#if DEBUG
    printf("time_s : %f\n", time_s);
#endif
  }

  printf("Attempts : %ld\n", *attempts);
}