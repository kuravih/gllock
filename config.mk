# gllock version
VERSION = 0.1-alpha

# Customize below to fit your system

# paths
SHADER_LOCATION = $(HOME)/.gllock

# shader
FRGMNT_SHADER = circle.fragment.glsl
# FRGMNT_SHADER = square.fragment.glsl
# FRGMNT_SHADER = glitch.fragment.glsl
# FRGMNT_SHADER = ascii.fragment.glsl
# FRGMNT_SHADER = crt.fragment.glsl
# FRGMNT_SHADER = rain.fragment.glsl
# FRGMNT_SHADER = bokeh.fragment.glsl
# FRGMNT_SHADER = radialbokeh.fragment.glsl
# FRGMNT_SHADER = blur.fragment.glsl

PREFIX = /usr/local

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# includes and libs
INCS = -I. -I/usr/include -I${X11INC}
LIBS = -L/usr/lib -lc -lcrypt -L${X11LIB} -lX11 -lGL -lGLEW -lpthread

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -DHAVE_SHADOW_H -DSHADER_LOCATION=\"${SHADER_LOCATION}\" -DFRGMNT_SHADER=\"${FRGMNT_SHADER}\"
CFLAGS = -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS = -s ${LIBS}

# On *BSD remove -DHAVE_SHADOW_H from CPPFLAGS and add -DHAVE_BSD_AUTH
# On OpenBSD and Darwin remove -lcrypt from LIBS

# compiler and linker
CC = cc

# Install mode. On BSD systems MODE=2755 and GROUP=auth
# On others MODE=4755 and GROUP=root
#MODE=2755
#GROUP=auth
