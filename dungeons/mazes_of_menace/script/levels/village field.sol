spawn_entity(@make_stairs(320, 0, 1, 0, "church", 304, 576));

@spawn_outside_torch(256, 0);
@spawn_outside_torch(384, 0);

spawn_entity(make_entity("human_civilian", 256, 288));

spawn_entity(make_entity("acid_blob", 256, 288));
spawn_entity(make_entity("gelatinous_cube", 288, 288));

set_current_dialog("Entering the dungeons of doom is unsafe.
Find a better hobby, adventurer.");
