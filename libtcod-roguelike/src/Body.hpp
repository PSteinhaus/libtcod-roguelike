class BodyPart : public Persistent
{
public:
	BodyPart() = default;
	BodyPart(int numOfSlots, ...);
	virtual ~BodyPart();
	enum EquipmentSlot {
		HAND, RING, HEAD, AMULET, TORSO, LEGS, FEET
	};
	std::vector<std::pair<EquipmentSlot,Actor*>> slots;
	Actor* equip(Actor* owner, Actor* equipment, bool replace);
	bool unequip(Actor* owner, Actor* equipment);
	void load(TCODZip& zip);
	void save(TCODZip& zip);
};

class Body : public Persistent
{
public:
	TCODList<BodyPart*> parts;
	virtual ~Body();
	Actor* equip(Actor* owner, Actor* equipment);
	bool unequip(Actor* owner, Actor* equipment);
	void load(TCODZip& zip);
	void save(TCODZip& zip);
};