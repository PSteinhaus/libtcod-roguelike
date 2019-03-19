#include "main.hpp"

void Tile::render(TCODConsole* con, int xOffset, int yOffset) const {
	if ( engine.camera->isOnCamera(x,y) ) {
		const int renderX = x-xOffset;
		const int renderY = y-yOffset;
		if ( engine.map->isInFov(x,y) )
			con->setCharForeground(renderX,renderY, color() );
		else
			con->setCharForeground(renderX,renderY, TCODColor::darkestGrey );
		con->setChar( renderX,renderY, ch() );
	}
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