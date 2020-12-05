#ifndef AUDIO_H
#define AUDIO_H

#include "extra/bass.h"
#include <map>
#include <string>

class Audio{
public:

	static Audio* manager; //audio manager

	//sample manager
	std::map<std::string, HSAMPLE> AudioSamples;

	//channels manager
	std::map<HSAMPLE, HCHANNEL> AudioChannels;

	Audio();
	~Audio();

	HSAMPLE Get(const char* filename); //get sample
	HCHANNEL GetChannel(HSAMPLE sample);
	void setVolume(float vol);
	void stopChannel(const char* filename);
	void playSample(const char* filename, bool loop, bool beggining);//play the sample

};
#endif
