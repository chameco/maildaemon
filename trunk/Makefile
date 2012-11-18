OBJECTS = main.o game.o level.o character.o enemy.o utils.o projectile.o gui.o
CFILES = src/main.c src/game.c src/level.c src/character.c src/enemy.c src/utils.c src/projectile.c src/gui.c
CFLAGS = -Iinclude -g -Wall
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
libdir = $(exec_prefix)/lib

all : objects
	gcc $(OBJECTS) -lSDL -lGL -lGLU -lSDL_image -lSDL_ttf -o purge
objects : $(CFILES)
	gcc $(CFILES) -Iinclude -g -Wall -c
install:
	$(INSTALL_PROGRAM) purge $(DESTDIR)$(bindir)purge
uninstall:
	rm $(DESTDIR)$(bindir)/purge
clean:
	rm *.o
