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
};

extern Engine engine;