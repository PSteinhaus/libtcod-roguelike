class Useable : public Persistent
{
public:
	Useable(TargetSelector *selector, Effect *effect);
	virtual ~Useable();
	virtual bool use(Actor* owner, Actor* user);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	TargetSelector *selector;
	Effect *effect;
};

class Pickable : public Persistent
{
public:
	Useable* useable;

	Pickable();
	virtual ~Pickable();
	bool pick(Actor* owner, Actor* picker);
	bool use(Actor* owner, Actor* user);
	void drop(Actor* owner, Actor* carrier);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};