struct Tile : public Persistent {
	bool explored;	// has the player already seen this tile?
	Tile() : explored(false) {}
	void load(TCODZip& zip);
	void save(TCODZip& zip);
};

class Map : public Persistent
{
public:
	int width,height;

	Map(int width, int height);
	virtual ~Map();
	void init();
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov();
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	void render() const;
	void addMonster(int x, int y);
	void addItem(int x, int y);
	void load(TCODZip& zip);
	void save(TCODZip& zip);

protected:
	Tile* tiles;
	TCODMap* map;
	enum BiomeType {
		TUTORIAL, CAVE
	} biome;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void connectRoomsRandom(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2, int randomness, int thickness );
	void fillRoom(int x1, int y1, int x2, int y2, bool first);
	void randomFreeField(int x0, int y0, int width, int height ,int* x, int* y);
};