class Actor
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
	~Actor();
	void update();
	//bool moveOrAttack(int x, int y);
	void render() const;
};