#include "main.hpp"

Actor* ActorRep::Rat(int x, int y) {
	Actor *rat = new Actor(x,y,'r',"rat", TCODColor::grey, 3,1000,500);
	rat->destructible = new MonsterDestructible(7,0,"rat corpse");
	rat->attacker = new Attacker(3);
	rat->ai = new MonsterAi();
	return rat;
}

Actor* ActorRep::Orc(int x, int y) {
	Actor *orc = new Actor(x,y,'o',"orc", TCODColor::desaturatedGreen);
	orc->destructible = new MonsterDestructible(10,0,"dead orc");
	orc->attacker = new Attacker(3);
	orc->ai = new MonsterAi();
	return orc;
}

Actor* ActorRep::Troll(int x, int y) {
	Actor *troll = new Actor(x,y,'T',"troll", TCODColor::darkerGreen);
	troll->destructible = new MonsterDestructible(16,1,"troll carcass");
	troll->attacker = new Attacker(4);
	troll->ai = new MonsterAi();
	return troll;
}

Actor* ActorRep::HealthPotion(int x, int y) {
	Actor *item = new Actor(x,y,'!',"health potion", TCODColor::violet);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(NULL,new HealthEffect(4,NULL));
	return item;
}

Actor* ActorRep::LightningScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of lightning bolt", TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(
		new TargetSelector(TargetSelector::CLOSEST_MONSTER,5),
		new HealthEffect(-20,"A lighting bolt strikes the %s with a loud thunder!\nThe damage is %g hit points.") );
	return item;
}

Actor* ActorRep::FireballScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of fireball",TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(
		new TargetSelector(TargetSelector::SELECTED_RANGE,5,3),
		new HealthEffect(-12,"The %s gets burned for %g hit points.") );
	return item;
}

Actor* ActorRep::ConfusionScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of confusion",TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(
		new TargetSelector(TargetSelector::SELECTED_MONSTER,5),
		new ConfusionEffect(8,"The eyes of the %s look vacant,\nas he starts to stumble around!") );
	return item;
}