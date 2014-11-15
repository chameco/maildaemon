SMOKE_BLACK = make_color(0, 0, 0, 1);
SMOKE_GRAY = make_color(0.1, 0.1, 0.1, 1);

SMOKE_PARTICLE_UPDATE = fn p {
	set_particle_x(p, get_particle_x(p) + random_integer(5) - 2);
	set_particle_y(p, get_particle_y(p) - 1);
};

SMOKE = build_fx_prototype("smoke", -1,
		fn e {
			spawn_particle(SMOKE_BLACK, get_effect_x(e) + random_integer(11) - 5, get_effect_y(e), 8, 8, 50, SMOKE_PARTICLE_UPDATE);
		});
