#include "main.hpp"

Chunk::Chunk(BroadType broadType, bool persistentMap, int depth) : persistentMap(persistentMap),map(NULL),broadType(broadType) {
	switch(broadType) {
		case CAVE:	terrainData.startWithWalls	= true;
					terrainData.roomCreation 	= TerrainData::RoomCreation::DIG_RANDOM;
					terrainData.tunnelCreation	= TerrainData::TunnelCreation::RANDOM;
					if ( depth < engine.worldDepth-1 )
						terrainData.numDownStairs 	= 2;
					terrainData.numUpStairs 	= 2;

					biomeData.creatures[ActorRep::RAT] = 20;
		break;
		case PLAINS:terrainData.startWithWalls	= false;
					terrainData.roomCreation 	= TerrainData::RoomCreation::NONE;
					terrainData.tunnelCreation	= TerrainData::TunnelCreation::NONE;
					terrainData.numDownStairs = 1;
		break;
		default:
		break;
	}
}