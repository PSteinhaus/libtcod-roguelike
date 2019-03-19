class Camera
{
public:
	Camera(int x, int y, int width, int height);
	~Camera();
	void setToPos(int x, int y);
	void followActor(Actor* actor);
	void render();
	bool isOnCamera(int x0, int y0) const;

private:
	int x;
	int y;
	int width;
	int height;
	TCODConsole* con;
	int mapX;
	int mapY;
};