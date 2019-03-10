#include "main.hpp"

void Tile::render(int x, int y) const {
	if ( engine.map->isInFov(x,y) )
		TCODConsole::root->setCharForeground(x,y, color() );
	else
		TCODConsole::root->setCharForeground(x,y, TCODColor::darkestGrey );
	TCODConsole::root->setChar( x,y, ch() );
}