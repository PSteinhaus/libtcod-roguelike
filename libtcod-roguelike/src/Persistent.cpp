/* a=target variable, b=bit number to act upon 0-n */
#ifndef BIT_MACROS
#define BIT_MACROS
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1
#endif

#include "main.hpp"

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
	if ( player!=NULL && player->destructible->isDead() ) {
		TCODSystem::deleteFile("game.sav");
	} else if ( player!=NULL && !player->destructible->isDead() ) {
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
		zip.putInt(totalActors()-1);
		for (auto it=actorsBegin(); it!=actorsEnd(); it++) {
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
	showLoadingScreen();
	TCODZip zip;
	// continue a saved game
	terminate();
	zip.loadFromFile("game.sav");
	// load the player
	player = new Actor(0,0,0,"",TCODColor::white);
	player->load(zip);
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
		addActor(actor);
		nbActors--;
	}
	addActor(player);
	// the message log
	gui->load(zip);
	// and finally THE WORLD!
	
	for(int i=0; i<worldSize; i++)
		for(int j=0; j<worldSize; j++)
			for(int k=0; k<worldDepth; k++) {
				world[i][j][k] = new Chunk(Chunk::PLAINS);
				world[i][j][k]->load(zip);
			}

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
	zip.putInt( map != NULL );
	if ( map && map!=engine.map ) {
		zip.putInt( map->width );
		zip.putInt( map->height );
		map->save(zip);
	}
	zip.putInt( (int)broadType );
	// terrainData
	zip.putInt( terrainData.startWithWalls );
	zip.putInt( (int)terrainData.roomCreation );
	zip.putInt( (int)terrainData.tunnelCreation );
	zip.putInt( terrainData.numDownStairs );
	zip.putInt( terrainData.numUpStairs );
	// biomeData
	zip.putInt( biomeData.creatures.size() );
	for(auto it = biomeData.creatures.cbegin(); it != biomeData.creatures.cend(); ++it) {
		zip.putInt( it->second );	  // number of creatures of this type
		zip.putInt( (int)it->first ); // name
	}
}

void Chunk::load(TCODZip& zip) {
	persistentMap = zip.getInt();
	if ( zip.getInt() ) {
		if ( this == engine.currentChunk() ) {
			map = engine.map;
			map->chunk = this;
		} else {
			map = new Map( zip.getInt(), zip.getInt(), this);
			map->load(zip);
		}
	}
	broadType = (BroadType)zip.getInt();
	// terrainData
	terrainData.startWithWalls = (bool)zip.getInt();
	terrainData.roomCreation = (TerrainData::RoomCreation)zip.getInt();
	terrainData.tunnelCreation = (TerrainData::TunnelCreation)zip.getInt();
	terrainData.numDownStairs = zip.getInt();
	terrainData.numUpStairs = zip.getInt();
	// biomeData
	// creature map
	biomeData.creatures.clear();
	int size = zip.getInt();
	for(int i=0; i<size; ++i) {
		biomeData.creatures.insert( std::pair<ActorRep::Name,int>( (ActorRep::Name)zip.getInt(),zip.getInt() ) );
	}
}

void Map::save(TCODZip& zip) {
	// iterate over the whole map and save every tile
	for (int i = 0; i < width*height; i++) {
		tiles[i]->save(zip);
	}
	// save the savedActors
	zip.putInt(savedActors.size());
	for (Actor** it=savedActors.begin(); it!=savedActors.end(); it++) {
		(*it)->save(zip);
	}
}

void Map::load(TCODZip& zip) {
	// iterate over the whole map and load every tile 
	for (int i = 0; i < width*height; i++) {
		delete tiles[i];
		tiles[i] = Tile::create(zip);
	}
	// load the savedActors
	int nbActors = zip.getInt();
	while ( nbActors > 0 ) {
		Actor* actor = new Actor(0,0,0,"",TCODColor::white);
		actor->load(zip);
		savedActors.push(actor);
		nbActors--;
	}
	// recalculate tileMap
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			computeTCODMapAt(x,y);
			computeTileMapAt(x,y);
		}
}

Tile* Tile::create(TCODZip& zip) {
	FieldType type = (FieldType)(zip.getInt());
	Tile* tile = NULL;
	switch(type) {
		case FLOOR : tile = new FloorTile(0,0); break;
		case WALL : tile = new WallTile(0,0); break;
		case GRASS: tile = new GrassTile(0,0); break;
		case TREE: tile = new TreeTile(0,0); break;
	}
	tile->load(zip);
	return tile;
}

