static const int MAX_NAME_LENGTH = 12;

class Actor
{
public:
	int x,y;		// position on map
	int ch;			// ascii code
	TCODColor col;	// color
	char name [ MAX_NAME_LENGTH ]; // the actor's name

	Actor(int x, int y, int ch, const char* name, const TCODColor& col);
	void update();
	bool moveOrAttack(int x, int y);
	void render() const;
};