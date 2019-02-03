#include <map>

struct Chunk : public Persistent
{
	static const int size = 100;
	bool persistentMap;
	enum BroadType {
		CAVE, PLAINS, FOREST, ROCK
	} broadType;

	struct TerrainData {
		bool startWithWalls;
		enum class RoomCreation 	{ NONE, DIG, DIG_RANDOM } roomCreation;
		enum class TunnelCreation { NONE, STRAIGHT, RANDOM } tunnelCreation;
	} terrainData;

	struct BiomeData {
		// collection of creatures and plants inhibiting this chunk (this also includes trees, bushes, grass, etc.)
		std::map<ActorRep::CreatureName,int> creatures;
		// TCODList<Plants*> plants;
	} biomeData;

	Chunk(BroadType broadType, bool persistentMap=false);
	virtual ~Chunk() {};
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};