class Pickable : public Persistent
{
public:
	Useable* useable;
	Digestor* digestor;
	Equipable* equipable;

	Pickable();
	virtual ~Pickable();
	bool pick(Actor* owner, Actor* picker);
	bool use(Actor* owner, Actor* user);
	void drop(Actor* owner, Actor* carrier);
	void vomit(Actor* owner, Actor* carrier);
	bool eat(Actor* owner, Actor* eater);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};