/* a=target variable, b=bit number to act upon 0-n */
#ifndef BIT_MACROS
#define BIT_MACROS
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1
#endif

#include "main.hpp"
#include <map>

// it's not yet sure whether using BitArrays will have any advantage because of the final compression
static BitArray bitArray;

BitArray::BitArray(TCODZip* zipPtr) : array(0),bitNum(0) {
	if (zipPtr) array = zipPtr->getChar();	// to start in reading-mode
}

void BitArray::save(TCODZip& zip, bool value) {
	// check for free space, if false save the bitArray to zip and start a new one
	if (bitNum >= 8) {
		finish(zip);
	}
	if (value) BIT_SET(array,bitNum);
	bitNum++;
}

bool BitArray::load(TCODZip& zip) {
	// check for invalid index, if true load the next char from zip
	if ( bitNum >= 8 ) {
		array = zip.getChar();
		bitNum = 0;
	}
	return BIT_CHECK(array,bitNum++);
}

void BitArray::finish(TCODZip& zip) {
	// save the current bitArray and reinitialize
	zip.putChar(array);
	array = 0;
	bitNum = 0;
}

void Engine::save() {
	if ( player->destructible->isDead() ) {
		TCODSystem::deleteFile("game.sav");
	} else {
		TCODZip zip;
		// save the player first
		player->save(zip);
		// then some engine data
		zip.putInt(x);
		zip.putInt(y);
		zip.putInt(depth);
		// then the map
		zip.putInt(map->width);
		zip.putInt(map->height);
		map->save(zip);
		// then all the other actors
		zip.putInt(actors.size()-1);
		for (Actor** it=actors.begin(); it!=actors.end(); it++) {
			if ( *it != player ) {
				(*it)->save(zip);
			}
		}
		// save the message log
		gui->save(zip);
		// and finally SAVE THE WORLD!
		for(int i=0; i<worldSize; i++)
			for(int j=0; j<worldSize; j++)
				for(int k=0; k<worldDepth; k++)
					world[i][j][k]->save(zip);
		zip.saveToFile("game.sav");
	}
}

void Engine::load() {
	TCODZip zip;
	// continue a saved game
	terminate();
	zip.loadFromFile("game.sav");
	// load the player
	player = new Actor(0,0,0,"",TCODColor::white);
	player->load(zip);
	actors.push(player);
	// then some engine data
	x = zip.getInt();
	y = zip.getInt();
	depth = zip.getInt();
	// then the map
	int width = zip.getInt();
	int height = zip.getInt();
	map = new Map(width,height, currentChunk() );
	map->load(zip);
	// then all the other actors
	int nbActors = zip.getInt();
	while ( nbActors > 0 ) {
		Actor* actor = new Actor(0,0,0,"",TCODColor::white);
		actor->load(zip);
		actors.push(actor);
		nbActors--;
	}
	// the message log
	gui->load(zip);
	// and finally THE WORLD!
	for(int i=0; i<worldSize; i++)
		for(int j=0; j<worldSize; j++)
			for(int k=0; k<worldDepth; k++)
				world[i][j][k]->load(zip);
	// to force FOV recomputation
	gameStatus = STARTUP;
}

void Gui::save(TCODZip& zip) {
	int size = log.size();
	zip.putInt(size);
	Message* message;
	for (int i = size-1; i >= 0; i--) {
		message = log.get(i);
		zip.putString(message->text);
		zip.putColor(&message->col);
	}
}

void Gui::load(TCODZip& zip) {
	int nbMessages = zip.getInt();
	while (nbMessages > 0) {
		const char* text = zip.getString();
		TCODColor col = zip.getColor();
		message(col,text);
		nbMessages--;
	}
}

void Chunk::save(TCODZip& zip) {
	zip.putInt( persistentMap );
	zip.putInt( (int)broadType );
	// terrainData
	zip.putInt( terrainData.startWithWalls );
	zip.putInt( (int)terrainData.roomCreation );
	zip.putInt( (int)terrainData.tunnelCreation );
	// biomeData
	zip.putInt( biomeData.creatures.size() );
	for(auto it = biomeData.creatures.cbegin(); it != biomeData.creatures.cend(); ++it) {
		zip.putInt( (int)it->first ); // name
		zip.putInt( it->second );	  // number of creatures of this type
	}
}

