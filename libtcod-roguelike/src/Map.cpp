#include "main.hpp"
#include <cmath>
#include <stdio.h>
#include <map>

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

// ###########################
// # STANDARD TUTORIAL STUFF #
// ###########################

class BspListener : public ITCODBspCallback {
private:
	Map& map; // a map to dig
	int roomNum; // room number
	int lastX, lastY; // center of the last room

public:
	bool addStuff;
	float dig_rnd_mod; int dig_rnd_centricity;
	enum { DIG_RND, CONNECT_RND, DEFAULT } state;

	BspListener(Map& map) : map(map),roomNum(0),addStuff(true),dig_rnd_mod(1),dig_rnd_centricity(40) {}
	bool visitNode(TCODBsp* node, void* userData) {
		switch(state) {
			case DIG_RND: // dig a random pattern starting in the middle of this node
			{
				if ( node->isLeaf() ) {
					int x = node->x+(node->w/2);
					int y = node->y+(node->h/2);
					int xStep, yStep;
					TCODRandom* rng = TCODRandom::getInstance();
					for(float i=0; i<( pow(node->w*node->h,2)*dig_rnd_mod/1000 ); i++) {
						map.setField(x,y, Tile::FieldType::FLOOR);
						if ( rng->getInt(1,100) > dig_rnd_centricity ) { // make sure not to stray too far from the center
							do {
								xStep = rng->getInt(-1,1);
								yStep = rng->getInt(-1,1);
							} while ( !map.inMap(x+xStep,y+yStep, false) );
							x += xStep;
							y += yStep;
						} else {
							x += x < node->x+(node->w/2) ? 1 : -1;
							y += y < node->y+(node->h/2) ? 1 : -1;							
						}
					}
					if (addStuff) map.fillRoom(node->x, node->y, node->x+node->w-1, node->y+node->h-1, roomNum == 0);
					roomNum++;
				}
			}
			break;
			case CONNECT_RND:
			{
				TCODBsp* sibling = node->getLeft();
				if ( sibling != NULL ) {
					TCODPath* path = static_cast<TCODPath*>(userData);
					int thickness = 2;
					if ( !node->isLeaf() ) { // node and (maybe) sibling are parents; replace each with one of their (already interconnected) children
						//if ( !sibling->isLeaf() ) thickness = 2;
						int x1 = node->x+node->w-1; int y1 = node->y+node->h-1;
						int x2 = sibling->x+sibling->w-1; int y2 = node->y+node->h-1;
						node = node->findNode(node->x+node->w-1,node->y+node->h-1);
						sibling = sibling->findNode(sibling->x+sibling->w-1,sibling->y+sibling->h-1);
						if ( node==sibling ) printf("Node at %d,%d is node at %d,%d.\n", x1,y1, x2,y2 );
					}
					while ( !path->compute(node->x+(node->w/2),node->y+(node->h/2),
										sibling->x+(sibling->w/2),sibling->y+(sibling->h/2) )  ) {
						// no connection found, so establish one
						map.connectRoomsRandom(node->x+(node->w/2),node->y+(node->h/2),1,1,
											sibling->x+(sibling->w/2),sibling->y+(sibling->h/2),1,1, 40, thickness);
					}
				}
			}
			break;
			case DEFAULT:
			{
				if ( node->isLeaf() ) {
					int x,y,w,h;
					// dig a room
					TCODRandom *rng=TCODRandom::getInstance();
					w = rng->getInt(ROOM_MIN_SIZE, node->w-2);
					h = rng->getInt(ROOM_MIN_SIZE, node->h-2);
					x = rng->getInt(node->x+1, node->x+node->w-w-1);
					y = rng->getInt(node->y+1, node->y+node->h-h-1);
					map.dig (x, y, x+w-1, y+h-1);
					map.fillRoom(x, y, x+w-1, y+h-1, roomNum == 0);
				
					if ( roomNum != 0 ) {
						// dig a corridor from last room
						map.dig(lastX, lastY, x+w/2, lastY);
						map.dig(x+w/2, lastY, x+w/2, y+h/2);
					}
					lastX = x+w/2;
					lastY = y+h/2;
					roomNum++;
				}
			}
			break;
		}		
		return true;
	}
};

void Map::addMonster(int x, int y) {
	// choose a random creature from this chunks biome
	int biomeSize = chunk->biomeData.creatures.size();
	if (biomeSize != 0) {
		// check if there is any creature left at all
		for (auto it = chunk->biomeData.creatures.begin(); it != chunk->biomeData.creatures.end(); it++)
			if (it->second != 0) goto ok;
		return;
		
		ok:
		TCODRandom* rng = TCODRandom::getInstance();
		auto it = chunk->biomeData.creatures.begin();
		std::advance(it, rng->getInt( 0, biomeSize-1 ) );
		auto name = it->first;
		while (it->second <= 0) {
			it = chunk->biomeData.creatures.begin();				//
			std::advance(it, rng->getInt( 0, biomeSize-1 ) );		// choose
			name = it->first;										//
		}
		it->second--;												// load OFF the biomeData
		engine.addActor( ActorRep::getActor(name, x,y) );		// place into the map (or engine as of now)
	}
}

