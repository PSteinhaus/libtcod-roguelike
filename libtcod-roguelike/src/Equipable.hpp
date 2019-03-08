class Equipable : public Persistent
{
public:
	Equipable(BodyPart::EquipmentSlot slotNeeded, Effect *equipEffect, Effect *unequipEffect);
	virtual ~Equipable();
	bool equipableOn(Actor* actor, BodyPart::EquipmentSlot slot);
	void equipOn(Actor* actor);
	void unequipOn(Actor* actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
private:
	BodyPart::EquipmentSlot slotNeeded;
	Effect *equipEffect;
	Effect *unequipEffect;
};