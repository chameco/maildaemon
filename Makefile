CC = gcc
SRCS = $(filter-out main.c level_comp.c,$(foreach file,$(wildcard src/*),$(notdir $(file))))
GAME = maildaemon
BUILD_DIR = build_$(CC)
LIB_DIR = lib_$(CC)
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))

INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = cp -r

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
libdir = $(exec_prefix)/lib

CFLAGS_gcc = -Iinclude -std=c99 -g -Wall -Werror `pkg-config guile-2.0 --cflags`
CFLAGS_clang = -Iinclude -std=c99 -g -Wall -Werror `pkg-config guile-2.0 --cflags`
CFLAGS_x86_64-w64-mingw32-gcc = -static -DGLEW_STATIC -Iinclude -g -Wall
CFLAGS = $(CFLAGS_$(CC))
LINKER_FLAGS_gcc = -Wl,-z,origin '-Wl,-rpath,$$ORIGIN/$(LIB_DIR):$$ORIGIN/steam-runtime/amd64/lib/x86_64-linux-gnu:$$ORIGIN/steam-runtime/amd64/lib:$$ORIGIN/steam-runtime/amd64/usr/lib/x86_64-linux-gnu:$$ORIGIN/steam-runtime/amd64/usr/lib' \
	-L$(LIB_DIR)/ -lm -lcuttle `pkg-config guile-2.0 --libs` -lGLEW -lSDL2 -lGL -lGLU -lSDL2_image -lSDL2_mixer
LINKER_FLAGS_clang = -Wl,-z,origin '-Wl,-rpath,$$ORIGIN/$(LIB_DIR):$$ORIGIN/steam-runtime/amd64/lib/x86_64-linux-gnu:$$ORIGIN/steam-runtime/amd64/lib:$$ORIGIN/steam-runtime/amd64/usr/lib/x86_64-linux-gnu:$$ORIGIN/steam-runtime/amd64/usr/lib' \
	-L$(LIB_DIR)/ -lm -lcuttle `pkg-config guile-2.0 --libs` -lGLEW -lSDL2 -lGL -lGLU -lSDL2_image -lSDL2_mixer
LINKER_FLAGS = $(LINKER_FLAGS_$(CC))

vpath %.c src

.PHONY: all directories install uninstall clean

all: directories $(GAME) level_comp

directories: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $<

$(GAME): $(BUILD_DIR)/main.o $(OBJS)
	$(CC) $^ $(LINKER_FLAGS) -o $@

level_comp: $(BUILD_DIR)/level_comp.o $(OBJS)
	$(CC) $^ $(LINKER_FLAGS) -o $@

install:
	$(INSTALL_PROGRAM) maildaemon $(DESTDIR)$(bindir)/maildaemon
	mkdir -p $(DESTDIR)$(datadir)/maildaemon
	$(INSTALL_DATA) levels textures music fonts $(DESTDIR)$(datadir)/maildaemon/

uninstall:
	rm $(DESTDIR)$(bindir)/maildaemon
	rm $(DESTDIR)$(datadir)/maildaemon -r

clean:
	rm $(BUILD_DIR)/*.o
