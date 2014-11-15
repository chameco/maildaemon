COLOR_RED = make_color(1, 0, 0, 1);

EXPLOSION_PARTICLE_UPDATE = fn p {
	set_particle_x(p, get_particle_x(p) + 1);
	set_particle_y(p, get_particle_y(p) + 1);
};

EXPLOSION = build_fx_prototype("explosion", 1,
		fn e {
			print("hello");
			spawn_particle(COLOR_RED, get_effect_x(e), get_effect_y(e), 10, 10, 100, EXPLOSION_PARTICLE_UPDATE);
		});
