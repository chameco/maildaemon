define_standard_gui_mode(
	"main_menu",
	fn ~ {
		reset_gui();
		spawn_label(-(/(get_screen_width(), 2), 640), 100, "maildaemon", 16.0);
		spawn_image(-(/(get_screen_width(), 2), 500), -(/(get_screen_height(), 2), 100), "textures/ampersand.png");
		spawn_image(+(/(get_screen_width(), 2), 180), -(/(get_screen_height(), 2), 100), "textures/ampersand.png");
		spawn_button(
			-(/(get_screen_width(), 2), 100),
			-(/(get_screen_height(), 2), 100),
			"continue",
			fn ~ if load_game(1) schedule(fn ~ set_mode("main"), 0));
#schedule(fn ~ set_mode("not_found"), 0));
		spawn_button(
			-(/(get_screen_width(), 2), 100),
			-(/(get_screen_height(), 2), 25),
			"new game",
			fn ~ schedule(fn ~ set_mode("new_game"), 0));
		spawn_button(
			-(/(get_screen_width(), 2), 100),
			+(/(get_screen_height(), 2), 50),
			"quit",
			fn ~ set_running(false));
	}
);
