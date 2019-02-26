class Tile;

class Map : public Persistent
{
public:
	int width,height;
	enum FieldType {
		FLOOR, WALL, GRASS, TREE
	};

	Map(int width, int height, Chunk* chunk);
	virtual ~Map();
	void init();
	void leave();
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov();
	bool inMap(int x, int y, bool includeBorders=true) const;
	bool isWall(int x, int y) const;
	inline FieldType fieldTypeAt(int x, int y) const;
	bool canWalk(int x, int y) const;
	void render() const;
	void addMonster(int x, int y);
	void addItem(int x, int y);
	void load(TCODZip& zip);
	void save(TCODZip& zip);

	// map modification tools
	void setField	(int x, int y, FieldType fieldType);
	void setRect	(int x, int y, int width0, int height0, FieldType fieldType);
	void setEllipse	(int x, int y, int width0, int height0, FieldType fieldType);
	inline void fill(FieldType fieldType) { setRect(0,0,width,height,fieldType); }

protected:
	Tile* tiles;
	TCODMap* map;
	Chunk* chunk;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	bool connectRoomsRandom(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2, int randomness, int thickness );
	void fillRoom(int x1, int y1, int x2, int y2, bool first);
	bool randomFreeField(int x0, int y0, int width, int height ,int* x, int* y, bool wall=false);
};

struct Tile : public Persistent {
	bool explored;	// has the player already seen this tile?
	Map::FieldType fieldType;

	Tile() : explored(false), fieldType(Map::FieldType::FLOOR) {}
	void load(TCODZip& zip);
	void save(TCODZip& zip);
};

Map::FieldType Map::fieldTypeAt(int x, int y) const { return tiles[x+y*width].fieldType; }