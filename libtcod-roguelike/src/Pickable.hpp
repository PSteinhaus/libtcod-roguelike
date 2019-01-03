class Useable : public Persistent
{
public:
	virtual bool use(Actor* owner, Actor* user);
	static Useable* create(TCODZip& zip);

protected:
	enum UseableType {
		HEALER,LIGHTNING,FIREBALL,CONFUSER
	};
};

class Pickable : public Persistent
{
public:
	Useable* useable;

	Pickable();
	virtual ~Pickable() {};
	bool pick(Actor* owner, Actor* picker);
	bool use(Actor* owner, Actor* user);
	void drop(Actor* owner, Actor* carrier);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

// USEABLES

class Healer : public Useable {
public:
	float amount;

	Healer(float amount);
	bool use(Actor* owner, Actor* user);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class LightningBolt : public Useable {
public:
	float range, damage;
	LightningBolt(float range, float damage);
	bool use(Actor* owner, Actor* user);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class Fireball : public LightningBolt {
public:
	float areaRange;

	Fireball(float range, float damage, float areaRange);
	bool use(Actor* owner, Actor* user);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class Confuser : public Useable {
public:
	float range;
	int nbTurns;
	Confuser(float range, int nbTurns);
	bool use(Actor* owner, Actor* user);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};