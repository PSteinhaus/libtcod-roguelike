#include "main.hpp"

TargetSelector::TargetSelector(SelectorType type, float range, float areaRange) : type(type), range(range), areaRange(areaRange) {
}

void TargetSelector::selectTargets(Actor *user, Actor *owner, TCODList<Actor *> & list) {
	switch(type) {
		case CLOSEST_MONSTER :
		{
			Actor *closestMonster=engine.getClosestMonster(user->x,user->y,range);
			if ( closestMonster ) {
				list.push(closestMonster);
			}
		}
		break;
		case SELECTED_MONSTER :
		{
			int x,y;
			engine.gui->message(TCODColor::cyan, "Choose target,\npress Esc to cancel.");
			if ( engine.pickATile(&x,&y,range)) {
				Actor *actor=engine.getActor(x,y);
				if ( actor ) {
					list.push(actor);
				}
			}
		}
		break;
		case USER_RANGE :
		{
			for (auto iterator=engine.actorsBegin(); iterator != engine.actorsEnd(); iterator++)
			{
				Actor* actor=*iterator;
				if ( actor != user && actor->destructible && !actor->destructible->isDead()
					&& actor->getDistance(user->x,user->y) <= range)
				{
					list.push(actor);
				}
			}
		}
		break;
		case SELECTED_RANGE :
			int x,y;
			engine.gui->message(TCODColor::cyan, "Choose target,\npress Esc to cancel.");
			if ( engine.pickATile(&x,&y,range)) {
				for (auto iterator=engine.actorsBegin(); iterator != engine.actorsEnd(); iterator++) {
					Actor *actor=*iterator;
					if ( actor->destructible && !actor->destructible->isDead() && actor->getDistance(x,y) <= areaRange) {                
						list.push(actor);
					}
				}
			}
		break;
		case EFFECT_CARRIER :
			list.push(owner);
		break;
	}
	if ( list.isEmpty() ) {
		 engine.gui->message(TCODColor::lightGrey,"No target found.");
	}
}