void Map::addItem(int x, int y) {
	/*	// on ice, until I figure out what items I want and how to distribute them
	if (item) engine.actors.insertBefore(item,0); 
	*/
}

void Map::addActor(Actor* actor) {
	computeTCODMapAt(actor->x,actor->y);
}

void Map::removeActor(Actor* actor) {
	computeTCODMapAt(actor->x,actor->y);
}

void Map::computeTCODMapAt(int x, int y) {
	// first check tile
	bool transparent = tileAt(x,y)->transparent();
	bool walkable = tileAt(x,y)->walkable();
	// then the actors
	TCODList<Actor*> list = engine.getActors(x,y,false);
	for( Actor** it=list.begin(); it!=list.end(); it++ ) {
		if (walkable==false && transparent==false) break;
		Actor* actor = *it;
		if ( actor->blocks ) walkable = false;
		if ( !actor->transparent ) transparent = false;
	}
	map->setProperties(x,y, transparent,walkable);
}

void Map::computeTileMapAt(int x, int y) {
	bool transparent = tileAt(x,y)->transparent();
	bool walkable = tileAt(x,y)->walkable();
	tileMap->setProperties(x,y, transparent, walkable);
}

void Map::fillRoom(int x1, int y1, int x2, int y2, bool first=false) {
	TCODRandom* rng = TCODRandom::getInstance();
	int x,y;
	if ( first ) {
		// put the player in the first room
		engine.moveActor(engine.player, (x1+x2)/2,(y1+y2)/2 );
	} else {
		int nbMonsters = rng->getInt(0,MAX_ROOM_MONSTERS);
		while ( nbMonsters > 0 ) {
			if ( randomFreeField(x1,y1,abs(x1-x2)+1,abs(y1-y2)+1, &x,&y) )
				addMonster(x,y);
			nbMonsters--;
		}
	}
	// add items
	int nbItems=rng->getInt(0,MAX_ROOM_ITEMS);
	while (nbItems > 0) {
		if ( randomFreeField(x1,y1,abs(x1-x2)+1,abs(y1-y2)+1, &x,&y) )
			addItem(x,y);
		nbItems--;
	}
}

// ###########################
// # UNIVERSAL MAP FUNCTIONS #
// ###########################

Map::Map(int width, int height, Chunk* chunk) : width(width), height(height), chunk(chunk) {
	
	tiles = new Tile*[width*height];
	for ( int x=0; x<width; ++x )
		for ( int y=0; y<height; ++y )
			tiles[x+y*width] = new FloorTile();
	
	map = new TCODMap(width,height);
	tileMap = new TCODMap(width,height);
}

Map::~Map() {
	for ( int x=0; x<width; ++x )
		for ( int y=0; y<height; ++y )
			delete tiles[x+y*width];
	delete [] tiles;
	delete map;
	savedActors.clearAndDelete();
	chunk->map = NULL;
}

void Map::init() {
	Chunk* chunk = engine.currentChunk();
	chunk->map = this;
	TCODBsp bsp(0,0,width,height);
	BspListener listener(*this);
	{
		typedef Chunk::TerrainData TD;

		switch (chunk->broadType)
		{
		// special cases
		case Chunk::WITCH_HUT :
			fill(Tile::FieldType::TREE);
			setEllipseGrad(1,1, width-2,height-2, 0.5, Tile::FieldType::GRASS);
			engine.addActor( ActorRep::getActor( ActorRep::Name::DOWNSTAIRS, (width/2)+9,(height/2)-5 ) );
			// build witch-hut
			setRect( (width/2)-3,(height/2)-2, 6,5, Tile::FieldType::WALL );
			setRect( (width/2)-2,(height/2)-1, 4,3, Tile::FieldType::FLOOR );
			setField( (width/2),(height/2)-2, Tile::FieldType::FLOOR );
			engine.addActor( ActorRep::getActor( ActorRep::Name::DOOR, (width/2),(height/2)-2 ) );

		break;

		default:
			// check whether to start with walls or fields
			if (chunk->terrainData.startWithWalls)
				switch(chunk->broadType) {
					case Chunk::WITCH_HUT :
					case Chunk::PLAINS : fill(Tile::FieldType::TREE);
					break;
					default : fill(Tile::FieldType::WALL);
					break;
				}
			else 
				switch(chunk->broadType) {
					case Chunk::WITCH_HUT :
					case Chunk::PLAINS : fill(Tile::FieldType::GRASS);
					break;
					default : fill(Tile::FieldType::FLOOR);
					break;
				}

			// dig the rooms
			switch(chunk->terrainData.roomCreation) {
				case TD::RoomCreation::DIG_RANDOM:
				{
					bsp.splitRecursive(NULL,4,ROOM_MIN_SIZE,ROOM_MIN_SIZE,8.0f,8.0f);

					listener.state = BspListener::DIG_RND;
					listener.dig_rnd_mod = 0.4;
					bsp.traverseInvertedLevelOrder(&listener,NULL);
				}
				break;
				case TD::RoomCreation::DIG:
				{
					TCODBsp bsp(0,0,width,height);
					bsp.splitRecursive(NULL,6,ROOM_MIN_SIZE,ROOM_MIN_SIZE,8.0f,8.0f);
					BspListener listener(*this);
					listener.state = BspListener::DEFAULT;
					bsp.traverseInvertedLevelOrder(&listener,NULL);
				}
				break;
				default:
				break;
			}

			// connect them
			
			switch(chunk->terrainData.tunnelCreation) {
				case TD::TunnelCreation::RANDOM:
				{
					listener.state = BspListener::CONNECT_RND;
					TCODPath path = TCODPath(tileMap);
					bsp.traverseInvertedLevelOrder(&listener,&path);
				}
				break;
				default:
				break;
			}

			// add stairs
			{
				int x, y;
				for(int i=0; i<chunk->terrainData.numDownStairs; i++) {
					randomFreeField(0,0,width,height, &x,&y);
					engine.addActor( ActorRep::getActor( ActorRep::Name::DOWNSTAIRS, x,y ) );
				}
				for(int i=0; i<chunk->terrainData.numUpStairs; i++) {
					randomFreeField(0,0,width,height, &x,&y);
					engine.addActor( ActorRep::getActor( ActorRep::Name::UPSTAIRS, x,y ) );
				}
			}
		}
	}
}

