HUMAN_HP = 100;
HUMAN_SPEED = 6;
HUMAN_EXP_VALUE = 10;

HUMAN_CIVILIAN = build_entity_prototype("human_civilian", 32, 32, HUMAN_HP, HUMAN_SPEED, HUMAN_EXP_VALUE);
set_entity_collide(HUMAN_CIVILIAN, fn e, data set_current_dialog("Watch it!"));
