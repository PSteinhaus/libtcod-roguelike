#include "main.hpp"

void Tile::render() const {
	if ( engine.map->isInFov(x,y) )
		TCODConsole::root->setCharForeground(x,y, color() );
	else
		TCODConsole::root->setCharForeground(x,y, TCODColor::darkestGrey );
	TCODConsole::root->setChar( x,y, ch() );
}

Tile* Tile::createByType(FieldType type, int x, int y) {
	Tile* tile = NULL;
	switch(type) {
		case FLOOR : tile = new FloorTile(x,y); break;
		case WALL : tile = new WallTile(x,y); break;
		case GRASS: tile = new GrassTile(x,y); break;
		case TREE: tile = new TreeTile(x,y); break;
		case WATER: tile = new WaterTile(x,y); break;
	}
	return tile;
}

bool TreeTile::applyCut(float cutValue) {
	if ( cutValue >= 5 ) {
		// turn into wood
		engine.addActor( ActorRep::getActor( ActorRep::WOOD, x,y ) );
		engine.map->setField(this, GRASS);
	}
	return true;
}