void Chunk::load(TCODZip& zip) {
	persistentMap = zip.getInt();
	broadType = (BroadType)zip.getInt();
	// terrainData
	terrainData.startWithWalls = (bool)zip.getInt();
	terrainData.roomCreation = (TerrainData::RoomCreation)zip.getInt();
	terrainData.tunnelCreation = (TerrainData::TunnelCreation)zip.getInt();
	// biomeData
	// creature map
	biomeData.creatures.clear();
	int size = zip.getInt();
	for(int i=0; i<size; ++i) {
		biomeData.creatures.insert( std::pair<ActorRep::Name,int>( (ActorRep::Name)zip.getInt(),zip.getInt() ) );
	}
}

void Map::save(TCODZip& zip) {
	bitArray = BitArray();	
	// iterate over the whole map and save every tile
	for (int i = 0; i < width*height; i++) {
		tiles[i].save(zip);
	}
	// save the map (libtcod-map-container)
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			bitArray.save(zip, map->isTransparent(x,y));
			bitArray.save(zip, isWall(x,y));
		}
	// save the last bit array
	bitArray.finish(zip);
}

void Map::load(TCODZip& zip) {
	// load the first bit array
	bitArray = BitArray(&zip);
	// iterate over the whole map and load every tile
	for (int i = 0; i < width*height; i++) {
		tiles[i].load(zip);
	}
	// load the map (libtcod-map-container)
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			map->setProperties(x,y,!bitArray.load(zip),bitArray.load(zip));
		}
}

void Tile::save(TCODZip& zip) {
	bitArray.save(zip, explored);
	//zip.putInt(explored);
}

void Tile::load(TCODZip& zip) {
	explored = bitArray.load(zip);
	//explored = zip.getInt();
}

void Actor::save(TCODZip& zip) {
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(ch);
	zip.putColor(&col);
	zip.putString(name);
	zip.putInt(blocks);
	zip.putFloat(volume);
	zip.putFloat(weight);
	zip.putFloat(nutrition);
	// save component flags
	zip.putInt(attacker != NULL);
	zip.putInt(destructible != NULL);
	zip.putInt(ai != NULL);
	zip.putInt(pickable != NULL);
	zip.putInt(container != NULL);
	zip.putInt(stomach != NULL);
	// save components themselves
	if ( attacker ) attacker->save(zip);
	if ( destructible ) destructible->save(zip);
	if ( ai ) ai->save(zip);
	if ( pickable ) pickable->save(zip);
	if ( container ) container->save(zip);
	if ( stomach ) stomach->save(zip);
}

void Actor::load(TCODZip& zip) {
	x=zip.getInt();
	y=zip.getInt();
	ch=zip.getInt();
	col=zip.getColor();
	strcpy( this->name, zip.getString() );
	blocks=zip.getInt();
	volume=zip.getFloat();
	weight=zip.getFloat();
	nutrition=zip.getFloat();
	// load component flags
	bool hasAttacker=zip.getInt();
	bool hasDestructible=zip.getInt();
	bool hasAi=zip.getInt();
	bool hasPickable=zip.getInt();
	bool hasContainer=zip.getInt();
	bool hasStomach=zip.getInt();
	// load components
	if ( hasAttacker ) {
		attacker = new Attacker(0.0f);
		attacker->load(zip);
	}
	if ( hasDestructible ) {
		destructible = Destructible::create(zip);
	}
	if ( hasAi ) {
		ai = Ai::create(zip);
	}
	if ( hasPickable ) {
		pickable = new Pickable();
		pickable->load(zip);
	}
	if ( hasContainer ) {
		container = new Container(0);
		container->load(zip);
	}
	if ( hasStomach ) {
		stomach = new Stomach(0,0,0,0);
		stomach->load(zip);
	}
}

void Attacker::save(TCODZip& zip) {
	zip.putFloat(power);
}

void Attacker::load(TCODZip& zip) {
	power = zip.getFloat();
}

void Container::save(TCODZip& zip) {
	zip.putInt(size);
	zip.putFloat(maxVolume);
	zip.putInt(inventory.size());
	for (Actor** it=inventory.begin(); it!=inventory.end(); it++) {
		(*it)->save(zip);
	}
}

void Container::load(TCODZip& zip) {
	size = zip.getInt();
	maxVolume = zip.getFloat();
	int nbActors = zip.getInt();
	while ( nbActors > 0 ) {
		Actor* actor = new Actor(0,0,0,"",TCODColor::white);
		actor->load(zip);
		inventory.push(actor);
		nbActors--;
	}
}

void Stomach::save(TCODZip& zip) {
	Container::save(zip);
	zip.putFloat( nutrition );
	zip.putFloat( maxNutrition );
	zip.putFloat( need );
	zip.putFloat( digestionRate );
}

void Stomach::load(TCODZip& zip) {
	Container::load(zip);
	nutrition = zip.getFloat();
	maxNutrition = zip.getFloat();
	need = zip.getFloat();
	digestionRate = zip.getFloat();
}

