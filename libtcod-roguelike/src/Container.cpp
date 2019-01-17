#include "main.hpp"

Container::Container(int size, float maxVolume) : size(size),maxVolume(maxVolume) {
}

Container::~Container() {
	inventory.clearAndDelete();
}

bool Container::add(Actor* actor) {
	if ( size > 0 && inventory.size() >= size ) {
		// inventory full
		return false;
	}
	// check for free space (volume)
	if ( getVolume() + actor->getVolume() > maxVolume ) return false;

	inventory.push(actor);
	return true;
}

void Container::remove(Actor* actor) {
	inventory.remove(actor);
}

// get totalVolume
float Container::getVolume() {
	float totalVolume = 0;
	for (Actor** it=inventory.begin(); it != inventory.end(); it++) {
		Actor* invActor = *it;
		totalVolume += invActor->getVolume();
	}
	return totalVolume;
}

Stomach::Stomach(float maxNutrition, float need, int size, float maxVolume, float nutrition) : Container::Container(size,maxVolume),
	nutrition(nutrition),maxNutrition(maxNutrition),need(need)
{
	// fill stomach completely as standard ( nutrition=-1 is standard )
	if ( nutrition == -1 ) nutrition = maxNutrition;
}