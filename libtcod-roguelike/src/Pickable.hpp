class Useable
{
public:
	virtual bool use(Actor* owner, Actor* user);
};

class Pickable
{
public:
	Useable* useable;

	Pickable();
	virtual ~Pickable() {};
	bool pick(Actor* owner, Actor* picker);
	bool use(Actor* owner, Actor* user);
	void drop(Actor* owner, Actor* carrier);
};

// USEABLES

class Healer : public Useable {
public:
	float amount;

	Healer(float amount);
	bool use(Actor* owner, Actor* user);
};

class LightningBolt : public Useable {
public:
	float range, damage;
	LightningBolt(float range, float damage);
	bool use(Actor* owner, Actor* user);
};

class Fireball : public LightningBolt {
public:
	float areaRange;

	Fireball(float range, float damage, float areaRange);
	bool use(Actor* owner, Actor* user);
};

class Confuser : public Useable {
public:
	float range;
	int nbTurns;
	Confuser(float range, int nbTurns);
	bool use(Actor* owner, Actor* user);
};