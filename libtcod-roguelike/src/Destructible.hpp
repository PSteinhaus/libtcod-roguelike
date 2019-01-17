class Destructible : public Persistent {
public:
	float maxHp;	// maximum health points
	float hp;		// current health points
	float defense; 	// hit points deflected
	char corpseName [ MAX_NAME_LENGTH ];	// the actor's name once dead/destroyed

	Destructible(float maxHp, float defense, const char* corpseName );
	virtual ~Destructible() {};
	inline bool isDead() { return hp <= 0; }
	float takeDamage(Actor* owner, float damage, bool directly=false);
	virtual void die(Actor* owner);
	float heal(float amount);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
	static Destructible* create(TCODZip& zip);

protected:
	enum DestructibleType {
		MONSTER,PLAYER
	};
};

class MonsterDestructible : public Destructible {
public:
	MonsterDestructible(float maxHp, float defense, const char* corpseName);
	void die(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class PlayerDestructible : public Destructible {
public:
	PlayerDestructible(float maxHp, float defense, const char *corpseName);
	void die(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};