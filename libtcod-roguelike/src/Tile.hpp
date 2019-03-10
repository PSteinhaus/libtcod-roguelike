class Tile : public Persistent {
public:
	bool explored;	// has the player already seen this tile?
	virtual bool transparent() = 0;	// can the player (and possibly monsters) look through this tile?
	virtual bool walkable() = 0;	// can the player (and possibly monsters) walk over this tile?
	virtual TCODColor color() const = 0;
	virtual int ch() const = 0;

	enum FieldType {
		FLOOR, WALL, GRASS, TREE
	};
	virtual FieldType fieldType() const = 0;

	Tile() : explored(false) {};
	virtual ~Tile() {};
	void render(int x, int y) const;
	virtual void load(TCODZip& zip);
	virtual void save(TCODZip& zip);
	static Tile* create(TCODZip& zip);
};

class FloorTile : public Tile {
public:
	FieldType fieldType() const { return FLOOR; };
	bool transparent() { return true; }
	bool walkable() { return true; }
	TCODColor color() const { return TCODColor::white; }
	int ch() const { return '.'; }
};

class WallTile : public Tile {
public:
	FieldType fieldType() const { return WALL; };
	bool transparent() { return false; };
	bool walkable() { return false; };
	TCODColor color() const { return TCODColor::white; }
	int ch() const { return '#'; }
};

class GrassTile : public Tile {
public:
	FieldType fieldType() const { return GRASS; };
	bool transparent() { return true; };
	bool walkable() { return true; };
	TCODColor color() const { return TCODColor::green; }
	int ch() const { return '.'; }
};

class TreeTile : public Tile {
public:
	FieldType fieldType() const { return TREE; };
	bool transparent() { return false; };
	bool walkable() { return false; };
	TCODColor color() const { return TCODColor::green; }
	int ch() const { return '#'; }
};