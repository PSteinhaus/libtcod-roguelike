#include "main.hpp"

Body::~Body() {
	parts.clearAndDelete();
}

Actor* Body::equip(Actor* owner, Actor* equipment) {
	Actor* returnVal = equipment;
	// first check if this actor can be equiped at all
	if ( !equipment->pickable || !equipment->pickable->equipable ) return returnVal;
	// try to equip on any bodypart without replacing anything (try to find a free slot)
	for ( auto it=parts.begin(); it!=parts.end(); it++ ) {
		returnVal = (*it)->equip(owner,equipment, false);
		if ( returnVal != equipment ) return returnVal; // successfully equiped
	}
	// try to equip on any bodypart (try to find a slot, even if it's already filled)
	for ( auto it=parts.begin(); it!=parts.end(); it++ ) {
		returnVal = (*it)->equip(owner,equipment, true);
		if ( returnVal != equipment ) return returnVal; // successfully equiped
	}
	// failure
	return returnVal;
}

BodyPart::~BodyPart() {
	for ( auto it=slots.begin(); it!=slots.end(); it++ ) {
		auto slot = *it;
		if ( slot.second ) delete slot.second;
	}
}

Actor* BodyPart::equip(Actor* owner, Actor* equipment, bool replace) {
	Actor* returnVal = equipment;
	for ( auto it=slots.begin(); it!=slots.end(); it++ ) {			// check all slots
		std::pair<EquipmentSlot,Actor*> slot = *it;
		if ( slot.second==NULL || replace )							// if the slot is free, or can be replaced
			if( equipment->equipableOn(owner, slot.first) ) {		// and the actor can be equiped there:
				returnVal = slot.second;							// return the old equipment of this slot
				if (returnVal) returnVal->unequipOn(owner);			// unequip it
				equipment->equipOn(owner);							// equip the new equipment
				slot.second = equipment;							// and save the new equipment there
				break;
			}
	}
	return returnVal;
}