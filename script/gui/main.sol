HUD_GREEN = make_color(0, 1, 0, 1);
HUD_GRAY = make_color(0.1, 0.1, 0.1, 1);
HUD_GOLD = make_color(1, 0.8, 0, 1);
fn ~ {
	reset_gui();
	spawn_meter(0, 0, "Health", HUD_GREEN, fn ~ get_player_stat("health") * 100 / get_player_stat("max_health"));
	spawn_meter(0, 32, "Ammo", HUD_GRAY, fn ~ get_item_charge(get_player_item()));
#spawn_dynamic_meter(0, 64, fn ~ concat("lvl ", get_player_stat("level")), HUD_GOLD, fn ~ /(get_player_stat("exp") * 100, get_player_stat("exp_to_next")));
	spawn_dynamic_label(150, 0, get_level_name, 4);
	spawn_dialog_box(-(get_screen_width() / 2, 500), get_screen_height() - 100, get_current_dialog);
};
