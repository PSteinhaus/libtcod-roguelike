class Effect : public Persistent
{
public :
	bool empty;

	Effect() : empty(false) {};
	virtual ~Effect() {};
	virtual bool applyTo(Actor *actor) { return false; };
	virtual bool applyTo(Tile *tile) { return false; };
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
	bool applyTo(Actor *actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class ConfusionEffect : public Effect {
public :
	int nbTurns;
	const char *message;

	ConfusionEffect(int nbTurns, const char *message);
	bool applyTo(Actor *actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class DoorEffect : public Effect {
public :
	int originalChar;
	DoorEffect(char originalChar);
	bool applyTo(Actor *actor);
	bool applyTo(Tile *tile);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

