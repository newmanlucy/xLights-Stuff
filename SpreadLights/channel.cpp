#include "channel.h"
#include <regex>
#include <QDebug>

string channelToString(channel c) {
            // <channel name="Unit 1B.05 Rect 1-1 White" color="16777215" centiseconds="28066" deviceType="LOR" unit="27" circuit="5" savedIndex="1658">
    string res = "\t\t<channel name=\"" + string(c.name) + "\"";
    res += " color=\"" + to_string(c.color) + "\"";
    res += " centiseconds=\"" + to_string(c.centiseconds) + "\"";
    res += " deviceType=\"" + string(c.deviceType) + "\"";
    res += " unit=\"" + to_string(c.unit) + "\"";
    res += " circuit=\"" + to_string(c.circuit) + "\"";
    res += " savedIndex=\"" + to_string(c.savedIndex) + "\">\n";
    return res;
}

void printChannel(channel c) {
    qDebug() << QString::fromStdString(channelToString(c)).toUtf8();
}

void printChannelQuick(channel c) {
    string channel = to_string(c.unit) + "." + to_string(c.circuit) + " " + to_string(c.index);
    qDebug() << QString::fromStdString(channel).toUtf8();
}

channel makeChannel(uint unit, uint circuit, uint index) {
    channel c;
    c.unit = unit;
    c.circuit = circuit;
    c.index = index;
    string name = "Unit " + to_string(unit) + "." + to_string(circuit);
    strcpy(c.name, name.c_str());
    return c;
}

channel makeChannel(string name, uint color, uint centiseconds, string deviceType, uint unit, uint circuit, uint savedIndex, uint index) {
    channel c;
    strcpy(c.name, name.c_str());
    c.color = color;
    c.centiseconds = centiseconds;
    strcpy(c.deviceType, deviceType.c_str());
    c.unit = unit;
    c.circuit = circuit;
    c.savedIndex = savedIndex;
    c.index = index;
    return c;
}

void copyChannel(channel *c_src, channel *c_dst) {
    c_dst->color = c_src->color;
    c_dst->unit = c_src->unit;
    c_dst->circuit = c_src->circuit;
    c_dst->centiseconds = c_src->centiseconds;
    c_dst->savedIndex = c_src->savedIndex;
    c_dst->index = c_src->index;
//    qDebug() << "here 3";
    strcpy(c_dst->name, c_src->name);
//    qDebug() << c_dst->name;
    strcpy(c_dst->deviceType, c_src->deviceType);
//    qDebug() << c_dst->deviceType;
}

channel *allocChannel(channel c) {
    channel *cNew = new channel();
    cNew->color = c.color;
    cNew->centiseconds = c.centiseconds;
    cNew->unit = c.unit;
    cNew->circuit = c.circuit;
    cNew->savedIndex = c.savedIndex;
    cNew->index = c.index;
    strcpy(cNew->name, c.name);
    strcpy(cNew->deviceType, c.deviceType);
    return cNew;
}

channel *allocChannel(uint unit, uint circuit, uint index) {
    channel *c = new channel();
    c->unit = unit;
    c->circuit = circuit;
    c->index = index;
    string name = "Unit " + to_string(unit) + "." + to_string(circuit);
    strcpy(c->name, name.c_str());
    return c;
}

channel *allocChannel(string name, uint color, uint centiseconds, string deviceType, uint unit, uint circuit, uint savedIndex, uint index) {
    channel *c = new channel();
    strcpy(c->name, name.c_str());
    c->color = color;
    c->centiseconds = centiseconds;
    strcpy(c->deviceType, deviceType.c_str());
    c->unit = unit;
    c->circuit = circuit;
    c->savedIndex = savedIndex;
    c->index = index;
    return c;
}


bool parseChannel(string lms_line, channel *c, uint index) {
    regex pattern("<channel name=\"(.*)\" color=\"(.*)\" centiseconds=\"(.*)\" deviceType=\"(.*)\" unit=\"(.*)\" circuit=\"(.*)\" savedIndex=\"(.*)\">");
//    qDebug() << QString::fromStdString(lms_line);
    regex name_pattern("Unit [0-9A-F]+\\.[0-9]+[ \t]+(.+)");
//    regex name_pattern("Unit [0-9A-F]+\.[0-9]+[ \t]+(.+)");
    smatch match_res;
    smatch name_match_res;
    if (regex_search(lms_line, match_res, pattern)) {
        string name = match_res[1];
        if (regex_search(name, name_match_res, name_pattern)) {
            name = name_match_res[1];
        }
//        qDebug() << QString::fromStdString(name);
        uint color = stoi(match_res[2]);
        uint centiseconds = stoi(match_res[3]);
        string deviceType = match_res[4];
        uint unit = stoi(match_res[5]);
        uint circuit = stoi(match_res[6]);
        uint savedIndex = stoi(match_res[7]);
        strcpy(c->name, name.c_str());
        c->color = color;
        c->centiseconds = centiseconds;
        strcpy(c->deviceType, deviceType.c_str());
        c->unit = unit;
        c->circuit = circuit;
        c->savedIndex = savedIndex;
        c->index = index;
        if (unit  == 27 && circuit == 5) {
            qDebug() << QString::fromStdString(channelToString(*c));
        }
        return true;
    }
    return false;
}

bool channelsEq(channel c1, channel c2) {
    return c1.unit == c2.unit && c1.circuit == c2.circuit;
}

bool compareChannels(channel c1, channel c2) {
    return c1.unit < c2.unit || (c1.unit == c2.unit && c1.circuit < c2.circuit);
}

bool channelsMatch(list<channel> channels, string lms_line, channel *c, uint index) {
    if (!parseChannel(lms_line, c, index)) {
        return false;
    }
    for (std::list<channel>::iterator it=channels.begin(); it != channels.end(); ++it) {
        if (channelsEq((*it), *c)) {
            return true;
        }
    }
    return false;

}

uint getCOid(channel c) {
    return 24 * c.unit + c.circuit;
}

