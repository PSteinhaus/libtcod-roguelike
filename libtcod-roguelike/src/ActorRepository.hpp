// for simple access and documentation of items, monsters, etc.

namespace ActorRep {
	enum Name {
		NONE, RAT, ORC, TROLL, DOWNSTAIRS, UPSTAIRS, DOOR, AXE, WOOD
	};

	Actor* getActor(Name name, int x, int y);

	/*
	Actor* Rat(int x, int y);
	Actor* Orc(int x, int y);
	Actor* Troll(int x, int y);

	Actor* HealthPotion(int x, int y);
	Actor* LightningScroll(int x, int y);
	Actor* FireballScroll(int x, int y);
	Actor* ConfusionScroll(int x, int y);
	*/
}