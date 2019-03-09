class TargetSelector : public Persistent
{
public :
	enum SelectorType {
		CLOSEST_MONSTER,
		SELECTED_MONSTER,
		USER_RANGE,
		SELECTED_RANGE,
		EFFECT_CARRIER,
		NUMPAD
	} selectorType;
	bool aliveRequired;
	bool singleOnly;
	TargetSelector(bool targetActors=true, bool targetTiles=true);
	virtual ~TargetSelector() {};
	virtual void selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly=false ) = 0;
	virtual void save(TCODZip& zip);
	static TargetSelector* create(TCODZip& zip);
	virtual void load(TCODZip& zip) = 0;
	
protected :
	void addTargets(int x, int y, TCODList<Actor*>& list, TCODList<Tile*>& tileList);
	bool targetActors;
	bool targetTiles;
};

class SelectClosestMonster : public TargetSelector
{
public:
	SelectClosestMonster() = default;
	SelectClosestMonster(float range, bool targetActors=true, bool targetTiles=true);
	void selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly=false );
	void save(TCODZip& zip);
	void load(TCODZip& zip);
private:
	float range;
};

class SelectField : public TargetSelector
{
public:
	SelectField() = default;
	SelectField(float range, bool targetActors=true, bool targetTiles=true);
	void selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly=false );
	void save(TCODZip& zip);
	void load(TCODZip& zip);
private:
	float range;
};

class RangeAroundUser : public TargetSelector
{
public:
	RangeAroundUser() = default;
	RangeAroundUser(float range, bool includeUser, bool targetActors=true, bool targetTiles=true);
	void selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly=false );
	void save(TCODZip& zip);
	void load(TCODZip& zip);
private:
	float range;
	bool includeUser;
};

class SelectFieldRange : public TargetSelector
{
public:
	SelectFieldRange() = default;
	SelectFieldRange(float range, float areaRange, bool targetActors=true, bool targetTiles=true);
	void selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly=false );
	void save(TCODZip& zip);
	void load(TCODZip& zip);
private:
	float range;
	float areaRange;
};

class SelectEffectCarrier : public TargetSelector
{
public:
	SelectEffectCarrier(bool targetActors=true, bool targetTiles=true);
	void selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly=false );
	void save(TCODZip& zip);
	void load(TCODZip& zip);
};

class SelectNumpadAdjecent : public TargetSelector
{
public:
	SelectNumpadAdjecent() = default;
	SelectNumpadAdjecent( bool acceptCenter, bool targetActors=true, bool targetTiles=true);
	void selectTargets(Actor *user, Actor *owner, TCODList<Actor*>& list, TCODList<Tile*>& tileList, bool implicitly=false );
	void save(TCODZip& zip);
	void load(TCODZip& zip);
private:
	bool acceptCenter;
};