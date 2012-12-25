OBJECTS = src/main.o src/game.o src/level.o src/player.o src/enemy.o src/ai.o src/utils.o src/projectile.o src/gui.o src/fx.o
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = cp -r
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
libdir = $(exec_prefix)/lib
CFLAGS = -Iinclude -g -Wall -DDATADIR="\"$(DESTDIR)$(datadir)/purge/\""

#ENABLE DEBUG MODE
#CFLAGS = -Iinclude -g -Wall -DDATADIR="\"\""

all : $(OBJECTS)
	gcc $(OBJECTS) -lm -lGLEW -lSDL -lGL -lGLU -lSDL_image -lSDL_ttf -o purge
install:
	$(INSTALL_PROGRAM) purge $(DESTDIR)$(bindir)/purge
	mkdir -p $(DESTDIR)$(datadir)/purge
	$(INSTALL_DATA) levels textures music fonts $(DESTDIR)$(datadir)/purge/
uninstall:
	rm $(DESTDIR)$(bindir)/purge
	rm $(DESTDIR)$(datadir)/purge -r
clean:
	rm src/*.o
