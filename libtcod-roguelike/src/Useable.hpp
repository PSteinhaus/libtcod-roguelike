class Useable : public Persistent
{
public:
	Useable(TargetSelector *selector, Effect *effect, bool destroyWhenEmpty=true);
	virtual ~Useable();
	bool use(Actor* owner, Actor* user, bool implicitly=false);
	void save(TCODZip& zip);
	void load(TCODZip& zip);

protected:
	TargetSelector *selector;
	Effect *effect;
	bool destroyWhenEmpty;
};