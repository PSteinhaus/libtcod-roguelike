#include "main.hpp"

LiquidContainer::LiquidContainer(float maxVolume) : maxVolume(maxVolume), liquid(NULL) {}

LiquidContainer::~LiquidContainer() {
	if (liquid) delete liquid;
}

Liquid* LiquidContainer::fill(Liquid* newLiquid) {
	// try to fill the container with the new liquid
	if ( liquid ) {												// if you already contain a liquid
		if ( liquid->type != newLiquid->type ) return 0; 	// and it's not of the same type as the new one -> abort
		liquid->absorb(newLiquid, maxVolume);					// else let your liquid absorb the new liquid (up to maxVolume);
	} else {													// if you do not contain a liquid
		liquid = newLiquid->split(maxVolume);					// split the new liquid (allocates a new liquid)
	}
	if ( newLiquid->empty() ) {								// if the new one is now empty
		delete newLiquid;									// delete it
		return NULL;										// and return NULL to signal the outer function that the liquid has been absorbed completely
	}
	return newLiquid;
}

Liquid* LiquidContainer::drain(float amount) {
	Liquid* returnedLiquid = NULL;
	if ( liquid ) {
		returnedLiquid = liquid->split(amount);
		if ( liquid->empty() ) {
			delete liquid;
			liquid = NULL;
		}
	}
	return returnedLiquid;
}