class Effect : public Persistent
{
public :
	bool empty;

	Effect() : empty(false) {};
	virtual ~Effect() {};
	virtual void applyTo(Actor *actor) = 0;
	static Effect* create(TCODZip& zip);
	virtual void save(TCODZip& zip);
	virtual void load(TCODZip& zip);
protected:
	enum EffectType {
		HEALTH, CONFUSE, DOOR
	};
};

class HealthEffect : public Effect {
public :
	float amount;
	const char *message;

	HealthEffect(float amount, const char *message);
	void applyTo(Actor *actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class ConfusionEffect : public Effect {
public :
	int nbTurns;
	const char *message;

	ConfusionEffect(int nbTurns, const char *message);
	void applyTo(Actor *actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class DoorEffect : public Effect {
public :
	int originalChar;
	DoorEffect(char originalChar);
	void applyTo(Actor *actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};