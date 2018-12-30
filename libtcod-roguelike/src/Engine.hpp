class Engine
{
public :
	TCODList<Actor*> actors;
	Actor* player;
	Map* map;
	int fovRadius;
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;

	Engine();
	~Engine();
	void update();
	void render();
};

extern Engine engine;