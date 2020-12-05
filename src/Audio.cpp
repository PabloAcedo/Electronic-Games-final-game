#include "Audio.h"

#include "extra/bass.h"
#include "includes.h"
#include <cassert>


Audio::Audio(){

	manager = this;

	//init audio
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) //-1 significa usar el por defecto del sistema operativo
	{
		std::cout << "Audio ERROR" << std::endl;
	}
	else {
		std::cout << "Audio OK" << std::endl;
	}

	setVolume(0.9);

}


Audio::~Audio(){

}

//audio samples manager
HSAMPLE Audio::Get(const char* filename) {

	assert(filename);
	//check if loaded
	auto it = AudioSamples.find(filename);
	if (it != AudioSamples.end())
		return it->second;


	//load it
	HSAMPLE sample;
	if (BASS_SampleLoad(false, filename, 0, 0, 3, 0) == 0)
	{
		return NULL;
	}

	sample = BASS_SampleLoad(false, filename, 0, 0, 3, 0);
	AudioSamples[filename] = sample;
	std::cout << "Uploaded audio: " << filename << std::endl;

	return sample;
}

HCHANNEL Audio::GetChannel(HSAMPLE sample) {
	//check if loaded
	auto it = AudioChannels.find(sample);
	if (it != AudioChannels.end())
		return it->second;

	
	if (BASS_SampleGetChannel(sample, false) == 0)
	{
		return NULL;
	}

	HCHANNEL channel = BASS_SampleGetChannel(sample, false);
	AudioChannels[sample] = channel;
	std::cout << "Uploaded channel " << std::endl;
	
	return channel;

}

//play an audio file
void Audio::playSample(const char* filename, bool loop, bool beggining) {
	HSAMPLE sample = Get(filename); //get the audio file

	if (loop) {
		BASS_SAMPLE info;
		BASS_SampleGetInfo(sample, &info);
		info.flags = BASS_SAMPLE_LOOP;
		BASS_SampleSetInfo(sample, &info);
	}

	//create channel
	HCHANNEL hSampleChannel = GetChannel(sample);
	
	//play sample
	BASS_ChannelPlay(hSampleChannel, beggining);
}

void Audio::setVolume(float vol) {
	BASS_SetVolume(vol);
}

void Audio::stopChannel(const char* filename) {
	HSAMPLE sample = Get(filename);
	BASS_ChannelPause(GetChannel(sample));
}
