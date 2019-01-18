#include "main.hpp"
#include <math.h>

// how many turns the monster chases the player after losing his sight
static const int TRACKING_TURNS = 5;

void PlayerAi::update(Actor* owner) {
	if ( owner->destructible && owner->destructible->isDead() ) {
		return;
	}
	int dx = 0, dy = 0;
	numpadMove(&dx, &dy);
	if ( engine.lastKey.vk == TCODK_KP5 ) engine.gameStatus = Engine::NEW_TURN;
	switch(engine.lastKey.vk) {
		case TCODK_CHAR :	handleActionKey(owner, engine.lastKey.c); break;
		default: break;
	}
	if ( dx != 0 || dy != 0 ) {
		engine.gameStatus = Engine::NEW_TURN;
		if ( moveOrAttack(owner, owner->x+dx, owner->y+dy) ) {
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor* owner, int targetX, int targetY) {
	if ( engine.map->isWall(targetX,targetY) ) return false;
	// look for living actors to attack
	for (Actor** iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		if ( actor->destructible && !actor->destructible->isDead() && actor->x == targetX && actor->y == targetY ) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// look for corpses or items
	for (Actor** iterator=engine.actors.begin();
		iterator != engine.actors.end(); iterator++)
	{
		Actor* actor=*iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->pickable;
		if ( corpseOrItem
			&& actor->x == targetX && actor->y == targetY )
		{
			engine.gui->message(TCODColor::white, "There's a %s here",actor->name);
		}
	}
	owner->x = targetX;
	owner->y = targetY;
	return true;
}

void PlayerAi::handleActionKey(Actor* owner, int ascii) {
	switch(ascii) {
		case 'g' : // pickup item
		{
			Actor* topItem = NULL;
			for (Actor** iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
				Actor* actor = *iterator;
				if ( actor->pickable && actor->x == owner->x && actor->y == owner->y ) {
					topItem = actor;
				}
			}
			if (topItem) {
				if (topItem->pickable->pick(topItem,owner)) {
					engine.gui->message(TCODColor::lightGrey,"You pick up the %s.", topItem->name);
				} else {
					engine.gui->message(TCODColor::red,"Your inventory is full.");
				}
			} else {
				engine.gui->message(TCODColor::lightGrey,"There's nothing here that you can pick up.");
			}
			engine.gameStatus = Engine::NEW_TURN;
		}
		break;
		case 'i' : // display inventory
		{
			Actor* actor = choseFromInventory(owner);
			if ( actor ) {
				actor->pickable->use(actor,owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
		}
		break;
		case 'd' : // drop item
		{
			Actor* actor = choseFromInventory(owner);
			if ( actor ) {
				actor->pickable->drop(actor,owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
		}
		break;
		case 'e' : // eat item
		{
			Actor* actor = choseFromInventory(owner);
			if ( actor ) {
				actor->pickable->eat(actor,owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
		}
		break;
		case '<' :
			if ( engine.lastKey.shift ) {
				if ( engine.stairs->x == owner->x && engine.stairs->y == owner->y ) {
					engine.nextLevel();
				} else {
					engine.gui->message(TCODColor::lightGrey,"There are no downstairs here.");
				}
			} else {
				if ( engine.stairs->x == owner->x && engine.stairs->y == owner->y ) {
					//engine.nextLevel();
				} else {
					engine.gui->message(TCODColor::lightGrey,"There are no upstairs here.");
				}
			}
		break;
	}
}

Actor* PlayerAi::choseFromInventory(Actor* owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH,INVENTORY_HEIGHT);
	// display the inventory frame
	con.setDefaultForeground(TCODColor::white);
	con.printFrame(0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,true,TCOD_BKGND_DEFAULT,"inventory");
	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor** it = owner->container->inventory.begin(); it != owner->container->inventory.end(); it++) {
		Actor* actor = *it;
		con.printRect(2,y,INVENTORY_WIDTH-3,0, "(%c) %s", shortcut, actor->name);
		y++;
		shortcut++;
	}
	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0,0,INVENTORY_WIDTH,INVENTORY_HEIGHT,
		TCODConsole::root, engine.screenWidth/2 - INVENTORY_WIDTH/2,
		engine.screenHeight/2 - INVENTORY_HEIGHT/2);
	TCODConsole::flush();
	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
	if ( key.vk == TCODK_CHAR ) {
		int actorIndex = key.c - 'a';
		if ( actorIndex >= 0 && actorIndex < owner->container->inventory.size() ) {
			return owner->container->inventory.get(actorIndex);
		}
	}
	return NULL;
}

void MonsterAi::update(Actor* owner) {
	if ( owner->destructible && owner->destructible->isDead() ) {
		return;
	}
	if ( engine.map->isInFov(owner->x,owner->y) ) {
		// we can see the player. move towards him
		moveCount = TRACKING_TURNS;
	} else {
		moveCount--;
	}

	if ( moveCount > 0 ) {
		moveOrAttack(owner, engine.player->x,engine.player->y);
	}
}

void MonsterAi::moveOrAttack(Actor* owner, int targetX, int targetY) {
	int dx = targetX - owner->x;
	int dy = targetY - owner->y;
	int stepdx = (dx > 0 ? 1:-1);
	int stepdy = (dy > 0 ? 1:-1);
	float distance = sqrt(dx*dx + dy*dy);
	if ( distance >= 2 ) {
		dx = (int)(round(dx/distance));
		dy = (int)(round(dy/distance));
	}
	if ( engine.map->canWalk(owner->x+dx, owner->y+dy) ) {
			owner->x += dx;
			owner->y += dy;
	} else if ( owner->attacker && distance < 2 ) {
		owner->attacker->attack(owner, engine.player);
	} else if ( engine.map->canWalk(owner->x+stepdx,owner->y) ) {
		owner->x += stepdx;
	} else if ( engine.map->canWalk(owner->x, owner->y+stepdy) ) {
		owner->y += stepdy;
	}
}

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai* oldAi) : nbTurns(nbTurns),oldAi(oldAi) {
}

void ConfusedMonsterAi::update(Actor* owner) {
	if ( owner->destructible && owner->destructible->isDead() ) {
		return;
	}
	TCODRandom* rng = TCODRandom::getInstance();
	int dx = rng->getInt(-1,1);
	int dy = rng->getInt(-1,1);
	if ( dx != 0 || dy != 0 ) {
		int destX = owner->x+dx;
		int destY = owner->y+dy;
		if ( engine.map->canWalk(destX,destY) ) {
			owner->x = destX;
			owner->y = destY;
		} else {
			Actor* actor = engine.getActor(destX,destY);
			if ( actor && owner->attacker ) {
				owner->attacker->attack(owner, actor);
			}
		}
	}
	if ( owner == engine.player ) {
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&engine.lastKey,NULL,true);
		engine.gameStatus = Engine::NEW_TURN;
	}
	nbTurns--;
	if (nbTurns == 0) {
		// restore the old Ai
		owner->ai = oldAi;
		delete this;
	}
}