/* a=target variable, b=bit number to act upon 0-n */
#ifndef BIT_MACROS
#define BIT_MACROS
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1
#endif

#include "main.hpp"

void saveToBitArray(bool value, char& bitArray, int& bitNum, TCODZip& zip ) {
	// check for free space, if false save the bitArray to zip and start a new one
	if (bitNum >= 8) {
		zip.putChar(bitArray);
		bitArray = 0;
		bitNum = 0;
	}
	if (value) BIT_SET(bitArray,bitNum++);
}

bool loadFromBitArray(char& bitArray, int& bitNum, TCODZip& zip) {
	// check for invalid index, if true load the next char from zip
	if ( bitNum >= 8 ) {
		bitArray = zip.getChar();
		bitNum = 0;
	}
	return BIT_CHECK(bitArray,bitNum++);
}