#include "LevelCreator.h"
#include "Target_Person.h"
#include "texture.h"
#include "levelmap.h"



LevelCreator::LevelCreator(){
}

LevelCreator::LevelCreator(const char* level_map_name) {
	level = new levelmap(256, 256, level_map_name, true);
}


LevelCreator::~LevelCreator(){

}

