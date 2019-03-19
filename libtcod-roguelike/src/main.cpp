#include "main.hpp"

Engine engine(110,65);

int main() {
	engine.gameMenu();
	while ( !TCODConsole::isWindowClosed() && engine.gameStatus!=Engine::EXIT ) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	engine.save();
	TCOD_quit();
	return 0;
}