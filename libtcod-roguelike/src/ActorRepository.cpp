#include "main.hpp"



Actor* ActorRep::getActor(ActorRep::Name name, int x, int y) {
	using namespace ActorRep;
	Actor* actor;
	switch (name) {
		case RAT:
			actor = new Actor(x,y,'r',"rat", TCODColor::grey, 3,1000,500);
			actor->destructible = new MonsterDestructible(7,0,"rat corpse");
			actor->attacker = new Attacker(3);
			actor->ai = new MonsterAi();
		break;
		case ORC:
			actor = new Actor(x,y,'o',"orc", TCODColor::desaturatedGreen);
			actor->destructible = new MonsterDestructible(10,0,"dead orc");
			actor->attacker = new Attacker(3);
			actor->ai = new MonsterAi();
		break;
		case TROLL:
			actor = new Actor(x,y,'T',"troll", TCODColor::darkerGreen);
			actor->destructible = new MonsterDestructible(16,1,"troll carcass");
			actor->attacker = new Attacker(4);
			actor->ai = new MonsterAi();
		break;
		default:;
	}
	actor->actorRepName = name;
	return actor;
}

Actor* Rat(int x, int y) {
	Actor *rat = new Actor(x,y,'r',"rat", TCODColor::grey, 3,1000,500);
	rat->destructible = new MonsterDestructible(7,0,"rat corpse");
	rat->attacker = new Attacker(3);
	rat->ai = new MonsterAi();
	return rat;
}

Actor* Orc(int x, int y) {
	Actor *orc = new Actor(x,y,'o',"orc", TCODColor::desaturatedGreen);
	orc->destructible = new MonsterDestructible(10,0,"dead orc");
	orc->attacker = new Attacker(3);
	orc->ai = new MonsterAi();
	return orc;
}

Actor* Troll(int x, int y) {
	Actor *troll = new Actor(x,y,'T',"troll", TCODColor::darkerGreen);
	troll->destructible = new MonsterDestructible(16,1,"troll carcass");
	troll->attacker = new Attacker(4);
	troll->ai = new MonsterAi();
	return troll;
}

Actor* HealthPotion(int x, int y) {
	Actor *item = new Actor(x,y,'!',"health potion", TCODColor::violet);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(NULL,new HealthEffect(4,NULL));
	return item;
}

Actor* LightningScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of lightning bolt", TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(
		new TargetSelector(TargetSelector::CLOSEST_MONSTER,5),
		new HealthEffect(-20,"A lighting bolt strikes the %s with a loud thunder!\nThe damage is %g hit points.") );
	return item;
}

Actor* FireballScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of fireball",TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(
		new TargetSelector(TargetSelector::SELECTED_RANGE,5,3),
		new HealthEffect(-12,"The %s gets burned for %g hit points.") );
	return item;
}

Actor* ConfusionScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of confusion",TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(
		new TargetSelector(TargetSelector::SELECTED_MONSTER,5),
		new ConfusionEffect(8,"The eyes of the %s look vacant,\nas he starts to stumble around!") );
	return item;
}