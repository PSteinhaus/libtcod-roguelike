class BodyPart : public Persistent
{
public:
	virtual ~BodyPart();
	enum EquipmentSlot {
		HAND, RING, HEAD, AMULET, TORSO, LEGS, FEET
	};
	Actor* equip(Actor* owner, Actor* equipment, bool replace);
	void load(TCODZip& zip);
	void save(TCODZip& zip);
private:
	std::vector<std::pair<EquipmentSlot,Actor*>> slots;
	//TCODList<std::pair<EquipmentSlot,Actor*>> slots;
	friend class Body;
};

class Body : public Persistent
{
public:
	virtual ~Body();
	Actor* equip(Actor* owner, Actor* equipment);
	void load(TCODZip& zip);
	void save(TCODZip& zip);
private:
	TCODList<BodyPart*> parts;
};