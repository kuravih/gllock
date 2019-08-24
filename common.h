#ifndef _COMMON_h
#define _COMMON_h

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/gl.h>

struct shader_data {
  float rate;
  GLuint timeID;
  GLuint endID;
  GLuint rateID;
};

Window fullscreen_win(Display*, Window);
void create_gl_context(Display*, Window);
GLuint setup_shaders(Display* xdisplay, Window root, Window win, struct shader_data* data, const char* vertex_shader_file, const char* fragment_shader_file);
void animate(Display* xdisplay, Window win, Bool* condition, long* attempts, struct shader_data* data);

#endif /* _COMMON_h */


