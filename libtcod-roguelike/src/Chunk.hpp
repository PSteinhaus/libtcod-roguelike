#include <map>

struct Chunk : public Persistent
{
	static const int size = 100;
	bool persistentMap;
	Map* map;
	enum BroadType {
		CAVE, PLAINS, FOREST, ROCK
	} broadType;

	struct TerrainData {
		bool startWithWalls;
		enum class RoomCreation { NONE, DIG, DIG_RANDOM } roomCreation = RoomCreation::NONE;
		enum class TunnelCreation { NONE, STRAIGHT, RANDOM } tunnelCreation = TunnelCreation::NONE;
		int numDownStairs = 0;
		int numUpStairs = 0;
	} terrainData;

	struct BiomeData {
		// collection of creatures and plants inhibiting this chunk (this also includes trees, bushes, grass, etc.)
		std::map<ActorRep::Name,int> creatures;
		// TCODList<Plants*> plants;
	} biomeData;

	Chunk(BroadType broadType, bool persistentMap=false, int depth=0);
	virtual ~Chunk() {};
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};