void Tile::save(TCODZip& zip) {
	zip.putInt( (int)fieldType() );
	zip.putInt(explored);
	zip.putInt(x);
	zip.putInt(y);
}

void Tile::load(TCODZip& zip) {
	explored = zip.getInt();
	x = zip.getInt();
	y = zip.getInt();
}

void Actor::save(TCODZip& zip) {
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(ch);
	zip.putColor(&col);
	zip.putString(name);
	zip.putInt(blocks);
	zip.putInt(fovOnly);
	zip.putFloat(volume);
	zip.putFloat(weight);
	zip.putInt( (int)actorRepName );
	// save component flags
	zip.putInt(attacker != NULL);
	zip.putInt(destructible != NULL);
	zip.putInt(ai != NULL);
	zip.putInt(pickable != NULL);
	zip.putInt(container != NULL);
	zip.putInt(stomach != NULL);
	zip.putInt(interactable != NULL);
	// save components themselves
	if ( attacker ) attacker->save(zip);
	if ( destructible ) destructible->save(zip);
	if ( ai ) ai->save(zip);
	if ( pickable ) pickable->save(zip);
	if ( container ) container->save(zip);
	if ( stomach ) stomach->save(zip);
	if ( interactable ) interactable->save(zip);
}

void Actor::load(TCODZip& zip) {
	x=zip.getInt();
	y=zip.getInt();
	ch=zip.getInt();
	col=zip.getColor();
	strcpy( this->name, zip.getString() );
	blocks=zip.getInt();
	fovOnly=zip.getInt();
	volume=zip.getFloat();
	weight=zip.getFloat();
	actorRepName=(ActorRep::Name)zip.getInt();
	// load component flags
	bool hasAttacker=zip.getInt();
	bool hasDestructible=zip.getInt();
	bool hasAi=zip.getInt();
	bool hasPickable=zip.getInt();
	bool hasContainer=zip.getInt();
	bool hasStomach=zip.getInt();
	bool hasInteractable=zip.getInt();
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
	if ( hasInteractable ) {
		interactable = new Useable(NULL,NULL);
		interactable->load(zip);
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
	zip.putInt( !!digestor );
	if ( digestor ) digestor->save(zip);
}

void Pickable::load(TCODZip& zip) {
	if ( zip.getInt() ) {
		useable = new Useable(NULL,NULL);
		useable->load(zip);
	}
	if ( zip.getInt() ) {
		digestor = new Digestor(0);
		digestor->load(zip);
	}
}

void Digestor::save(TCODZip& zip) {
	zip.putFloat(nutrition);
}

void Digestor::load(TCODZip& zip) {
	nutrition = zip.getFloat();
}

void Useable::save(TCODZip& zip) {
	zip.putInt( !!selector ); // has selector
	if (selector) selector->save(zip);
	effect->save(zip);
	zip.putInt( destroyWhenEmpty );
}

void Useable::load(TCODZip& zip) {
	if ( zip.getInt() ) {
		selector =	TargetSelector::create(zip);
	}
	effect =	Effect::create(zip);
	destroyWhenEmpty = zip.getInt();
}

void TargetSelector::save(TCODZip& zip) {
	zip.putInt((int)selectorType);
}

TargetSelector* TargetSelector::create(TCODZip& zip) {
	SelectorType type = (SelectorType)zip.getInt();
	TargetSelector* selector = NULL;
	switch(type) {
		case CLOSEST_MONSTER : selector = new SelectClosestMonster(); break;
		case SELECTED_MONSTER : selector = new SelectField(); break;
		case USER_RANGE : selector = new RangeAroundUser(); break;
		case SELECTED_RANGE : selector = new SelectFieldRange(); break;
		case EFFECT_CARRIER : selector = new SelectEffectCarrier(); break;
		case NUMPAD : selector = new SelectNumpadAdjecent(); break;
	}
	selector->load(zip);
	return selector;
}

void SelectClosestMonster::save(TCODZip& zip) {
	TargetSelector::save(zip);
	zip.putFloat(range);
}

void SelectClosestMonster::load(TCODZip& zip) {
	range = zip.getFloat();
}

void SelectField::save(TCODZip& zip) {
	TargetSelector::save(zip);
	zip.putFloat(range);
}

void SelectField::load(TCODZip& zip) {
	range = zip.getFloat();
}

void RangeAroundUser::save(TCODZip& zip) {
	TargetSelector::save(zip);
	zip.putFloat(range);
	zip.putInt(includeUser);
}

void RangeAroundUser::load(TCODZip& zip) {
	range = zip.getFloat();
	includeUser = zip.getInt();
}

void SelectFieldRange::save(TCODZip& zip) {
	TargetSelector::save(zip);
	zip.putFloat(range);
	zip.putFloat(areaRange);
}

void SelectFieldRange::load(TCODZip& zip) {
	range = zip.getFloat();
	areaRange = zip.getFloat();
}

void SelectEffectCarrier::save(TCODZip& zip) {
	TargetSelector::save(zip);
}

void SelectEffectCarrier::load(TCODZip& zip) {
}

void SelectNumpadAdjecent::save(TCODZip& zip) {
	TargetSelector::save(zip);
	zip.putInt(acceptCenter);
}

void SelectNumpadAdjecent::load(TCODZip& zip) {
	acceptCenter = zip.getInt();
}

// Effects

Effect* Effect::create(TCODZip& zip) {
	EffectType type = (EffectType)zip.getInt();
	Effect* effect = NULL;
	switch(type) {
		case HEALTH : effect = new HealthEffect(0,NULL); break;
		case CONFUSE : effect = new ConfusionEffect(0,NULL); break;
		case DOOR : effect = new DoorEffect('*'); break;
	}
	effect->load(zip);
	return effect;
}

void Effect::save(TCODZip& zip) {
	zip.putInt(empty);
}

void Effect::load(TCODZip& zip) {
	empty = zip.getInt();
}

void HealthEffect::save(TCODZip& zip) {
	zip.putInt(HEALTH);
	Effect::save(zip);
	zip.putFloat(amount);
	zip.putInt( !!message ); // has selector
	if (message) {
		const char* msg = message;
		zip.putString(msg);
	}
}

void HealthEffect::load(TCODZip& zip) {
	Effect::load(zip);
	amount = zip.getFloat();
	if ( zip.getInt() ) message = zip.getString();
}

void ConfusionEffect::save(TCODZip& zip) {
	zip.putInt(CONFUSE);
	Effect::save(zip);
	zip.putInt(nbTurns);
	zip.putInt( !!message ); // has selector
	if (message) {
		const char* msg = message;
		zip.putString(msg);
	}
}

void ConfusionEffect::load(TCODZip& zip) {
	Effect::load(zip);
	nbTurns = zip.getInt();
	if ( zip.getInt() ) message = zip.getString();
}

void DoorEffect::save(TCODZip& zip) {
	zip.putInt(DOOR);
	Effect::save(zip);
	zip.putInt(originalChar);
}

void DoorEffect::load(TCODZip& zip) {
	Effect::load(zip);
	originalChar = zip.getInt();
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

void Equipable::save(TCODZip &zip) {
	zip.putInt( (int)slotNeeded );
	zip.putInt( !!equipEffect );
	if ( equipEffect ) equipEffect->save(zip);
	zip.putInt( !!unequipEffect );
	if ( unequipEffect ) unequipEffect->save(zip);
	zip.putInt( implicitlyUseable );
}

void Equipable::load(TCODZip &zip) {
	slotNeeded = (BodyPart::EquipmentSlot)zip.getInt();
	if ( zip.getInt() ) equipEffect = Effect::create(zip);
	if ( zip.getInt() ) unequipEffect = Effect::create(zip);
	implicitlyUseable = zip.getInt();
}

void Body::save(TCODZip &zip) {
	zip.putInt(parts.size());
	for ( auto it=parts.begin(); it!=parts.end(); ++it ) {
		(*it)->save(zip);
	}
}

void Body::load(TCODZip &zip) {
	parts.clearAndDelete();
	for ( int numParts=zip.getInt(); numParts>0; --numParts ) {
		BodyPart* part = new BodyPart();
		part->load(zip);
		parts.push(part);
	}
}

void BodyPart::save(TCODZip &zip) {
	zip.putInt(slots.size());
	for ( auto it=slots.begin(); it!=slots.end(); ++it ) {
		auto slot = *it;
		zip.putInt( (int)slot.first );
		zip.putInt( !!slot.second );
		if ( slot.second ) slot.second->save(zip);
	}
}

void BodyPart::load(TCODZip &zip) {
	for ( int numSlots=zip.getInt(); numSlots>0; --numSlots ) {
		auto slot = std::pair<EquipmentSlot,Actor*> {};
		slot.first = (EquipmentSlot)zip.getInt();
		if ( zip.getInt() ) {
			slot.second = new Actor();
			slot.second->load(zip);
		}
		slots.push_back(slot);
	}
}