#ifndef _COMMON_h
#define _COMMON_h

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/gl.h>

enum ease_enum { in, cts, out};
typedef enum ease_enum ease_t;

Window fullscreen_win(Display*, Window);
void create_gl_context(Display*, Window);
GLuint setup_shaders(Display* xdisplay, Window root, Window win, const char* vertex_shader_file, const char* fragment_shader_file);
void animate(Display* xdisplay, Window win, ease_t ease, GLuint timeID);
void animate_in(Display* xdisplay, Window win, GLuint timeID);
void animate_cts(Display* xdisplay, Window win, Bool* condition, GLuint timeID);
void animate_out(Display* xdisplay, Window win, GLuint timeID);

#endif /* _COMMON_h */


