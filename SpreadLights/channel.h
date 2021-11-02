#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>
#include <string>

using namespace std;

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

string channelToString(channel c);

void printChannel(channel c);

void printChannelQuick(channel c);

channel makeChannel(uint unit, uint circuit, uint index);

channel makeChannel(string name, uint color, uint centiseconds, string deviceType, uint unit, uint circuit, uint savedIndex, uint index);

channel *allocChannel(uint unit, uint circuit, uint index);

channel *allocChannel(string name, uint color, uint centiseconds, string deviceType, uint unit, uint circuit, uint savedIndex, uint index);

bool parseChannel(string lms_line, channel *c, uint index);

bool channelsEq(channel c1, channel c2);

bool compareChannels(channel c1, channel c2);

bool channelsMatch(list<channel> channels, string lms_line, channel *c, uint index);

channel *allocChannel(channel c);

void copyChannel(channel *c_src, channel *c_dst);

uint getCOid(channel c);


#endif // CHANNEL_H
