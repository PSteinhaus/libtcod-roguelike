#include "main.hpp"

void Tile::render() const {
	if ( engine.map->isInFov(x,y) )
		TCODConsole::root->setCharForeground(x,y, color() );
	else
		TCODConsole::root->setCharForeground(x,y, TCODColor::darkestGrey );
	TCODConsole::root->setChar( x,y, ch() );
}

bool TreeTile::applyCut(float cutValue) {
	if ( cutValue >= 5 ) {
		// turn into wood
		engine.addActor( ActorRep::getActor( ActorRep::WOOD, x,y ) );
		engine.map->setField(this, GRASS);
	}
	return true;
}
