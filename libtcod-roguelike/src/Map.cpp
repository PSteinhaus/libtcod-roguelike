#include "main.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback {
private:
	Map& map; // a map to dig
	int roomNum; // room number
	int lastX, lastY; // center of the last room

public:
	BspListener(Map& map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp* node, void* userData) {
		if ( node->isLeaf() ) {
			int x,y,w,h;
			// dig a room
			TCODRandom *rng=TCODRandom::getInstance();
			w = rng->getInt(ROOM_MIN_SIZE, node->w-2);
			h = rng->getInt(ROOM_MIN_SIZE, node->h-2);
			x = rng->getInt(node->x+1, node->x+node->w-w-1);
			y = rng->getInt(node->y+1, node->y+node->h-h-1);
			map.createRoom(x, y, x+w-1, y+h-1, roomNum == 0);
		
			if ( roomNum != 0 ) {
				// dig a corridor from last room
				map.dig(lastX, lastY, x+w/2, lastY);
				map.dig(x+w/2, lastY, x+w/2, y+h/2);
			}
			lastX = x+w/2;
			lastY = y+h/2;
			roomNum++;
		}
		return true;
	}
};

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width*height];
	map = new TCODMap(width,height);
}

Map::~Map() {
	delete [] tiles;
	delete map;
}

void Map::init() {
	TCODBsp bsp(0,0,width,height);
	bsp.splitRecursive(NULL,6,ROOM_MIN_SIZE+2,ROOM_MIN_SIZE+2,8.0f,8.0f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener,NULL);
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

void Map::addMonster(int x, int y) {
	TCODRandom* rng = TCODRandom::getInstance();
	if ( rng->getInt(0,100) < 80 ) {
		// create an orc
		Actor *orc = new Actor(x,y,'o',"orc", TCODColor::desaturatedGreen);
		orc->destructible = new MonsterDestructible(10,0,"dead orc");
		orc->attacker = new Attacker(3);
		orc->ai = new MonsterAi();
		engine.actors.push(orc);
	} else {
		// create a troll
		Actor *troll = new Actor(x,y,'T',"troll", TCODColor::darkerGreen);
		troll->destructible = new MonsterDestructible(16,1,"troll carcass");
		troll->attacker = new Attacker(4);
		troll->ai = new MonsterAi();
		engine.actors.push(troll); 
	}
}

void Map::addItem(int x, int y) {
	TCODRandom* rng = TCODRandom::getInstance();
	int dice = rng->getInt(1,100);
	Actor* item = NULL;
	if ( dice <= 70 ) {
		//create a health potion
		item = new Actor(x,y,'!',"health potion", TCODColor::violet);
		item->blocks = false;
		item->pickable = new Pickable();
		item->pickable->useable = new Healer(4);
	} else if ( dice <= 70+10 ) { 
		// create a scroll of lightning bolt
		item = new Actor(x,y,'?',"scroll of lightning bolt", TCODColor::lightYellow);
		item->blocks = false;
		item->pickable = new Pickable();
		item->pickable->useable = new LightningBolt(5,20);
	} else if ( dice <= 80+10 ) {
		// create a scroll of fireball
		item = new Actor(x,y,'?',"scroll of fireball",
			TCODColor::lightYellow);
		item->blocks = false;
		item->pickable = new Pickable();
		item->pickable->useable = new Fireball(7,10,3);
	} else if ( dice <= 90+10 ) {
		// create a scroll of confusion
		item = new Actor(x,y,'?',"scroll of confusion",
			TCODColor::lightYellow);
		item->blocks = false;
		item->pickable = new Pickable();
		item->pickable->useable = new Confuser(8,8);
	}
	if (item) engine.actors.insertBefore(item,0);
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

void Map::createRoom(int x1, int y1, int x2, int y2, bool first=false) {
	dig (x1, y1, x2, y2);
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
			if ( canWalk(x,y) ) addMonster(x,y);
			nbMonsters--;
		}
	}
	// add items
	int nbItems=rng->getInt(0,MAX_ROOM_ITEMS+20);
	while (nbItems > 0) {
		int x=rng->getInt(x1,x2);
		int y=rng->getInt(y1,y2);
		if ( canWalk(x,y) ) {
			addItem(x,y);
		}
		nbItems--;
	}
	// set stairs position
	engine.stairs->x=(x1+x2)/2;
	engine.stairs->y=(y1+y2)/2;
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