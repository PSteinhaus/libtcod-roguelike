#include "main.hpp"

void Engine::save() {
	if ( player->destructible->isDead() ) {
		TCODSystem::deleteFile("game.sav");
	} else {
		TCODZip zip;
		// save the player first
		player->save(zip);
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
		// finally the message log
		gui->save(zip);
		zip.saveToFile("game.sav");
	}
}

void Engine::load() {
	if ( TCODSystem::fileExists("game.sav") ) {
		TCODZip zip;
		zip.loadFromFile("game.sav");
		// load the player
		player = new Actor(0,0,0,"",TCODColor::white);
		player->load(zip);
		actors.push(player);
		// then the map
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width,height);
		map->load(zip);
		// then all the other actors
		int nbActors = zip.getInt();
		while ( nbActors > 0 ) {
			Actor* actor = new Actor(0,0,0,"",TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		// finally the message log
		gui->load(zip);
	} else {
		init();
	}
}

void Gui::save(TCODZip& zip) {
	zip.putInt(log.size());
	for (Message** it=log.begin(); it!=log.end(); it++) {
		zip.putString((*it)->text);
		zip.putColor(&(*it)->col);
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

void Map::save(TCODZip& zip) {
	// iterate over the whole map and save every tile
	for (int i = 0; i < width*height; i++) {
		tiles[i].save(zip);
	}
	// save the map (libtcod-map-container)
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			zip.putInt(map->isTransparent(x,y));
			zip.putInt(isWall(x,y));
		}
}

void Map::load(TCODZip& zip) {
	// iterate over the whole map and load every tile
	for (int i = 0; i < width*height; i++) {
		tiles[i].load(zip);
	}
	// load the map (libtcod-map-container)
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			map->setProperties(x,y,!zip.getInt(),zip.getInt());
		}
}

void Tile::save(TCODZip& zip) {
	zip.putInt(explored);
}

void Tile::load(TCODZip& zip) {
	explored = zip.getInt();
}

void Actor::save(TCODZip& zip) {
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(ch);
	zip.putColor(&col);
	zip.putString(name);
	zip.putInt(blocks);
	// save component flags
	zip.putInt(attacker != NULL);
	zip.putInt(destructible != NULL);
	zip.putInt(ai != NULL);
	zip.putInt(pickable != NULL);
	zip.putInt(container != NULL);
	// save components themselves
	if ( attacker ) attacker->save(zip);
	if ( destructible ) destructible->save(zip);
	if ( ai ) ai->save(zip);
	if ( pickable ) pickable->save(zip);
	if ( container ) container->save(zip);
}

void Actor::load(TCODZip& zip) {
	x=zip.getInt();
	y=zip.getInt();
	ch=zip.getInt();
	col=zip.getColor();
	strcpy( this->name, zip.getString() );
	blocks=zip.getInt();
	// load component flags
	bool hasAttacker=zip.getInt();
	bool hasDestructible=zip.getInt();
	bool hasAi=zip.getInt();
	bool hasPickable=zip.getInt();
	bool hasContainer=zip.getInt();
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
}

void Attacker::save(TCODZip& zip) {
	zip.putFloat(power);
}

void Attacker::load(TCODZip& zip) {
	power = zip.getFloat();
}

void Container::save(TCODZip& zip) {
	zip.putInt(size);
	zip.putInt(inventory.size());
	for (Actor** it=inventory.begin(); it!=inventory.end(); it++) {
		(*it)->save(zip);
	}
}

void Container::load(TCODZip& zip) {
	size = zip.getInt();
	int nbActors = zip.getInt();
	while ( nbActors > 0 ) {
		Actor* actor = new Actor(0,0,0,"",TCODColor::white);
		actor->load(zip);
		inventory.push(actor);
		nbActors--;
	}
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
	useable->save(zip);
}

void Pickable::load(TCODZip& zip) {
	useable = Useable::create(zip);
}

Useable* Useable::create(TCODZip& zip) {
	UseableType type = (UseableType)zip.getInt();
	Useable* useable = NULL;
	switch(type) {
		case HEALER : useable = new Healer(0); break;
		case LIGHTNING : useable = new LightningBolt(0,0); break;
		case FIREBALL : useable = new Fireball(0,0,0); break;
		case CONFUSER : useable = new Confuser(0,0); break;
	}
	useable->load(zip);
	return useable;
}

void Healer::save(TCODZip& zip) {
	zip.putInt(HEALER);
	zip.putFloat(amount);
}

void Healer::load(TCODZip& zip) {
	amount = zip.getFloat();
}

void LightningBolt::save(TCODZip& zip) {
	zip.putInt(LIGHTNING);
	zip.putFloat(range);
	zip.putFloat(damage);
}

void LightningBolt::load(TCODZip& zip) {
	range = zip.getFloat();
	damage = zip.getFloat();
}

void Fireball::save(TCODZip& zip) {
	zip.putInt(FIREBALL);
	zip.putFloat(range);
	zip.putFloat(damage);
	zip.putFloat(areaRange);
}

void Fireball::load(TCODZip& zip) {
	range = zip.getFloat();
	damage = zip.getFloat();
	areaRange = zip.getFloat();
}

void Confuser::save(TCODZip& zip) {
	zip.putInt(CONFUSER);
	zip.putFloat(range);
	zip.putFloat(nbTurns);
}

void Confuser::load(TCODZip& zip) {
	range = zip.getFloat();
	nbTurns = zip.getFloat();
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