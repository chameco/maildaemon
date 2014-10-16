STAIRS = build_entity_prototype("stairs", 32, 32, 0, 0, 0);
set_entity_hit(STAIRS, fn e, dmg, data data);

make_stairs = fn x, y, sr, sc, dest, destx, desty {
	s = make_entity("stairs", x, y);
	set_entity_collide(s, fn e schedule(fn ~ do
						warp_player(destx, desty);
						switch_level(dest);
					    end, 0));
	set_sheet_row(get_entity_texture(s), sr);
	set_sheet_column(get_entity_texture(s), sc);
	return s;
};
