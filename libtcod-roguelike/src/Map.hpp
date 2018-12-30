struct Tile {
	bool explored;	// has the player already seen this tile?
	Tile() : explored(false) {}
};

class Map
{
public:
	int width,height;

	Map(int width, int height);
	~Map();
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov();
	bool isWall(int x, int y) const;
	bool canWalk(int x, int y) const;
	void render() const;
	void addMonster(int x, int y);

protected:
	Tile* tiles;
	TCODMap* map;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(int x1, int y1, int x2, int y2, bool first);
};