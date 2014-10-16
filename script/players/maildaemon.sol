set_player_stat("move_speed", 6);
set_player_stat("diag_speed", 5);
set_player_stat("width", get_tile_dim());
set_player_stat("height", get_tile_dim()/2);
set_player_stat("level", 0);
set_player_stat("exp", 0);
set_player_stat("exp_to_next", 100);
set_player_stat("health", 100);
set_player_stat("max_health", 100);
set_player_stat("regen", 0.1);

set_player_item(1, make_item("laser"));
#set_player_item(2, make_item("flamethrower"))

set_player_item_index(1);
