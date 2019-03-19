class Map : public Persistent
{
public:
	int width,height;
	/*
	enum FieldType {
		FLOOR, WALL, GRASS, TREE
	};*/

	Map(int width, int height, Chunk* chunk);
	virtual ~Map();
	void init();
	void leave();
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov();
	void computeTCODMapAt(int x, int y);
	void computeTileMapAt(int x, int y);
	bool inMap(int x, int y, bool includeBorders=true) const;
	bool isWall(int x, int y) const;
	Tile* tileAt(int x, int y) const;
	bool canWalk(int x, int y) const;
	void render(TCODConsole* con, int xOffset, int yOffset) const;
	void addMonster(int x, int y);
	void addItem(int x, int y);
	void addActor(Actor* actor);
	void removeActor(Actor* actor);
	void loadSavedActors();
	void load(TCODZip& zip);
	void save(TCODZip& zip);

	// map modification tools
	void setField	(int x, int y,Tile::FieldType fieldType);
	void setField	(Tile* tile, Tile::FieldType fieldType);
	void setRect	(int x, int y, int width0, int height0, Tile::FieldType fieldType);
	void setEllipse	(int x, int y, int width0, int height0, Tile::FieldType fieldType);
	void setEllipseGrad	(int x, int y, int width0, int height0, float gradStart, Tile::FieldType fieldType);
	inline void fill(Tile::FieldType fieldType) { setRect(0,0,width,height,fieldType); }

protected:
	Tile** tiles;
	TCODMap* map;
	TCODMap* tileMap;
	Chunk* chunk;
	TCODList<Actor*> savedActors;
	friend class BspListener;
	friend class Engine;
	friend class Chunk;

	void dig(int x1, int y1, int x2, int y2);
	bool connectRoomsRandom(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2, int randomness, int thickness );
	void fillRoom(int x1, int y1, int x2, int y2, bool first);
	bool randomFreeField(int x0, int y0, int width, int height ,int* x, int* y, bool wall=false);
};