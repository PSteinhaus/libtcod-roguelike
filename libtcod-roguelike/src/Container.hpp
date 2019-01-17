class Container : public Persistent
{
public:
	int size; // maximum number of actors. 0 = unlimited
	float maxVolume;
	TCODList<Actor*> inventory;

	Container(int size, float maxVolume = 0);
	virtual ~Container();
	bool add(Actor* actor);
	void remove(Actor* actor);
	void load(TCODZip& zip);
	void save(TCODZip& zip);
	float getVolume();
};

// contains actors to be digested
class Stomach : public Container
{
public:
	float nutrition;
	float maxNutrition;
	float need;
	float digestionRate;

	Stomach(float maxNutrition, float need, float digestionRate, int size, float maxVolume=0, float nutrition=-1);
	void digest(Actor* owner);
	void load(TCODZip& zip);
	void save(TCODZip& zip);
};