void Map::leave() {
	if ( !chunk->persistentMap ) {
		// load off all actors back into the biomeData
		for (auto it=engine.actorsBegin(); it!=engine.actorsEnd(); it++) {
			Actor* actor = *it;
			if ( actor->actorRepName != ActorRep::NONE && actor->destructible && !actor->destructible->isDead() ) {
				chunk->biomeData.creatures[actor->actorRepName]++;
			}	
			// delete all actors but the player
			if ( actor != engine.player ) {
				it = engine.removeActor(it, true);
			}
		}
		delete this;
	} else {
		// load off all actors into the savedActors list
		for (auto it=engine.actorsBegin(); it!=engine.actorsEnd(); it++) {
			Actor* actor = *it;
			if ( actor != engine.player ) {
				savedActors.push(actor);
				it = engine.removeActor(it, false);
			}
		}
	}
}

// load all actors saved in the savedActors field (filled when the map is left) back into the engine
void Map::loadSavedActors() {
	for ( Actor** it=savedActors.begin(); it!=savedActors.end(); it++ )
		engine.addActor(*it);
	savedActors.clear();
}

bool Map::isWall(int x, int y) const {
	return !tileMap->isWalkable(x,y);
}

bool Map::inMap(int x, int y, bool includeBorders) const {
	if (includeBorders)
		return ( x>=0 && x<width && y>=0 && y<height );
	else
		return ( x>0 && x<width-1 && y>0 && y<height-1 );
}

bool Map::canWalk(int x, int y) const {
	return map->isWalkable(x,y);
}

Tile* Map::tileAt(int x, int y) const {
	return tiles[x+y*width];
}

bool Map::isExplored(int x, int y) const {
	return tiles[x+y*width]->explored;
}

bool Map::isInFov(int x, int y) const {
	if ( map->isInFov(x,y) ) {
		tiles[x+y*width]->explored = true;
		return true;
	}
	return false;
}

void Map::computeFov() {
	map->computeFov(engine.player->x, engine.player->y, engine.fovRadius);
}

bool Map::randomFreeField(int x0, int y0, int width0, int height0 ,int* x, int* y, bool wall ) {
	TCODRandom* rng = TCODRandom::getInstance();
	int tries = 0;
	do {
		*x = rng->getInt( x0, x0+width0-1 );
		*y = rng->getInt( y0, y0+height0-1 );
		tries++;
	} while ( !( (!wall && canWalk(*x,*y)) || (wall && !isWall(*x,*y) ) ) && tries < width0*height0*width0*height0 );
	if ( !( (!wall && canWalk(*x,*y)) || (wall && !isWall(*x,*y)) ) ) { // simply go through all fields and take the first free one
		for(int i=0; i<width0; i++)
			for(int j=0; j<height0; j++) {
				*x = i;
				*y = j;
				if ( (!wall && canWalk(*x,*y)) || (wall && !isWall(*x,*y)) ) break;
			}
	}
	return ( (!wall && canWalk(*x,*y)) || (wall && !isWall(*x,*y)) );
}

void Map::render() const {
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y) {
			//if ( isExplored(x,y) )
			tileAt(x,y)->render(x,y);
		}
}