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

Stomach::Stomach(float maxNutrition, float need, float digestionRate, int size, float maxVolume, float nutrition) : Container::Container(size,maxVolume),
	nutrition(nutrition),maxNutrition(maxNutrition),need(need),digestionRate(digestionRate)
{
	// fill stomach completely as standard ( nutrition=-1 is standard )
	if ( nutrition == -1 ) nutrition = maxNutrition;
}

// digest AND control hunger/starvation (basically a kind of stomach AI)
void Stomach::digest(Actor* owner) {
	// check whether there is something digestible inside the stomach and if true digest the first thing (the oldest)
	for(Actor** it=inventory.begin(); it != inventory.end(); it++) {
		Actor* food = *it;
		if ( food->nutrition > 0 ) {
			food->nutrition -= digestionRate;
			nutrition += digestionRate;
			if ( food->nutrition < 0 ) {
				nutrition += food->nutrition;
			}
			// if completely digested delete the actor
			if (food->nutrition <= 0) {
				inventory.remove(food);
				delete food;
			}
			break;
		}
	}
	if (owner->destructible) {
		// starvation
		if (nutrition <= maxNutrition/10) {
			owner->destructible->takeDamage(owner, 1, true);
			if ( owner == engine.player ) engine.gui->message(TCODColor::desaturatedRed,"You're starving.");
		} else if ( nutrition > maxNutrition/4 ) {
			// heal the owner of this stomach a little
			owner->destructible->heal( need/100 );
		}
	}
	// hunger
	nutrition -= need;
	if ( nutrition < 0 ) nutrition = 0;
	if (nutrition > maxNutrition) nutrition = maxNutrition;
}