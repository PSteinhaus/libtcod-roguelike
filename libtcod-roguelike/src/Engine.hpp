#include <tuple>
#include <map>
#include <list>

typedef std::pair<int,int> Point;
/*
class ActorIterator {
public:
	ActorIterator(bool begin=true);
	ActorIterator& operator++ ();     // prefix ++
	Actor* actor();

private:
	std::map< Point, TCODList<Actor*> >::iterator itWorld;
	Actor** it;
};
*/
class Engine
{
public :
	std::list<Actor*> actors;
	std::map< Point, TCODList<Actor*> > actorsAt;
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
	void changeChunk(int dx, int dy, int dz);
	void update();
	void render();
	// Actor management stuff
	std::list<Actor*>::iterator actorsBegin();
	std::list<Actor*>::iterator actorsEnd();
	void sendToBack(Actor* actor);
	void addActor(Actor* actor, int x=-1, int y=-1);
	void addActorToPos(Actor* actor, int x=-1, int y=-1);
	void removeActor(Actor* actor, bool destroy=true);
	std::list<Actor*>::iterator removeActor(std::list<Actor*>::iterator iterator, bool destroy=true);
	void removeActorFromPos(Actor* actor);
	void moveActor(Actor* actor, int x, int y);
	int totalActors() const;

	Actor* getClosestMonster(bool fovRequired,int x, int y, float range = 0.0f);
	bool pickATile(int* x, int* y, float maxRange = 0.0f);
	void init();
	void terminate();
	void load();
	void save();
	void gameMenu();
	Actor* getActor(int x, int y, bool aliveRequired = true);
	TCODList<Actor*> getActors(int x, int y, bool aliveRequired = true);
	inline Chunk* currentChunk() { return world[x][y][depth]; }
};

extern Engine engine;