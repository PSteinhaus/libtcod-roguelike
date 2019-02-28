// all functions classifying as "map modification tools" go here, including members of "Map"

#include "main.hpp"
#include <cmath>

void Map::setField(int x, int y, Map::FieldType fieldType) {
	tiles[x+y*width].fieldType = fieldType;
	switch(fieldType) {
		case GRASS :
		case FLOOR :
			map->setProperties(x,y,true,true);
		break;
		case TREE :
		case WALL :
			map->setProperties(x,y,false,false);
		break;

		default:
		break;
	}
}

void Map::setRect(int x, int y, int width0, int height0, Map::FieldType fieldType) {
	for (int i=0; i<width0 && x+i<width; i++)
		for (int j=0; j<height0 && y+j<height; j++)
			setField(x+i,y+j,fieldType);
}

void Map::setEllipse(int x, int y, int width0, int height0, Map::FieldType fieldType) {
	setEllipseGrad(x,y, width0,height0, 1, fieldType);
}

void Map::setEllipseGrad(int x, int y, int width0, int height0, float gradStart, FieldType fieldType) {
	TCODRandom* rng = TCODRandom::getInstance();
	int xCenter = width0/2; 	// coordinates of the center of the ellipse
	int yCenter = height0/2;	// relative to x and y
	for (int i=0; i<width0 && i<width; i++)
		for (int j=0; j<height0 && j<height; j++) {
			int iFrC = i-xCenter; // i, measured from the center of the ellipse
			int jFrC = j-yCenter; // i, measured from the center of the ellipse
			float radius = pow(static_cast<float>(iFrC)/xCenter, 2) + pow(static_cast<float>(jFrC)/yCenter, 2);
			if( radius <= 1 && inMap(x+i,y+j) ) {
				if ( radius <= gradStart ) { setField(x+i,y+j,fieldType); continue; }
				if ( (radius-gradStart) / (1-gradStart) <= rng->getFloat(0,1) ) setField(x+i,y+j,fieldType); // just draw an ellipse yourself if you want to know what the first term does...
			}
		}
}


void Map::dig(int x1, int y1, int x2, int y2) {
	// make sure that x1 < x2 and y1 < y2
	if ( x2 < x1 ) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if ( y2 < y1 ) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	setRect(x1,y1, x2-x1+1, y2-y1+1, FieldType::FLOOR );

	/*
	for (int tileX = x1; tileX <= x2; tileX++) {
		for (int tileY = y1; tileY <= y2; tileY++) {
			map->setProperties(tileX,tileY,true,true);
		}
	} */
}

bool Map::connectRoomsRandom(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2, int randomness, int thickness) {
	TCODRandom* rng = TCODRandom::getInstance();
	int xStart; int yStart;
	if ( !randomFreeField(x1,y1, width1,height1, &xStart,&yStart, true) ) return false;
	int xEnd; int yEnd;
	if ( !randomFreeField(x2,y2, width2,height2, &xEnd,&yEnd, true) ) return false;

	int x=xStart; int y=yStart;
	while ( x!=xEnd || y!=yEnd ) {
		if ( rng->getInt(1,100) > randomness ) {
			// follow a straight path to your destination
			TCODLine::init (x,y, xEnd,yEnd);
			TCODLine::step (&x,&y);
		} else {
			// go into a random direction
			int xStep, yStep;
			do {
			xStep = rng->getInt(-1,1);
			yStep = rng->getInt(-1,1);
			} while ( !inMap(x+xStep,y+yStep) );
			x += xStep;
			y += yStep;
		}

		for (int i=0; i < thickness-1 || i==0; i++)
			for (int j=0; j < thickness; j++) {
				if ( inMap(x+i,y+j, false) ) setField(x+i,y+j, FieldType::FLOOR );
			}
		
	}
	return true;
}