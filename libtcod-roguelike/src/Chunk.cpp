#include "main.hpp"

Chunk::Chunk(BroadType broadType, bool persistentMap) : persistentMap(persistentMap),broadType(broadType) {
	switch(broadType) {
		case CAVE:	terrainData.startWithWalls	= true;
					terrainData.roomCreation 	= TerrainData::RoomCreation::DIG_RANDOM;
					terrainData.tunnelCreation	= TerrainData::TunnelCreation::RANDOM;

					biomeData.creatures[ActorRep::RAT] = 20;
		break;
		case PLAINS:terrainData.startWithWalls	= false;
					terrainData.roomCreation 	= TerrainData::RoomCreation::NONE;
					terrainData.tunnelCreation	= TerrainData::TunnelCreation::NONE;
		break;
		default:
		break;
	}
}