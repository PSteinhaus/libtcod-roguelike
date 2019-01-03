class Actor : public Persistent
{
public:
	int x,y;		// position on map
	int ch;			// ascii code
	TCODColor col;	// color
	char name [ MAX_NAME_LENGTH ]; // the actor's name
	int blocks;		// can we walk on this actor?
	Attacker* attacker; // something that deals damage
	Destructible* destructible; // something that can be damaged
	Ai* ai;			// something self-updating
	Pickable* pickable; // something that can be picked up and used
	Container* container; // something that can contain actors

	Actor(int x, int y, int ch, const char* name, const TCODColor& col);
	virtual ~Actor();
	void update();
	void render() const;
	float getDistance(int cx, int cy) const;
	void load(TCODZip& zip);
	void save(TCODZip& zip);
};