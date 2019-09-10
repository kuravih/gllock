# gllock - simple screen locker
# See LICENSE file for copyright and license details.

include config.mk

SRC = gllock.c common.c
OBJ = ${SRC:.c=.o}

all: options gllock

options:
	@echo gllock build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

gllock: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f gllock ${OBJ} gllock-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p gllock-${VERSION}
	@cp -R LICENSE Makefile README config.mk ${SRC} gllock-${VERSION}
	@tar -cf gllock-${VERSION}.tar gllock-${VERSION}
	@gzip gllock-${VERSION}.tar
	@rm -rf gllock-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f gllock ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/gllock
	@chmod u+s ${DESTDIR}${PREFIX}/bin/gllock
	@ln -sr shaders ${SHADER_LOCATION}

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/gllock
	@echo removing shader link from ${SHADER_LOCATION}
	@rm -f ${SHADER_LOCATION}/shaders

.PHONY: all options clean dist install uninstall
