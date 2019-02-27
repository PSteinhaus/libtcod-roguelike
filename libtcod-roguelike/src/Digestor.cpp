#include "main.hpp"

Digestor::Digestor(float nutrition) : nutrition(nutrition) {
}

void Digestor::operator()(Actor* owner, Actor* eater) {
	Stomach* stomach = eater->stomach;

	if ( nutrition > 0 ) {		
		nutrition -= stomach->digestionRate;
		stomach->nutrition += stomach->digestionRate;
		if ( nutrition < 0 ) { // make sure not more nutrition is absorbed than there is in this food
			stomach->nutrition += nutrition;
		}
	} 
	// if completely digested delete the actor
	if (nutrition <= 0) {
		stomach->inventory.remove(owner);
		delete owner;
	}
}