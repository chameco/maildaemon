build_item_prototype("laser",
	fn i {
		c = get_item_charge(i);
		if is_item_active(i) {
			if c > 5 {
				spawn_projectile(make_projectile("laser", get_item_x(i) + 12, get_item_y(i), get_item_rotation(i), i));
				set_item_charge(i, c - 5);
				deactivate_item(i);
				return 0;
			};
		};
		if c <= 100 set_item_charge(i, c + 0.1);
	});

