#include "asprintf.h"
#include "main.hpp"
#include <stdarg.h>
#include <stdlib.h>

static const int PANEL_HEIGHT = 7;
static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH+2;
static const int MSG_HEIGHT = PANEL_HEIGHT-1;
static const int MAX_LOG_SIZE = 30;

Gui::Gui() {
	con = new TCODConsole(engine.screenWidth, PANEL_HEIGHT);
}

Gui::~Gui() {
	delete con;
	clear();
}

void Gui::clear() {
	log.clearAndDelete();
}

void Gui::render() {
	// clear the GUI console
	con->setDefaultBackground(TCODColor::black);
	con->clear();
	// draw the health bar
	renderBar(1,1,BAR_WIDTH,"HP",engine.player->destructible->hp,
		engine.player->destructible->maxHp, TCODColor::lightRed, TCODColor::darkerRed);
	// draw the message log
	int y = 1;
	for (Message** it=log.begin(); it != log.end() && y < PANEL_HEIGHT ; it++) {
		Message* message = *it;
		con->setDefaultForeground(message->col);
		y += con->printRect(MSG_X, y, engine.screenWidth-MSG_X, PANEL_HEIGHT-1, message->text);
	}
	// blit the GUI console on the root console
	TCODConsole::blit(con,0,0,engine.screenWidth,PANEL_HEIGHT,
		TCODConsole::root,0,engine.screenHeight-PANEL_HEIGHT);
}

void Gui::renderBar(int x, int y, int width, const char* name, float value,
	float maxValue, const TCODColor& barColor, const TCODColor& backColor) {
	// fill the background
	con->setDefaultBackground(backColor);
	con->rect(x,y,width,1,false,TCOD_BKGND_SET);
	int barWidth = (int)(value / maxValue * width);
	if ( barWidth > 0 ) {
		// draw the bar
		con->setDefaultBackground(barColor);
		con->rect(x,y,barWidth,1,false,TCOD_BKGND_SET);
	}
	// print text on top of the bar
	con->setDefaultForeground(TCODColor::white);
	con->setAlignment(TCOD_CENTER);
	con->printf(x+width/2,y, "%s : %g/%g",name,value,maxValue);
	con->setAlignment(TCOD_LEFT);
}

Gui::Message::Message(char* text, const TCODColor& col) : text(strdup(text)), col(col) {
	/*
	this->text = new char[strlen(text)];
	strcpy(this->text, text);
	*/
}

Gui::Message::~Message() {
	free(text);
}

void Gui::message(const TCODColor& col, const char* text, ...) {
	// make sure there's space for a new message
	if ( log.size() == MAX_LOG_SIZE ) {
		Message *toRemove = log.pop(); // last element
		delete toRemove;
	}
	// add a new message to the log
	va_list args;
	va_start(args, text);
		// format text
	char* msgText = NULL;
	vasprintf(&msgText, text, args);
	va_end(args);
	if (msgText != NULL) {
		log.insertBefore( new Message(msgText, col), 0 );
		free(msgText);
	}
}

Menu::~Menu() {
	clear();
}

void Menu::clear() {
	items.clearAndDelete();
}

void Menu::addItem(MenuItemCode code, const char* label) {
	MenuItem* item = new MenuItem();
	item->code = code;
	item->label = label;
	items.push(item);
}

Menu::MenuItemCode Menu::pick() {
	int selectedItem = 0;
	while ( !TCODConsole::isWindowClosed() ) {
		int currentItem=0;
		for (MenuItem **it=items.begin(); it!=items.end(); it++) {
			if ( currentItem == selectedItem ) {
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			} else {
				TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->printf(10,10+currentItem*3,(*it)->label);
			currentItem++;
		}
		TCODConsole::flush();
		// check key presses
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL);
		switch(key.vk) {
			case TCODK_UP :
				selectedItem--;
				if (selectedItem < 0) {
					selectedItem = items.size()-1;
				}
			break;
			case TCODK_DOWN :
				selectedItem = ( selectedItem+1 ) % items.size();
			break;
			case TCODK_ENTER : return items.get(selectedItem)->code;
			default : break;
		}
	}
	return NONE;
}