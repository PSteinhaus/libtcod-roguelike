class Actor
{
public:
	int x,y;		// position on map
	int ch;			// ascii code
	TCODColor col;	// color

	Actor(int x, int y, int ch, const TCODColor &col);
	~Actor();
	void render() const;
};