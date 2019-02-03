#include "main.hpp"
#include <math.h>
#include <stdio.h>

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;
static Actor* dStairs = NULL;

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
						map.map->setProperties(x,y,true,true);
						if ( rng->getInt(1,100) > dig_rnd_centricity ) { // make sure not to stray too far from the center
							do {
								xStep = rng->getInt(-1,1);
								yStep = rng->getInt(-1,1);
							} while ( !map.inMap(x+xStep,y+yStep) );
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
	//TCODRandom* rng = TCODRandom::getInstance();
	// create a rat
	engine.actors.push( ActorRep::Rat(x,y) );
}

void Map::addItem(int x, int y) {
	/*	// on ice, until I figure out what items I want and how to distribute them
	if (item) engine.actors.insertBefore(item,0); 
	*/
}

void Map::fillRoom(int x1, int y1, int x2, int y2, bool first=false) {
	TCODRandom* rng = TCODRandom::getInstance();
	int x,y;
	if ( first ) {
		// put the player in the first room
		engine.player->x = (x1+x2)/2;
		engine.player->y = (y1+y2)/2;
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
	if ( !dStairs ) {
		// create stairs
		dStairs = new Actor(0,0,'>',"stairs",TCODColor::white);
		dStairs->blocks = false;
		dStairs->fovOnly = false;
		engine.actors.push(dStairs);
	}
	dStairs->x = (x1+x2)/2;
	dStairs->y = (y1+y2)/2;
}

// ###########################
// # UNIVERSAL MAP FUNCTIONS #
// ###########################

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width*height];
	map = new TCODMap(width,height);
}

Map::~Map() {
	delete [] tiles;
	delete map;
}

void Map::init() {
	Chunk* chunk = engine.currentChunk();
	TCODBsp bsp(0,0,width,height);
	BspListener listener(*this);
	dStairs = NULL;
	{
		typedef Chunk::TerrainData TD;

		// check whether to start with walls or fields
		if (!chunk->terrainData.startWithWalls) map->clear(true,true);

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
				TCODPath path = TCODPath(map);
				bsp.traverseInvertedLevelOrder(&listener,&path);
				// (add stairs)
			}
			break;
			default:
			break;
		}
	}
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x,y);
}

bool Map::inMap(int x, int y) const {
	return ( x!=0 && x!=width-1 && y!=0 && y!=height-1 );
}

bool Map::canWalk(int x, int y) const {
	if (isWall(x,y)) return false;
	for (Actor** iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor* actor = *iterator;
		if ( actor->blocks && actor->x == x && actor->y == y ) {
			// there is a blocking actor there. cannot walk
			return false;
		}
	}
	return true;
}

bool Map::isExplored(int x, int y) const {
	return tiles[x+y*width].explored;
}

bool Map::isInFov(int x, int y) const {
	if ( map->isInFov(x,y) ) {
		tiles[x+y*width].explored = true;
		return true;
	}
	return false;
}

void Map::computeFov() {
	map->computeFov(engine.player->x, engine.player->y, engine.fovRadius);
}

void Map::dig(int x1, int y1, int x2, int y2) {
	// make sure that x1 < x2 and y1 < y2
	if ( x2 < x1 ) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if ( y2 < y1 ) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	for (int tileX = x1; tileX <= x2; tileX++) {
		for (int tileY = y1; tileY <= y2; tileY++) {
			map->setProperties(tileX,tileY,true,true);
		}
	}
}


bool Map::randomFreeField(int x0, int y0, int width0, int height0 ,int* x, int* y, bool wall ) {
	TCODRandom* rng = TCODRandom::getInstance();
	int tries = 0;
	do {
		*x = rng->getInt( x0, x0+width0-1 );
		*y = rng->getInt( y0, y0+height0-1 );
		tries++;
	} while ( !( (!wall && canWalk(*x,*y)) || (wall && map->isWalkable(*x,*y)) ) && tries < width0*height0*width0*height0 );
	if ( !( (!wall && canWalk(*x,*y)) || (wall && map->isWalkable(*x,*y)) ) ) { // simply go through all fields and take the first free one
		for(int i=0; i<width0; i++)
			for(int j=0; j<height0; j++) {
				*x = i;
				*y = j;
				if ( (!wall && canWalk(*x,*y)) || (wall && map->isWalkable(*x,*y)) ) break;
			}
	}
	return ( (!wall && canWalk(*x,*y)) || (wall && map->isWalkable(*x,*y)) );
}

bool Map::connectRoomsRandom(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2, int randomness, int thickness) {
	TCODRandom* rng = TCODRandom::getInstance();
	int xStart; int yStart;
	if ( !randomFreeField(x1,y1, width1,height1, &xStart,&yStart, true) ) return false;
	int xEnd; int yEnd;
	if ( !randomFreeField(x2,y2, width2,height2, &xEnd,&yEnd, true) ) return false;

	int x=xStart; int y=yStart;
	while ( x!=xEnd || y!=yEnd ) {
		if ( rng->getInt(1,100) > randomness ) {
			// follow a straight path to your destination
			TCODLine::init (x,y, xEnd,yEnd);
			TCODLine::step (&x,&y);
		} else {
			// go into a random direction
			int xStep, yStep;
			do {
			xStep = rng->getInt(-1,1);
			yStep = rng->getInt(-1,1);
			} while ( !inMap(x+xStep,y+yStep) );
			x += xStep;
			y += yStep;
		}
		for (int i=0; i < thickness-1 || i==0; i++)
			for (int j=0; j < thickness; j++) {
				if ( inMap(x+i,y+j) ) map->setProperties(x+i,y+j,true,true);
			}
		
	}
	return true;
}

void Map::render() const {
	/*
	static const TCODColor darkWall(0,0,100);
	static const TCODColor darkGround(50,50,150);
	static const TCODColor lightWall(130,110,50);
	static const TCODColor lightGround(200,180,50);
	*/

	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			//if ( isExplored(x,y) ) {
			TCODConsole::root->setCharForeground(x, y, TCODColor::darkerGrey );
			TCODConsole::root->setChar(x,y, isWall(x,y) ? '#' : '.');
			//}
			if ( isInFov(x,y) ) TCODConsole::root->setCharForeground(x, y, TCODColor::white );
		}
	}
}