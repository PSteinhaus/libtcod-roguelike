#include "main.hpp"

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
					for(float i=0; i<(node->w*node->h*dig_rnd_mod); i++) {
						map.map->setProperties(x,y,true,true);
						if ( rng->getInt(1,100) > dig_rnd_centricity ) { // make sure not to stray too far from the center
							do {
								xStep = rng->getInt(-1,1);
								yStep = rng->getInt(-1,1);
							} while ( x+xStep==0 || x+xStep==map.width-1 || y+yStep==0 || y+yStep==map.height-1 );
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
					int thickness = 1;
					if ( !node->isLeaf() ) { // node and sibling are parents; replace each with one of their (already interconnected) children
						node = node->findNode(node->x+node->w-1,node->y+node->h-1);
						sibling = sibling->findNode(sibling->x+sibling->w-1,sibling->y+sibling->h-1);
					}
					while ( !path->compute(node->x+(node->w/2),node->y+(node->h/2),
										sibling->x+(sibling->w/2),sibling->y+(sibling->h/2)) ) {
						// no connection found, so establish one
						map.connectRoomsRandom(node->x,node->y,node->w,node->h,
											sibling->x,sibling->y,sibling->w,sibling->h, 40, thickness);
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
	if ( first ) {
		// put the player in the first room
		engine.player->x = (x1+x2)/2;
		engine.player->y = (y1+y2)/2;
	} else {
		int nbMonsters = rng->getInt(0,MAX_ROOM_MONSTERS);
		while ( nbMonsters > 0 ) { 
			int x = rng->getInt(x1,x2);
			int y = rng->getInt(y1,y2);
			if ( canWalk(x,y) ) {
				addMonster(x,y);
				nbMonsters--;
			}
		}
	}
	// add items
	int nbItems=rng->getInt(0,MAX_ROOM_ITEMS);
	while (nbItems > 0) {
		int x=rng->getInt(x1,x2);
		int y=rng->getInt(y1,y2);
		if ( canWalk(x,y) ) {
			addItem(x,y);
			nbItems--;
		}
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

Map::Map(int width, int height) : width(width), height(height), biome(CAVE) {
	tiles = new Tile[width*height];
	map = new TCODMap(width,height);
}

Map::~Map() {
	delete [] tiles;
	delete map;
}

void Map::init() {
	dStairs = NULL;
	switch(biome) {
		case CAVE:
		{
			TCODBsp bsp(0,0,width,height);
			bsp.splitRecursive(NULL,5,ROOM_MIN_SIZE,ROOM_MIN_SIZE,8.0f,8.0f);
			BspListener listener(*this);
			listener.state = BspListener::DIG_RND;
			listener.dig_rnd_mod = 0.8;
			bsp.traverseInvertedLevelOrder(&listener,NULL);
			listener.state = BspListener::CONNECT_RND;
			TCODPath path = TCODPath(map);
			bsp.traverseInvertedLevelOrder(&listener,&path);
			// add stairs
		}
		break;
		default:
		{
			TCODBsp bsp(0,0,width,height);
			bsp.splitRecursive(NULL,6,ROOM_MIN_SIZE,ROOM_MIN_SIZE,8.0f,8.0f);
			BspListener listener(*this);
			listener.state = BspListener::DEFAULT;
			bsp.traverseInvertedLevelOrder(&listener,NULL);
		}
		break;
	}
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x,y);
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

void Map::randomFreeField(int x0, int y0, int width0, int height0 ,int* x, int* y) {
	TCODRandom* rng = TCODRandom::getInstance();
	do {
		*x = rng->getInt( x0, x0+width0-1 );
		*y = rng->getInt( y0, y0+height0-1 );
	} while ( !canWalk(*x,*y) );
}

void Map::connectRoomsRandom(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2, int randomness, int thickness) {
	TCODRandom* rng = TCODRandom::getInstance();
	int xStart; int yStart;
	randomFreeField(x1,y1, width1,height1, &xStart,&yStart);
	int xEnd; int yEnd;
	randomFreeField(x2,y2, width2,height2, &xEnd,&yEnd);

	int x=xStart; int y=yStart;
	while ( x!=xEnd && y!=yEnd ) {
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
			} while ( x+xStep==0 || x+xStep==width-1 || y+yStep==0 || y+yStep==height-1 );
			x += xStep;
			y += yStep;
		}
		//map->setProperties(x,y,true,true);
		
		// adjust for tunnel thickness
		//x -= thickness-1/2;
		//y -= thickness-1/2;
		for (int i=0; i < thickness; i++)
			for (int j=0; j < thickness; j++) {
				map->setProperties(x+i,y+j,true,true);
			}
		
	}
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
			if ( isExplored(x,y) ) {
			TCODConsole::root->setCharForeground(x, y, TCODColor::darkerGrey );
			TCODConsole::root->setChar(x,y, isWall(x,y) ? '#' : '.');
			}
			if ( isInFov(x,y) ) TCODConsole::root->setCharForeground(x, y, TCODColor::white );
		}
	}
}