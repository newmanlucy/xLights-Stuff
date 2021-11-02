#ifndef FLASH_H
#define FLASH_H


#include <list>
#include <string>
#include "channel.h"

using namespace std;

enum effectType {
    INTENSITY,
    TWINKLE,
    SHIMMER,
    NONE
};

struct flash {
    uint start;
    uint end;
    uint startIntensity;
    uint endIntensity;
    effectType etype;
    channel channel;
};

typedef struct flash flash;

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
    list<uint> otherSide;
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

bool compareFlashStart(flash *f1, flash *f2);

bool compareFlashEnd(flash *f1, flash *f2);

bool compareFlash(flash *f1, flash *f2);

bool flashIsNear(flash_group *flash_group_p, flash *flash_p, uint buffer, bool isStart);

expanded_group *expandFlashGroup(flash_group fg, int multiple, bool isStart);

expanded_group *shrinkFlashGroup(flash_group fg, int size, bool isStart);

lights_signal makeSignal(int index, int time, bool isOn, uint intensity, effectType etype);

list<lights_signal> expandedGroupsToSignals(list<expanded_group> egs, bool isStart);

bool compareSignals(lights_signal s1, lights_signal s2);

flash *signalsToFlash(lights_signal s_on, lights_signal s_off, channel *new_channels);

list<flash*> *reconstructFlashes(list<expanded_group> eg_starts, list<expanded_group> eg_ends, channel *new_channels);

bool reconstructLMS(list<flash> flashes, string outfile_path);

list< flash_group *> findFlashGroups(list<flash *> *model_flashes, uint size, uint buffer, bool isStart);

flash *allocFlash(string seffectType, uint start, uint end, uint startIntensity, uint endIntensity, channel c);

flash *parseFlash(string s);

expanded_group *spreadFlashGroup(flash_group fg, int size, bool isStart);

list<flash *> *expandFlashes(list<flash *> *model_flashes, uint original_size, uint new_size, channel *new_channels);

#endif // FLASH_H
