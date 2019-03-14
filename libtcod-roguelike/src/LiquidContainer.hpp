class LiquidContainer : public Persistent
{
public:
	LiquidContainer(float maxVolume);
	virtual ~LiquidContainer();
	Liquid* fill(Liquid* newLiquid);
	Liquid* drain(float amount);
	void save(TCODZip& zip);
	void load(TCODZip& zip);
private:
	float maxVolume;
	Liquid* liquid;
};