void Destructible::save(TCODZip& zip) {
	zip.putFloat(maxHp);
	zip.putFloat(hp);
	zip.putFloat(defense);
	zip.putString(corpseName);
}

void Destructible::load(TCODZip& zip) {
	maxHp = zip.getFloat();
	hp = zip.getFloat();
	defense = zip.getFloat();
	strcpy(this->corpseName, zip.getString());
}

void PlayerDestructible::save(TCODZip& zip) {
	zip.putInt(PLAYER);
	Destructible::save(zip);
}

void MonsterDestructible::save(TCODZip& zip) {
	zip.putInt(MONSTER);
	Destructible::save(zip);
}

void PlayerDestructible::load(TCODZip& zip) {
	Destructible::load(zip);
}

void MonsterDestructible::load(TCODZip& zip) {
	Destructible::load(zip);
}

Destructible* Destructible::create(TCODZip& zip) {
	DestructibleType type = (DestructibleType)zip.getInt();
	Destructible* destructible = NULL;
	switch(type) {
		case MONSTER : destructible = new MonsterDestructible(0,0,""); break;
		case PLAYER : destructible = new PlayerDestructible(0,0,""); break;
	}
	destructible->load(zip);
	return destructible;
}

void Pickable::save(TCODZip& zip) {
	zip.putInt( !!useable );
	if ( useable ) useable->save(zip);
}

void Pickable::load(TCODZip& zip) {
	if ( zip.getInt() ) {
		useable = new Useable(NULL,NULL);
		useable->load(zip);
	}
}

void Useable::save(TCODZip& zip) {
	zip.putInt( !!selector ); // has selector
	if (selector) selector->save(zip);
	effect->save(zip);
}

void Useable::load(TCODZip& zip) {
	if ( zip.getInt() ) {
		selector =	new TargetSelector(TargetSelector::CLOSEST_MONSTER,0);
		selector->load(zip);
	}
	effect =	Effect::create(zip);
}

void TargetSelector::save(TCODZip& zip) {
	zip.putInt((int)type);
	zip.putFloat(range);
	zip.putFloat(areaRange);
}

void TargetSelector::load(TCODZip& zip) {
	type = (SelectorType)zip.getInt();
	range = zip.getFloat();
	areaRange = zip.getFloat();
}

Effect* Effect::create(TCODZip& zip) {
	EffectType type = (EffectType)zip.getInt();
	Effect* effect = NULL;
	switch(type) {
		case HEALTH : effect = new HealthEffect(0,NULL); break;
		case CONFUSE : effect = new ConfusionEffect(0,NULL); break;
	}
	effect->load(zip);
	return effect;
}

// Effects

void HealthEffect::save(TCODZip& zip) {
	zip.putInt(HEALTH);
	zip.putFloat(amount);
	zip.putInt( !!message ); // has selector
	if (message) {
		const char* msg = message;
		zip.putString(msg);
	}
}

void HealthEffect::load(TCODZip& zip) {
	amount = zip.getFloat();
	if ( zip.getInt() ) message = zip.getString();
}

void ConfusionEffect::save(TCODZip& zip) {
	zip.putInt(CONFUSE);
	zip.putInt(nbTurns);
	zip.putInt( !!message ); // has selector
	if (message) {
		const char* msg = message;
		zip.putString(msg);
	}
}

void ConfusionEffect::load(TCODZip& zip) {
	nbTurns = zip.getInt();
	if ( zip.getInt() ) message = zip.getString();
}

// AIs

void MonsterAi::load(TCODZip &zip) {
	moveCount=zip.getInt();
}

void MonsterAi::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
}

void ConfusedMonsterAi::load(TCODZip &zip) {
	nbTurns=zip.getInt();
	oldAi=Ai::create(zip);
}

void ConfusedMonsterAi::save(TCODZip &zip) {
	zip.putInt(CONFUSED);
	zip.putInt(nbTurns);
	oldAi->save(zip);
}

void PlayerAi::load(TCODZip &zip) {
}

void PlayerAi::save(TCODZip &zip) {
	zip.putInt(PLAYER);
}

Ai* Ai::create(TCODZip& zip) {
	AiType type = (AiType)zip.getInt();
	Ai* ai = NULL;
	switch(type) {
		case PLAYER : ai = new PlayerAi(); break;
		case MONSTER : ai = new MonsterAi(); break;
		case CONFUSED : ai = new ConfusedMonsterAi(0,NULL); break;
	}
	ai->load(zip);
	return ai;
}