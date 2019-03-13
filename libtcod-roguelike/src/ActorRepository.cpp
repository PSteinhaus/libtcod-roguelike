#include "main.hpp"

Actor* ActorRep::getActor(ActorRep::Name name, int x, int y) {
	using namespace ActorRep;
	Actor* actor;
	switch (name) {
		case RAT:
			actor = new Actor(x,y,'r',"rat", TCODColor::grey, 3,1000);
			actor->destructible = new MonsterDestructible(7,0,"rat corpse");
			actor->attacker = new Attacker(3);
			actor->ai = new MonsterAi();
			actor->pickable = new Pickable();
			actor->pickable->digestor = new Digestor(500);
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
		case DOWNSTAIRS:
			actor = new Actor(x,y,'>',"downstairs",TCODColor::white);
			actor->fovOnly = false;
			actor->blocks = false;
		break;
		case UPSTAIRS:
			actor = new Actor(x,y,'<',"upstairs",TCODColor::white);
			actor->fovOnly = false;
			actor->blocks = false;
		break;
		case DOOR:
			actor = new Actor(x,y,'+',"door",TCODColor::darkerOrange);
			actor->fovOnly = false;
			actor->transparent = false;
			actor->interactable = new Useable(
				new SelectEffectCarrier(),
				new DoorEffect( actor->ch ) );
		break;
		case AXE:
		{
			const float weaponDamage = 3;
			actor = new Actor(x,y,'/',"axe",TCODColor::darkGrey);
			actor->blocks = false;
			actor->pickable = new Pickable();
			actor->pickable->useable = new Useable(
				new SelectNumpadAdjecent(false),
				new ApplyCutEffect(5) );
			actor->pickable->equipable = new Equipable( BodyPart::HAND,
				new ChangeMeleeDamage(weaponDamage),
				new ChangeMeleeDamage(-weaponDamage) );
			actor->pickable->equipable->implicitlyUseable = true;
		}
		break;
		case WOOD:
			actor = new Actor(x,y,'/',"wood",TCODColor::darkerOrange);
			actor->blocks = false;
			actor->pickable = new Pickable();
		break;
		default:;
	}
	actor->actorRepName = name;
	return actor;
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
		new SelectClosestMonster(5),
		new HealthEffect(-20,"A lighting bolt strikes the %s with a loud thunder!\nThe damage is %g hit points.") );
	return item;
}

Actor* FireballScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of fireball",TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	item->pickable->useable = new Useable(
		new SelectFieldRange(5,3),
		new HealthEffect(-12,"The %s gets burned for %g hit points.") );
	return item;
}

Actor* ConfusionScroll(int x, int y) {
	Actor *item = new Actor(x,y,'?',"scroll of confusion",TCODColor::lightYellow);
	item->blocks = false;
	item->pickable = new Pickable();
	auto selector = new SelectField(5);
	selector->aliveRequired = true;
	item->pickable->useable = new Useable(
		selector,
		new ConfusionEffect(8,"The eyes of the %s look vacant,\nas he starts to stumble around!") );
	return item;
}