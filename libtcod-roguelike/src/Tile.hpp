class Tile : public Persistent {
public:
	bool explored;	// has the player already seen this tile?
	int x,y;	// coordinates on map
	virtual bool transparent() = 0;	// can the player (and possibly monsters) look through this tile?
	virtual bool walkable() = 0;	// can the player (and possibly monsters) walk over this tile?
	virtual TCODColor color() const = 0;
	virtual int ch() const = 0;

	enum FieldType {
		FLOOR, WALL, GRASS, TREE
	};
	virtual FieldType fieldType() const = 0;

	Tile() = default;
	Tile(int x, int y) : explored(false), x(x), y(y) {};
	//Tile(Tile* oldTile) : explored(oldTile->explored), x(oldTile->x), y(oldTile->y) {};
	virtual ~Tile() {};
	void render() const;
	virtual void load(TCODZip& zip);
	virtual void save(TCODZip& zip);
	static Tile* create(TCODZip& zip);

	// interaction
	virtual bool applyCut(float cutValue) { return false; };
};

class FloorTile : public Tile {
public:
	FloorTile() = default;
	FloorTile(int x, int y) : Tile::Tile(x,y) {};
	FieldType fieldType() const { return FLOOR; };
	bool transparent() { return true; }
	bool walkable() { return true; }
	TCODColor color() const { return TCODColor::white; }
	int ch() const { return '.'; }
};

class WallTile : public Tile {
public:
	WallTile() = default;
	WallTile(int x, int y) : Tile::Tile(x,y) {};
	FieldType fieldType() const { return WALL; };
	bool transparent() { return false; };
	bool walkable() { return false; };
	TCODColor color() const { return TCODColor::white; }
	int ch() const { return '#'; }
};

class GrassTile : public Tile {
public:
	GrassTile() = default;
	GrassTile(int x, int y) : Tile::Tile(x,y) {};
	FieldType fieldType() const { return GRASS; };
	bool transparent() { return true; };
	bool walkable() { return true; };
	TCODColor color() const { return TCODColor::green; }
	int ch() const { return '.'; }
};

class TreeTile : public Tile {
public:
	TreeTile() = default;
	TreeTile(int x, int y) : Tile::Tile(x,y) {};
	FieldType fieldType() const { return TREE; };
	bool transparent() { return false; };
	bool walkable() { return false; };
	TCODColor color() const { return TCODColor::green; }
	int ch() const { return '#'; }

	bool applyCut(float cutValue);
};