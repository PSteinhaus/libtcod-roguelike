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
	Gui* gui;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void update();
	void render();
	void sendToBack(Actor* actor);
	Actor* getClosestMonster(bool fovRequired,int x, int y, float range = 0.0f) const;
	bool pickATile(int* x, int* y, float maxRange = 0.0f);
};

extern Engine engine;