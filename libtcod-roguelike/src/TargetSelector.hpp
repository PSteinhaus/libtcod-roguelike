class TargetSelector
{
public :
	enum SelectorType {
		CLOSEST_MONSTER,
		SELECTED_MONSTER,
		USER_RANGE,
		SELECTED_RANGE,
		EFFECT_CARRIER
	};
	TargetSelector(SelectorType type, float range, float areaRange=0);
	void selectTargets(Actor *user, Actor *owner, TCODList<Actor *> & list);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
	
protected :
	SelectorType type;
	float range;
	float areaRange;
};