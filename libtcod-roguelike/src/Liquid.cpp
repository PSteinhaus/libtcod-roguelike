#include "main.hpp"

bool Liquid::empty() const {
	return volume == 0;
}

float Liquid::decreaseVolume(float amount) {
	float actuallyDecreased = amount;
	volume -= amount;
	if (volume<0) {
		actuallyDecreased += volume;
		volume = 0;
	}
	return actuallyDecreased;
}

float Liquid::increaseVolume(float amount, float maxVolume) {
	float actuallyIncreased = amount;
	volume += amount;
	if (volume>maxVolume) {
		actuallyIncreased -= volume - maxVolume;
		volume = maxVolume;
	}
	return actuallyIncreased;
}

void Liquid::absorb(Liquid* liquid, float maxVolumeAbsorbed, float maxVolumeReached) {
	increaseVolume( liquid->decreaseVolume(maxVolumeAbsorbed), maxVolumeReached );
}

Liquid* Liquid::split(float amount) {
	float newLiquidVolume = decreaseVolume(amount);
	return createByType(type, newLiquidVolume);
}

Liquid* Liquid::createByType(LiquidType type, float volume) {
	switch(type) {
		case WATER: return new WaterLiquid(volume);
		default: return NULL;
	}
}
