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
};

// USEABLES

class Healer : public Useable {
public:
	float amount;

	Healer(float amount);
	bool use(Actor* owner, Actor* user);
};