#ifndef LEVELMAP_H
#define LEVELMAP_H


#include "utils.h"



enum eCellType : uint8 {
	EMPTY, HOUSE, FLOOR, START, COIN, SUN, ENEMY, WALL, WALL_VERT, WALL_ENTRANCE, PINE, SAKURA, HOUSE_OLD, 
	FOOD, MISSION, INFORMANT, TARGET, MOUNTAINH, MOUNTAINV, CORNER1, CORNER2, CORNER3, CORNER4, LIGHT
};

struct sCell {
	eCellType type;
	bool printable;
};

struct sMapHeader {
	int w; //width of map
	int h; //height of map
	unsigned char bytes; //num bytes per cell
	unsigned char extra[7]; //filling bytes, not used
};



class levelmap
{
public:
	sCell* data;
	int w;
	int h;
	sMapHeader header;
	float dist;
	bool temporal_entities;
	
	//constructor 
	levelmap(int w, int h, const char* level_name, bool temporal);
	levelmap(int w, int h);
	levelmap();
	~levelmap();

	//methods
	sCell& getCell(int x, int y);

	Vector3 set_start_position(eCellType type);

	void setCell(int x, int y, eCellType type);

	bool loadMap(const char* filename);

	void createMap();

	//void createLevel(const char* filename);

	void createLevel();
	
	void set_items();

	void Init(int nc, int cs);

	static levelmap* Get(const char* filename); //maps manager


};

#endif
