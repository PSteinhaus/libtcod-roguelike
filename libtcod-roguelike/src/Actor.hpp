class Actor : public Persistent
{
public:
	int x,y;		// position on map
	int ch;			// ascii code
	TCODColor col;	// color
	char name [ MAX_NAME_LENGTH ]; // the actor's name
	bool blocks;		// can we walk on this actor?
	bool transparent;	// can we see through this actor?
	bool fovOnly;		// only display when in fov
	Attacker* attacker; // something that deals damage
	Destructible* destructible; // something that can be damaged
	Ai* ai;			// something self-updating
	Pickable* pickable; // something that can be picked up and used
	Container* container; // something that can contain actors
	Stomach* stomach; // a container that can contain actors to be digested
	Useable* interactable; // whether and how you can interact with this actor 
	float volume;	// 1 = 250ml
	float weight;	// 1 = 1g
	ActorRep::Name actorRepName; // under which (if any) name (enumerator) this Actor can be found

	Actor(int x, int y, int ch, const char* name, const TCODColor& col, float volume = 0, float weight = 0);
	virtual ~Actor();
	void update();
	void render() const;
	float getDistance(int cx, int cy) const;
	float getVolume() const;
	void switchBlocking();
	void switchTransparent();
	void load(TCODZip& zip);
	void save(TCODZip& zip);
};