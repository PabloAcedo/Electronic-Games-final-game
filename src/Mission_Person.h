#ifndef MISSION_PERSON_H
#define MISSION_PERSON_H


#include "Person.h"
#include "entity.h"
#include "Informant_Person.h"

enum eMissionState : uint8 {INITIAL, INFORMANTS, LETSFINISH, GAMEOVERR, DIENOTLEVEL, GAMEFINISHED};

enum eBuyItem : uint8 { STRENGTH,LIFES};

//this class controls almost everything of the game (almost all the functionalities, even menu functionalities)
class Mission_Person : public Person {
public:

	static Mission_Person* instance;
	int levelcount;
	int informantcount;
	int informantNum;
	Informant_Person* informant;
	eMissionState missionState;

	Mission_Person();
	~Mission_Person();

	void abandonMission();
	void saveGame();
	bool loadGame();
	void buy_things(eBuyItem item);
	void restart_all();
	void to_nextlevel();
	void change_message(const char* &message);
	void move_skeleton();
	void render_extras(Camera* camera, Light* light);
	void behaviour_tree(Vector3 front, double dt);
	void update(double dt);
	

};
#endif
