class Digestor : public Persistent
{
public:
	Digestor(float nutrition);
	virtual ~Digestor() = default;
	virtual void operator()(Actor* owner, Actor* eater);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

private:
	float nutrition; // how much nutrition an actor with a stomach could get out of this ( 1 = 1kcal )
};