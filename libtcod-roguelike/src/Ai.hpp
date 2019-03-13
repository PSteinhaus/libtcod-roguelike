class Ai : public Persistent
{
public:
	virtual ~Ai() {};
	virtual void update(Actor* owner) = 0;
	static Ai* create (TCODZip& zip);
protected:
	enum AiType {
		MONSTER,CONFUSED,PLAYER
	};
};

class PlayerAi : public Ai {
public:
	void update(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	bool interactAt(Actor* owner, int targetX, int targetY);
	bool moveOrAttack(Actor* owner, int targetX, int targetY);
	void handleActionKey(Actor* owner, int ascii);
	Actor* choseFromInventory(Actor* owner);
	Actor* choseFromEquipment(Actor* owner);
};

class MonsterAi : public Ai {
public:
	void update(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	int moveCount;
	
	void moveOrAttack(Actor* owner, int targetX, int targetY);
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int nbTurns, Ai* oldAi);
	void update(Actor* owner);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	int nbTurns;
	Ai* oldAi;
};