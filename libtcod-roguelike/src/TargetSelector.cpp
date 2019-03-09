#include "main.hpp"

TargetSelector::TargetSelector( bool targetActors, bool targetTiles)
: aliveRequired(false), singleOnly(false), targetActors(targetActors), targetTiles(targetTiles)
{}

void TargetSelector::addTargets(int x, int y, TCODList<Actor*>& list, TCODList<Tile*>& tileList) {
	if( targetActors ){
		auto baseList = engine.getActors(x,y, aliveRequired);
		if( singleOnly ) list.push(baseList.pop());
		else list.addAll(baseList);
	}
	if(targetTiles)  tileList.push(engine.tileAt(x,y));
}


SelectClosestMonster::SelectClosestMonster(float range, bool targetActors, bool targetTiles)
: TargetSelector::TargetSelector(targetActors,targetTiles), range(range) {
	selectorType = CLOSEST_MONSTER;
	singleOnly = true;
}

void SelectClosestMonster::selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly ) {
	Actor *closestMonster=engine.getClosestMonster(user->x,user->y,range);
	if ( closestMonster ) {
		addTargets(closestMonster->x,closestMonster->y, list, tileList);
	}
}

SelectField::SelectField(float range, bool targetActors, bool targetTiles)
: TargetSelector::TargetSelector(targetActors,targetTiles), range(range) {
	selectorType = SELECTED_MONSTER;
}

void SelectField::selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly ) {
	int x,y;
	engine.gui->message(TCODColor::cyan, "Choose target,\npress Esc to cancel.");
	if ( engine.pickATile(&x,&y,range)) {
		addTargets(x,y, list, tileList);
	}
}

RangeAroundUser::RangeAroundUser(float range, bool includeUser, bool targetActors, bool targetTiles)
: TargetSelector::TargetSelector(targetActors,targetTiles), range(range) {
	selectorType = USER_RANGE;
}

void RangeAroundUser::selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly ) {
	engine.entitiesInRange( user->x,user->y, range, aliveRequired, targetActors ? &list : NULL, targetTiles ? &tileList : NULL );
	if ( !includeUser ) list.remove(user);
}

SelectFieldRange::SelectFieldRange(float range, float areaRange, bool targetActors, bool targetTiles)
: TargetSelector::TargetSelector(targetActors,targetTiles), range(range), areaRange(areaRange) {
	selectorType = SELECTED_RANGE;
}

void SelectFieldRange::selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly ) {
	int x,y;
	engine.gui->message(TCODColor::cyan, "Choose target,\npress Esc to cancel.");
	if ( engine.pickATile(&x,&y,range))
		engine.entitiesInRange( user->x,user->y, range, aliveRequired, targetActors ? &list : NULL, targetTiles ? &tileList : NULL );
}

SelectEffectCarrier::SelectEffectCarrier(bool targetActors, bool targetTiles)
: TargetSelector::TargetSelector(targetActors,targetTiles) {
	selectorType = EFFECT_CARRIER;
}

void SelectEffectCarrier::selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly ) {
	list.push(owner);
}

SelectNumpadAdjecent::SelectNumpadAdjecent( bool acceptCenter, bool targetActors, bool targetTiles)
: TargetSelector::TargetSelector(targetActors,targetTiles), acceptCenter(acceptCenter) {
	selectorType = NUMPAD;
}

void SelectNumpadAdjecent::selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly ) {
	if ( !implicitly )
		engine.gui->message(TCODColor::lightGrey, "Use where?");
	int x=user->x;
	int y=user->y;
	if ( engine.waitForDirection(acceptCenter) ) {	
		numpadMove(&x,&y); 					// check into which direction the player is pressing right now
		addTargets(x,y, list, tileList);
	}
}