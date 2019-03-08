#include "main.hpp"

Equipable::Equipable(BodyPart::EquipmentSlot slotNeeded, Effect* equipEffect, Effect* unequipEffect) :
slotNeeded(slotNeeded), equipEffect(equipEffect), unequipEffect(unequipEffect)
{}

Equipable::~Equipable() {
	if ( equipEffect ) delete equipEffect;
	if ( unequipEffect ) delete unequipEffect;
}

bool Equipable::equipableOn(Actor* actor, BodyPart::EquipmentSlot slot) {
	// for now do not check any Actor requirements, just check the slot
	return slot == slotNeeded;
}

void Equipable::equipOn(Actor* actor) {
	equipEffect->applyTo(actor);
}

void Equipable::unequipOn(Actor* actor) {
	unequipEffect->applyTo(actor);
}
