spawn_torch := fn x, y {
	spawn_lightsource(make_lightsource(x + 16, y + 16, 200, 3, make_color(0.7, 0.7, 0.5, 1)));
	spawn_fx(make_fx("smoke", x + 16, y));
};

spawn_outside_torch := fn x, y {
	spawn_lightsource(make_lightsource(x + 16, y + 16, 200, 1, make_color(0.7, 0.7, 0.5, 1)));
#spawn_fx(make_fx(1, make_color(0.1, 0.1, 0.1, 1), x + 16, y, 8, 50, 100));
};
