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
		DEFEAT
	} gameStatus;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	int depth;
	Gui* gui;
	int x;
	int y;

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
	Actor* getActor(int x, int y, bool aliveRequired = true) const;
};

extern Engine engine;