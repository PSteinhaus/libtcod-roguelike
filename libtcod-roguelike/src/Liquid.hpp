class Liquid: public Persistent
{
public:
	enum LiquidType {
		WATER
	} type;
	Liquid(float volume=0, LiquidType type=WATER) : type(type), volume(volume) {};
	virtual ~Liquid() {};
	bool empty() const;
	float decreaseVolume(float amount);
	float increaseVolume(float amount, float maxVolume=std::numeric_limits<float>::infinity() );
	void absorb(Liquid* liquid, float maxVolumeAbsorbed, float maxVolumeReached=std::numeric_limits<float>::infinity() );
	Liquid* split(float amount);
	static Liquid* createByType(LiquidType type, float volume=0);
	static Liquid* create(TCODZip& zip);
	virtual void save(TCODZip& zip);
	virtual void load(TCODZip& zip);
private:
	float volume;
};

class WaterLiquid : public Liquid
{
public:
	WaterLiquid(float volume=0) : Liquid::Liquid(volume,WATER) {};
	virtual ~WaterLiquid() {};
};