Purge
=====
A modular (sort of) 2D game (engine|framework) for making most any kind of top-down game written in C using OpenGL, SDL, and GLEW.
How it's organized
------------------
Purge is basically set up as a set of C "modules". Right now, these modules are basically just a C source file and a header, contained in the directories `/trunk/include` and `/trunk/src`, respectively. Eventually I'm going to implement a true module system that loads shared object files, or maybe even invent my own format for saving modules (ELF sucks!). All modules are exposed entirely using functions - no `extern`s or global variable nastiness! There are several exceptions to this rule (mostly constants in `utils.h` and `utils.c`, but that doesn't really count because `utils` isn't really structured as a module). Right now, all modules contain the following functions:
- `initialize_<module name>`: Called once, at startup. Mostly used for initializing variables, VBOs, etc.
- `update_<module name>`: Called once every 50 milliseconds. Used for time dependent things, e.g. physics, movement.
- `draw_<module name>`: Called once every tick of the main loop. Used for neat things that need to go fast (particle effects), and drawing stuff, obviously.

List of current modules:
- `level`: Load, draw, and manage levels.
- `player`: Manage the player's movement, shoot the player's weapons, draw the player.
- `entity`: Possibly-moving objects that don't have AIs. Essentially blocks with custom hit/collision features.
- `enemy`: Draw enemies, manage enemy projectiles and movement.
- `ai`: Enemy AIs.
- `projectile`: Manage projectiles, draw projectiles, shoot projectiles.
- `fx`: Particle effects! Right now, smoke and explosions are implemented.
- `gui`: Graphical user interface elements, namely the HUD as of now.
- `light`: Dynamic lighting
