class Persistent {
public:
	virtual void load(TCODZip &zip) = 0;
	virtual void save(TCODZip &zip) = 0;
};

class BitArray {
public:
	BitArray(TCODZip* zipPtr = NULL); // standard is write-mode
	void save(TCODZip& zip, bool value);
	bool load(TCODZip& zip);
	void finish(TCODZip& zip); // make sure the last bitArray gets saved too

private:
	char array;
	int bitNum;
};