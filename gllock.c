
/* See LICENSE file for license details. */
#define _XOPEN_SOURCE 500

#if HAVE_SHADOW_H
#include <shadow.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "common.h"

#include <time.h>
#include <pthread.h>

#include <string.h>
#include <stdbool.h>

#include <crypt.h>

#include <GL/glew.h>
#include <GL/glx.h>

#if HAVE_BSD_AUTH
#include <login_cap.h>
#include <bsd_auth.h>
#endif



Display* xdisplay;

typedef struct
{
  int screen;
  Window root, win;
  pthread_t tid;
  Bool hold;
  long attempts;
} lock_t;

static lock_t* locks;
static int nscreens;
static Bool running = True;










static void
die(const char *errstr, ...)
{
  va_list ap;

  va_start(ap, errstr);
  vfprintf(stderr, errstr, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}










#ifndef HAVE_BSD_AUTH
static const char*
getpw(void)
{ /* only run as root */
  const char *rval;
  struct passwd *pw;

  pw = getpwuid(getuid());
  if(!pw)
    die("gllock: cannot retrieve password entry (make sure to suid or sgid gllock)");
  endpwent();
  rval =  pw->pw_passwd;

#if HAVE_SHADOW_H
  if (strlen(rval)>=1)
  { /* kludge, assumes pw placeholder entry has len >= 1 */
    struct spwd *sp;
    sp = getspnam(getenv("USER"));
    if(!sp)
      die("gllock: cannot retrieve shadow entry (make sure to suid or sgid gllock)\n");
    endspent();
    rval = sp->sp_pwdp;
  }
#endif

  /* drop privileges */
  if((setgid(pw->pw_gid)<0)||(setuid(pw->pw_uid)<0))
    die("gllock: cannot drop privileges");
  return rval;
}
#endif










static void
#ifdef HAVE_BSD_AUTH
readpw(void)
#else
readpw(const char* pws)
#endif
{
  char buf[32], passwd[256];
  int num, screen;
  unsigned int len, llen;
  KeySym ksym;
  XEvent ev;

  len = llen = 0;
  running = True;

  /* As "gllock" stands for "Simple X display locker".
   * The DPMS settings had been removed and you can set it with "xset" or some other utility.
   * This way the user can easily set a customized DPMS timeout. */
  while(running && !XNextEvent(xdisplay, &ev))
  {

    if(ev.type == KeyPress)
    {
      buf[0] = 0;
      num = XLookupString(&ev.xkey, buf, sizeof buf, &ksym, 0);

      if(IsKeypadKey(ksym))
      {
        if(ksym == XK_KP_Enter)
          ksym = XK_Return;
        else if(ksym >= XK_KP_0 && ksym <= XK_KP_9)
          ksym = (ksym - XK_KP_0) + XK_0;
      }

      if(IsFunctionKey(ksym) || IsKeypadKey(ksym) || IsMiscFunctionKey(ksym) || IsPFKey(ksym) || IsPrivateKeypadKey(ksym))
        continue;

      switch(ksym)
      {
      case XK_Return:
        passwd[len] = 0;

#ifdef HAVE_BSD_AUTH
        running = !auth_userokay(getlogin(), NULL, "auth-xlock", passwd);
#else
        running = strcmp(crypt(passwd, pws), pws);
#endif

        if(running != False) // Wrong password
        {
          XBell(xdisplay, 100);
          for(screen = 0; screen < nscreens; screen++)
          {
            locks[screen].attempts++;
          }
        }

        len = 0;
        break;

      case XK_Escape:
        len = 0;
        break;

      case XK_BackSpace:
        if(len)
          --len;
        break;

      default:
        if(num && !iscntrl((int) buf[0]) && (len + num < sizeof passwd))
        {
          memcpy(passwd + len, buf, num);
          len += num;
        }
        break;

      }

      if(llen == 0 && len != 0)
      {
        for(screen = 0; screen < nscreens; screen++)
        {
          // characters on the buffer
        }
      }
      else if(llen != 0 && len == 0)
      {
        for(screen = 0; screen < nscreens; screen++)
        {
          // Wrong password
        }
      }
      llen = len;
    }
    else
      for(screen = 0; screen < nscreens; screen++)
        XRaiseWindow(xdisplay, locks[screen].win);

  }
}










static void
unlockscreen(lock_t* lock)
{
  if(xdisplay == NULL || lock == NULL)
    return;

  lock->hold = False;

  pthread_join(lock->tid, NULL);

  XUngrabPointer(xdisplay, CurrentTime);
  XDestroyWindow(xdisplay, lock->win);
}










void*
animation_runner(void* arg)
{
  lock_t* lock = (lock_t*) arg;

  XLockDisplay(xdisplay);

  create_gl_context(xdisplay, lock->win);

  struct shader_data data;
  setup_shaders(xdisplay, lock->root, lock->win, &data, SHADER_LOCATION"/passthrough.vertex.glsl", SHADER_LOCATION"/"FRGMNT_SHADER);

  XUnlockDisplay(xdisplay);

  animate(xdisplay, lock->win, &(lock->hold), &(lock->attempts), &data);

  return NULL;
}










int
lockscreen(lock_t* lock) {
  unsigned int len;
  Cursor invisible = 0;

  lock->hold = True;
  lock->attempts = 0;

  XLockDisplay(xdisplay);

  lock->win = fullscreen_win(xdisplay, lock->root);

  XUnlockDisplay(xdisplay);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(&(lock->tid), NULL, animation_runner, lock);

  for(len = 1000; len; len--)
  {
    if(XGrabPointer(xdisplay, lock->root, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, invisible, CurrentTime) == GrabSuccess)
      break;
    usleep(1000);
  }
  if(running && (len > 0))
  {
    for(len = 1000; len; len--)
    {
      if(XGrabKeyboard(xdisplay, lock->root, True, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess)
        break;
      usleep(1000);
    }
  }

  running &= (len > 0);
  if(!running)
  {
    unlockscreen(lock);
    lock = NULL;
  }
  else
  {
    XSelectInput(xdisplay, lock->root, SubstructureNotifyMask);
    XSync(xdisplay, False);
  }

  return 1;
}










static void
usage(void)
{
  fprintf(stderr, "usage: gllock [-v]\n");
  exit(EXIT_FAILURE);
}










int
main(int argc, char **argv)
{
#ifndef HAVE_BSD_AUTH
  const char* password;
#endif
  int screen;

  {
    int result;
    while((result = getopt(argc,argv,"vo:")) != -1)
    {
      switch(result)
      {
        case 'v':
          die("gllock-%s, © 2019 Kuravi H\n", VERSION);
        case '?':
          usage();
          break;
      }
    }
    if((argc - optind) > 0)
      usage();
  }


  if(!getpwuid(getuid()))
    die("gllock: no passwd entry for you");


#ifndef HAVE_BSD_AUTH
  password = getpw();
#endif

  XInitThreads();

  if(!(xdisplay = XOpenDisplay(0)))
    die("gllock: cannot open display");


  /* Get the number of screens in display "xdisplay" and blank them all. */
  nscreens = ScreenCount(xdisplay);
  locks = malloc(nscreens*sizeof(lock_t));
  for(screen = 0; screen < nscreens; screen++)
  {
    locks[screen].screen = screen;
    locks[screen].root = RootWindow(xdisplay, locks[screen].screen);
  }
  if(locks == NULL)
    die("gllock: malloc: %s", strerror(errno));


  /* lock */
  int nlocks = 0;
  for(screen = 0; screen < nscreens; screen++)
  {
    if( lockscreen(&(locks[screen])) )
      nlocks++;
  }
  XSync(xdisplay, False);


  /* Did we actually manage to lock something? */
  if(nlocks == 0)
  { // nothing to protect
    free(locks);
    XCloseDisplay(xdisplay);
    return 1;
  }

  /* Everything is now blank. Now wait for the correct password. */
#ifdef HAVE_BSD_AUTH
  readpw();
#else
  readpw(password);
#endif

  /* Password ok, unlock everything and quit. */
  for(screen = 0; screen < nscreens; screen++)
    unlockscreen(&(locks[screen]));

  free(locks);
  XCloseDisplay(xdisplay);

  return 0;
}
