class Equipable : public Persistent
{
public:
	Equipable() = default;
	Equipable(BodyPart::EquipmentSlot slotNeeded, Effect *equipEffect, Effect *unequipEffect);
	virtual ~Equipable();
	bool equipableOn(Actor* actor, BodyPart::EquipmentSlot slot);
	void equipOn(Actor* actor);
	void unequipOn(Actor* actor);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

	bool implicitlyUseable; // a very strange bool that says whether or not this piece of Equipment 
							// (when worn by the player) can be automatically used (using Strg+Numpad)
							// (but of course only if the item can be used at all...)
private:
	BodyPart::EquipmentSlot slotNeeded;
	Effect *equipEffect;
	Effect *unequipEffect;
};