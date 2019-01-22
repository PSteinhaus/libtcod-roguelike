class Effect : public Persistent
{
public :
	virtual void applyTo(Actor *actor) = 0;
	virtual ~Effect() {};
	static Effect* create(TCODZip& zip);
protected:
	enum EffectType {
		HEALTH, CONFUSE
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