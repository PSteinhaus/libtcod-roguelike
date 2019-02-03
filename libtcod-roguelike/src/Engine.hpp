class Engine
{
public :
	TCODList<Actor*> actors;
	Actor* player;
	Map* map;
	TCOD_key_t lastKey;
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT,
		EXIT
	} gameStatus;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	static const int worldSize = 100;
	static const int worldDepth = 10;
	int depth;
	Gui* gui;
	int x;
	int y;
	Chunk* world[worldSize][worldSize][worldDepth];

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void nextLevel();
	void update();
	void render();
	void sendToBack(Actor* actor);
	Actor* getClosestMonster(bool fovRequired,int x, int y, float range = 0.0f) const;
	bool pickATile(int* x, int* y, float maxRange = 0.0f);
	void init();
	void terminate();
	void load();
	void save();
	void gameMenu();
	Actor* getActor(int x, int y, bool aliveRequired = true) const;
	inline Chunk* currentChunk() { return world[x][y][depth]; }
};

extern Engine engine;