#ifndef STAGE_H
#define STAGE_H

#include "framework.h"
#include <map>



enum eMode : int {
	PLAY, LOAD, CONTROLS, QUIT, MAIN, PAUSE, INTRO, MENU, GAMEOVER, WINNER, GAMESTATE, HISTORY, DEAD_PLAYER, INFO,
	SAVE, BUY, RESTART, BUY_STRENGTH, BUY_LIFE, ABANDON_MISSION
};

class Stage
{
public:
	//constructor
	Stage(const char* stage_type);
	Stage();
	eMode mode;

	//hat control time
	float hat_time;

	//menus things
	const char* buttons_text;
	//coordinates for textures
	int sprite_coord[6];
	int button_coord[6];
	bool options[6];
	//button sizes
	int but_sizex; int but_sizey;
	//events
	int event_control;

	static std::map<std::string, Stage*> s_stages;
	static Stage* currentStage;

	const char* message;

	
	bool input_plus_time(int pad_button);
	void change_event(int event_num);
	void update_hat_time(double seconds_elapsed);
	void Select_in_menu();
	static void changeStage(const char* stage_type);
	virtual void render();
	virtual void update(double seconds_elapsed);
	

};
#endif
