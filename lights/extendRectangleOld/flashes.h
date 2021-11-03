#include <iostream>
#include <string>
#include <list>

using namespace std;

enum effectType {
	INTENSITY,
	TWINKLE,
	SHIMMER,
	NONE
};

struct channel {
	uint unit;
	uint circuit;
	uint color;
	uint index;
	uint savedIndex;
	uint centiseconds;
	char name[100];
	char deviceType[4];
};

typedef struct channel channel;

struct flash {
	uint start;
	uint end;
	uint startIntensity;
	uint endIntensity;
	effectType etype;
	channel channel;
};

typedef struct flash flash;

// TODO: move color_model to other file and replace all instances here

struct color_model {
	list< channel > model_channels;
	list< channel > all_channels;
	list< list<channel> > other_channels;
	list< flash > model_flashes;
	list< flash > all_flashes;
	int color;
};

typedef struct color_model color_model;

struct flash_group {
	effectType etype;
	uint startIntensity;
	uint endIntensity;
	uint start;
	uint end;
	uint *startIndex;
	uint *endIndex;
	flash *flashes;
	uint size;
};

typedef struct flash_group flash_group;

struct expanded_group {
	uint startIndex;
	uint endIndex;
	uint startIntensity;
	uint endIntensity;
	effectType etype;
	uint size;
	list<uint> times;
};

typedef struct expanded_group expanded_group;

struct lights_signal {
	bool isOn;
	uint time;
	uint index;
	uint intensity;
	effectType etype;
	bool used; 
};

typedef struct lights_signal lights_signal;

string lights_signalToString(lights_signal s);

void printlights_signal(lights_signal s);

string expandedGroupToString(expanded_group eg);

void printExpandedGroup(expanded_group eg);

string effectTypeToString(effectType e);

effectType stringToEffectType(string s);

string channelToString(channel c);

void printChannel(channel c);

void printChannelQuick(channel c);

channel makeChannel(uint unit, uint circuit, uint index);

channel makeChannel(string name, uint color, uint centiseconds, string deviceType, uint unit, uint circuit, uint savedIndex, uint index);

flash makeFlash(string seffectType, uint start, uint end, uint startIntensity, uint endIntensity, channel c);

flash *allocFlash(flash * f_alloc, flash f);

string flashToString(flash f);

void printFlash(flash f);

bool flashIsEmpty(flash f);

string flashGroupToString(flash_group fg);

void printFlashGroup(flash_group fg);

flash_group *allocSingularFlashGroup(flash f, int length);

bool parseChannel(string lms_line, channel *c, uint index);

bool channelsEq(channel c1, channel c2);

bool channelsMatch(list<channel> channels, string lms_line, channel *c, uint index);

list<flash> readRectangle(string filename, color_model m);

bool compareFlashStart(flash f1, flash f2);

bool compareFlashEnd(flash f1, flash f2);

bool flashIsNear(flash_group *flash_group_p, flash *flash_p, uint buffer, bool isStart);

list< flash_group *> findFlashGroups(color_model m, uint buffer, bool isStart);

expanded_group *expandFlashGroup(flash_group fg, int multiple, bool isStart);

expanded_group *shrinkFlashGroup(flash_group fg, int size, bool isStart);

lights_signal makeSignal(int index, int time, bool isOn, uint intensity, effectType etype);

list<lights_signal> expandedGroupsToSignals(list<expanded_group> egs, bool isStart);

bool compareSignals(lights_signal s1, lights_signal s2);

flash signalsToFlash(lights_signal s_on, lights_signal s_off, channel *new_channels);

list<flash> reconstructFlashes(list<expanded_group> eg_starts, list<expanded_group> eg_ends, channel *new_channels);

list<flash> expandFlashes(color_model m, uint multiple, channel *new_channels);

bool reconstructLMS(list<flash> flashes, string outfile_path);

expanded_group *spreadFlashGroup(flash_group fg, int size, bool isStart);