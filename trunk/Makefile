CC = gcc
OBJECTS = src/main.o src/resources.o src/game.o src/worldgen.o src/weapon.o src/level.o src/lights.o src/player.o src/entity.o src/ai.o src/utils.o src/projectile.o src/gui.o src/fx.o
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = cp -r
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
libdir = $(exec_prefix)/lib
CFLAGS = -Iinclude -std=c99 -g -Wall -Werror -DDATADIR="\"$(DESTDIR)$(datadir)/purge/\""

all: $(OBJECTS) worldgen
	$(CC) $(OBJECTS) -lm -lcuttle -lGLEW -lSDL2 -lGL -lGLU -lSDL2_image -lSDL2_mixer -o maildaemon
worldgen: src/make_world.o src/worldgen.o
	$(CC) src/make_world.o src/worldgen.o -o make_world
install:
	$(INSTALL_PROGRAM) maildaemon $(DESTDIR)$(bindir)/maildaemon
	mkdir -p $(DESTDIR)$(datadir)/maildaemon
	$(INSTALL_DATA) levels textures music fonts $(DESTDIR)$(datadir)/maildaemon/
uninstall:
	rm $(DESTDIR)$(bindir)/maildaemon
	rm $(DESTDIR)$(datadir)/maildaemon -r
clean:
	rm src/*.o
