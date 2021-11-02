#include <math.h>
#include <regex>
#include <fstream>
#include "flash.h"
#include <QDebug>


string lights_signalToString(lights_signal s) {
    string res = "channel " + to_string(s.index);
    res += s.isOn ? " on  " : " off ";
    res += "at " + to_string(s.time);
    return res;
}

void printlights_signal(lights_signal s) {
    qDebug() << QString::fromStdString(lights_signalToString(s));
}

string expandedGroupToString(expanded_group eg) {
    string res = "expanded_group " + to_string(eg.startIndex) + "-" + to_string(eg.endIndex) + "\n";
    res += "size: " + to_string(eg.size) + "\n";
    uint i = 0;
    for (list<uint>::iterator it = eg.times.begin(); it != eg.times.end(); ++it) {
        if (eg.startIndex > eg.endIndex) {
            res += "\t" + to_string(eg.startIndex - i) + ": " + to_string((*it)) + "\n";
        } else {
            res += "\t" + to_string(eg.startIndex + i) + ": " + to_string((*it)) + "\n";
        }
        i++;
    }
    return res;
}

void printExpandedGroup(expanded_group eg) {
    qDebug() << QString::fromStdString(expandedGroupToString(eg));
}

string effectTypeToString(effectType e) {
    // TODO: Error checking
    switch (e) {
        case INTENSITY:
            return "intensity";
        case TWINKLE:
            return "twinkle";
        case SHIMMER:
            return "shimmer";
        default:
            return "";
    }
    return "";
}

effectType stringToEffectType(string s) {
    if (s.compare("intensity") == 0) {
        return INTENSITY;
    } if (s.compare("twinkle") == 0) {
        return TWINKLE;
    } if (s.compare("shimmer") == 0) {
        return SHIMMER;
    }
    return NONE;
}

flash makeFlash(string seffectType, uint start, uint end, uint startIntensity, uint endIntensity, channel c) {
    flash f;
    f.start = start;
    f.end = end;
    f.startIntensity = startIntensity;
    f.endIntensity = endIntensity;
    f.etype = stringToEffectType(seffectType);
    f.channel = c;
    return f;
}

flash *allocFlash(flash * f_alloc, flash f) {
    if (f_alloc == NULL) {
        f_alloc = (flash *) malloc(sizeof(flash));
    }
    f_alloc-> start = f.start;
    f_alloc->end = f.end;
    f_alloc->startIntensity = f.startIntensity;
    f_alloc->endIntensity = f.endIntensity;
    f_alloc->etype = f.etype;
    f_alloc->channel = f.channel;
    return f_alloc;
}

flash *allocFlash(string seffectType, uint start, uint end, uint startIntensity, uint endIntensity, channel c) {
    flash *f = new flash();
    f->start = start;
    f->end = end;
    f->startIntensity = startIntensity;
    f->endIntensity = endIntensity;
    f->etype = stringToEffectType(seffectType);
    f->channel = c;
    return f;
}

string flashToString(flash f) {
    string s = "";
    s += "\t\t\t<effect type=\"" + effectTypeToString(f.etype) + "\"";
    s += " startCentisecond=\"" + to_string(f.start) + "\" endCentisecond=\"" + to_string(f.end) + "\"";
    if (f.startIntensity != f.endIntensity) {
        s += " startIntensity=\"" + to_string(f.startIntensity) + "\" endIntensity=\"" + to_string(f.endIntensity) + "\"";
    } else {
        s += " intensity=\"" + to_string(f.startIntensity) + "\"";
    }
    s += "/>\n";
    return s;
}

string flashToStringWithChannel(flash f) {
    string s = flashToString(f);
    s += channelToString(f.channel);
    return s;
}

flash *parseFlash(string s) {
    channel c;
    parseChannel(s, &c, 0);
    flash *f;
    regex pattern1 ("<effect type=\"(.*)\" startCentisecond=\"(.*)\" endCentisecond=\"(.*)\" startIntensity=\"(.*)\" endIntensity=\"(.*)\"");
    regex pattern2 ("<effect type=\"(.*)\" startCentisecond=\"(.*)\" endCentisecond=\"(.*)\" intensity=\"(.*)\"");
    smatch match_res;
    if (regex_search(s, match_res, pattern1)) {
        f = allocFlash(match_res[1], stoi(match_res[2]), stoi(match_res[3]), stoi(match_res[4]), stoi(match_res[5]), c);
    } else if (regex_search(s, match_res, pattern2)) {
        f = allocFlash(match_res[1], stoi(match_res[2]), stoi(match_res[3]), stoi(match_res[4]), stoi(match_res[4]), c);
    } else {
        f = nullptr;
    }
//    qDebug() << "parse flash channel " << getCOid(f->channel);
    return f;
}

void printFlash(flash f) {
    qDebug() << QString::fromStdString(flashToString(f));
}

bool flashIsEmpty(flash f) {
    return f.start == 0 && f.end == 0 && f.startIntensity == 0 && f.endIntensity == 0;
}

string flashGroupToString(flash_group fg) {
    int num_flashes = 0;
    string res = "Flash Group start (" + to_string(*fg.startIndex)+ "):"  + to_string(fg.start);
    res += " end(" + to_string(*fg.endIndex) +"): " + to_string(fg.end) + "\n";
    for (uint i=0; i<fg.size; i++) {
        if (!flashIsEmpty(fg.flashes[i])) {
            res += "\t" + to_string(i) + " " + channelToString(fg.flashes[i].channel);
            res += flashToString(fg.flashes[i]) + "\n";
            num_flashes++;
        } else {
            res += "\tNULL\n";
        }
    }
    res += " size: " + to_string(num_flashes) + "\n";
    return res;

}

void printFlashGroup(flash_group fg) {
    qDebug() << &fg.flashes;
    qDebug() << QString::fromStdString(flashGroupToString(fg));
}

flash_group *allocSingularFlashGroup(flash f, int length) {
    flash_group *fg = (flash_group *) malloc(sizeof(flash_group));
    fg->flashes = (flash *) malloc(sizeof(flash) * length);
    fg->start = f.start;
    fg->end = f.end;
    fg->size = length;
    fg->startIntensity = f.startIntensity;
    fg->endIntensity = f.endIntensity;
    fg->etype = f.etype;
    fg->startIndex = (uint*) malloc(sizeof(int));
    *fg->startIndex = f.channel.index;
    fg->endIndex = (uint*)malloc(sizeof(int));
    *fg->endIndex = f.channel.index;
    for (int i=0; i<length; i++) {
        flash *f_i = (flash *) malloc(sizeof(flash));
        if (f_i == NULL) {
            // TODO
        } else {
        }
        f_i->start = 0;
        f_i->end = 0;
        f_i->startIntensity = 0;
        f_i->endIntensity = 0;
        fg->flashes[i] = *f_i;
    }
    qDebug() << "allocating flash group for index" << f.channel.index;
    printChannel(f.channel);
    fg->flashes[f.channel.index] = f;
    return fg;
}

void freeFlashGroup(flash_group *fg) {
    free(fg->startIndex);
    free(fg->endIndex);
    free(fg->flashes);
    free(fg);
}

bool compareFlashStart(flash *f1, flash *f2) {
    if (f1->start < f2->start) {
        return true;
    } if (f1->end > f2->end) {
        return false;
    } return f1->channel.index < f2->channel.index;
}

bool compareFlashEnd(flash *f1, flash *f2) {
    if (f1->end < f2->end) {
        return true;
    } if (f1->end > f2->end) {
        return false;
    } return f1->channel.index < f2->channel.index;
}

bool flashIsNear(flash_group *flash_group_p, flash *flash_p, uint buffer, bool isStart) {
    flash flash = *flash_p;
    flash_group fg = *flash_group_p;
    int first = -1;
    int last = -1;
    struct flash prev;
    uint avg_gap = 0;
    uint num_found = 0;
    uint gap;
    int index = -1;
    for (uint i = 0; i < fg.size; i++) {
        if (!flashIsEmpty(fg.flashes[i])) {
//            qDebug() << fg.flashes[i].start << fg.flashes[i].end << fg.flashes[i].startIntensity << fg.flashes[i].endIntensity;
            index = i;
            if (first == -1) {
                first = fg.flashes[i].channel.index;
            } else {
                gap = isStart ? fg.flashes[i].start - prev.start : fg.flashes[i].end - prev.end;
                avg_gap = ((avg_gap * num_found) + gap) / (num_found + 1);
                prev = fg.flashes[i];
            }
            last = fg.flashes[i].channel.index;
            num_found++;
        }
    }
//     qDebug() << "first: " << first << " last: " << last;
//     qDebug() << " avg_gap: " << avg_gap << " num_found: " << num_found;
//     qDebug() << "index: " << index;
//     qDebug() << "start: " << fg.flashes[index].start << " end: " << fg.flashes[index].end;
//     qDebug() << "flash index: " << flash.channel.index <<  " fir channel " << flash.channel.name;

    if ((int) flash.channel.index == first - 1 && flash.etype == fg.flashes[first].etype) {
        if ((isStart && (int) (avg_gap - buffer) <= abs((int) fg.flashes[first].start - (int) flash.start) <= avg_gap + buffer) ||
            (!isStart && (int) (avg_gap - buffer) <= abs((int) fg.flashes[first].end - (int) flash.end) <= avg_gap + buffer)) {
            flash_group_p->flashes[first-1] = flash;
            *flash_group_p->startIndex = flash.channel.index;
                    // fg->flashes[f.channel.index] = &f;
            return true;
        }
    }
    if ((int) flash.channel.index == last + 1 && flash.etype == fg.flashes[last].etype) {
        if ((isStart && (int) (avg_gap - buffer) <= abs((int) fg.flashes[last].start - (int) flash.start) <= avg_gap + buffer) ||
        (!isStart && (int) (avg_gap - buffer) <= abs((int) fg.flashes[last].end - (int) flash.end) <= avg_gap + buffer)) {
            *flash_group_p->endIndex = flash.channel.index;
            flash_group_p->flashes[last+1] = flash;
            return true;
        }
    }
    return false;
}

list< flash_group *> findFlashGroups(list<flash *> *model_flashes, uint size, uint buffer, bool isStart) {
    if (isStart) {
        model_flashes->sort(compareFlashStart);
    } else {
        model_flashes->sort(compareFlashEnd);
    }
    list< flash_group * > flash_groups;

    for (list<flash *>::iterator it=model_flashes->begin(); it != model_flashes->end(); ++it) {
        if (flash_groups.size() > 0) {
            flash_group *fg_l = flash_groups.back();
            flash_group fg_last = *fg_l;
            // printChannel(fg_last.flashes[0]->channel);
//            qDebug() << QString::fromStdString(flashGroupToString(fg_last));
            bool near = flashIsNear(fg_l, (*it), buffer, isStart);
//            qDebug() << "hi " << near;
//            qDebug() << " size " << size;
//            qDebug() << " buffer " << buffer;
//            qDebug() << " isStart " << isStart;
//            qDebug() << QString::fromStdString(flashToString(*(*it)));
//            qDebug() << (**it).channel.index;
//            qDebug() << QString::fromStdString(channelToString((**it).channel));
            if (near) {
                fg_last.flashes[(**it).channel.index] = (**it);
            } else {
                flash_group *fg = allocSingularFlashGroup(*(*it), size);
                flash_groups.push_back(fg);
            }
        } else {
            flash_group *fg = allocSingularFlashGroup(*(*it), size);
            flash_groups.push_back(fg);
        }
    }
    return flash_groups;
}

expanded_group *expandFlashGroup(flash_group fg, int multiple, bool isStart) {
    expanded_group *eg = new expanded_group();
    eg->startIndex = *fg.startIndex * multiple;
    eg->endIndex = eg->startIndex + (multiple * (1 + abs((int) *fg.startIndex - (int) *fg.endIndex))) - 1;
    eg->size = eg->endIndex - eg->startIndex + 1;
    eg->startIntensity = fg.startIntensity;
    eg->endIntensity = fg.endIntensity;
    eg->etype = fg.etype;
    flash first = fg.flashes[*fg.startIndex];
    flash last = fg.flashes[*fg.endIndex];
    int startTime = isStart ? (int) first.start : (int) first.end;
    int endTime = isStart ? (int) last.start : (int) last.end;
    uint increment = (float) abs(startTime - endTime) /  (float) (eg->size - 1);
    int nextTime = startTime;
    for (uint i=0; i<eg->size; i++) {
        eg->times.push_back(nextTime);
        nextTime = startTime > endTime ? nextTime - increment : nextTime + increment;
    }
    return eg;
}

expanded_group *fitFlashGroup(flash_group fg, int newSize, bool isStart) {
    expanded_group *eg = new expanded_group();
//    qDebug() << "flash group size: " << fg.size << "newSize: " << newSize;
    eg->startIndex = *fg.startIndex * newSize / fg.size;
    eg->endIndex = *fg.endIndex * newSize /fg.size;
    eg->size = eg->endIndex - eg->startIndex + 1;
    eg->startIntensity = fg.startIntensity;
    eg->endIntensity = fg.endIntensity;
    eg->etype = fg.etype;
    flash first = fg.flashes[*fg.startIndex];
    flash last = fg.flashes[*fg.endIndex];
    int startTime = isStart ? (int) first.start : (int) first.end;
    int endTime = isStart ? (int) last.start : (int) last.end;
    uint increment = (float) abs(startTime - endTime) /  (float) eg->size;
    int nextTime = startTime;
    for (uint i=0; i<eg->size; i++) {
        eg->times.push_back(nextTime);
        nextTime = startTime > endTime ? nextTime - increment : nextTime + increment;
    }
    printFlashGroup(fg);
    printExpandedGroup(*eg);
    return eg;
}

//expanded_group *layerFlashGroup(flash_group fg, int newSize, bool isStart) {

//}

expanded_group *shrinkFlashGroup(flash_group fg, int size, bool isStart) {
    expanded_group *eg = expandFlashGroup(fg, 1, isStart);
    int i = eg->endIndex;
    while (i >= size) {
        eg->times.pop_back();
        i--;
    }
    eg->size -= (eg->endIndex - i);
    if (i > 0) {
        eg->endIndex = size - 1;
    }
    return eg;
}

expanded_group *spreadFlashGroup(flash_group fg, int size, bool isStart) {
    int multiple = 1;
    uint tmp_size = size;
    while (tmp_size > fg.size) {
        tmp_size /= 2;
        multiple += 1;
    }
    expanded_group *eg = expandFlashGroup(fg, multiple, isStart);
    int i = eg->endIndex;
    while (i >= size && eg->times.size() > 0) {
        eg->times.pop_back();
        i--;
    }
    eg->size -= (eg->endIndex - i);
    if (i > 0) {
        eg->endIndex = size - 1;
    }
    return eg;
}

lights_signal makeSignal(int index, int time, bool isOn, uint intensity, effectType etype) {
    lights_signal s;
    s.index = index;
    s.time = time;
    s.isOn = isOn;
    s.intensity = intensity;
    s.etype = etype;
    s.used = false;
    return s;
}

list<lights_signal> expandedGroupsToSignals(list<expanded_group> egs, bool isStart) {
    list<lights_signal> lights_signals;
    for (list<expanded_group>::iterator it1 = egs.begin(); it1 != egs.end(); ++it1) {
        expanded_group eg = (*it1);
        int i = 0;
        for (list<uint>::iterator it2 = eg.times.begin(); it2 != eg.times.end(); ++it2) {
            int time = (*it2);
            int index = eg.startIndex > eg.endIndex ? eg.startIndex - i : eg.startIndex + i;
            i++;
            // TODO: intensity and effect type
            uint intensity = isStart ? eg.startIntensity : eg.endIntensity;
            lights_signal s = makeSignal(index, time, isStart, intensity, eg.etype);
            lights_signals.push_back(s);
        }
    }
    return lights_signals;
}

map<int, lights_signal> *makeEndFlashMap(list<flash_group *> fgs, int newSize) {
    map<int, lights_signal> *endMap = new map<int, lights_signal>();
    for (list<flash_group *>::iterator it=fgs.begin(); it!=fgs.end(); ++it) {
        expanded_group *eg = fitFlashGroup(**it, newSize, false);
    }

    return endMap;
}

bool compareSignals(lights_signal s1, lights_signal s2) {
    if (s1.time == s2.time) {
        return s1.index < s2.index;
    }
    return s1.time < s2.time;
}

flash *signalsToFlash(lights_signal s_on, lights_signal s_off, channel *new_channels) {
    return allocFlash(effectTypeToString(s_on.etype), s_on.time, s_off.time, s_on.intensity, s_off.intensity, new_channels[s_on.index]);
}

list<flash *> *reconstructFlashes(list<expanded_group> eg_starts, list<expanded_group> eg_ends, channel *new_channels) {
    list<lights_signal> starts = expandedGroupsToSignals(eg_starts, true);
    list<lights_signal> ends = expandedGroupsToSignals(eg_ends, false);
    starts.sort(compareSignals);
    ends.sort(compareSignals);
    list<flash *> *flashes = new list<flash*>();
    list<lights_signal>::iterator it2;
    lights_signal s_off;
    int i = 0;
    for (list<lights_signal>::iterator it1 = starts.begin(); it1 != starts.end(); ++it1) {
        lights_signal s_on = (*it1);
        it2 = ends.begin();
        advance(it2, i);
        s_off = (*it2);
        if (s_off.time >= s_on.time) {
            while (it2 != ends.begin() && (*it2).time >= s_on.time) {
                it2--;
            }
        }
        bool pastStart = false;
        for ( ; it2 != ends.end(); ++it2) {
            if ((*it2).time >= s_on.time) {
                pastStart = true;
            }
            if (pastStart && !(*it2).used) {
                if ((*it2).index == s_on.index) {
                    flash *f = signalsToFlash(s_on, (*it2), new_channels);
//                    qDebug() << "reconstruct channel " << getCOid(f->channel);
                    flashes->push_back(f);
                    (*it2).used = true;
                    break;
                }
            }
        }
        i++;
    }
    return flashes;
}

list<flash *> *expandFlashes(list<flash *> *model_flashes, uint original_size, uint new_size, channel *new_channels) {
//    qDebug() << "expand flashes original size " << original_size << " new size " << new_size;
//    qDebug() << model_flashes->size();
    list<flash_group *> fg_start = findFlashGroups(model_flashes, original_size, 100, true);
    list<flash_group *> fg_end = findFlashGroups(model_flashes, original_size, 100, false);
    list<expanded_group> eg_starts;
    list<expanded_group> eg_ends;
    for (list<flash_group *>::iterator it = fg_start.begin(); it != fg_start.end(); ++it) {
        flash_group *fg = dynamic_cast<flash_group *>(*it);
//        qDebug() << QString::fromStdString(flashGroupToString(*fg));
        expanded_group *eg = fitFlashGroup(*fg, new_size, true);
//        qDebug() << 5 << QString::fromStdString(flashGroupToString(*fg));
//        qDebug() << 5 << QString::fromStdString(expandedGroupToString(*eg));
        eg_starts.push_back(*eg);
        delete eg;
    }
    for (list<flash_group *>::iterator it = fg_end.begin(); it != fg_end.end(); ++it) {
        expanded_group *eg = fitFlashGroup(*(*it), new_size, false);
        eg_ends.push_back(*eg);
        delete eg;
    }
    list<flash *> *flashes = reconstructFlashes(eg_starts, eg_ends, new_channels);
    for (list<flash_group *>::iterator it=fg_start.begin(); it!=fg_start.end(); ++it) {
        free(*it);
    }
    for (list<flash_group *>::iterator it=fg_end.begin(); it!=fg_end.end(); ++it) {
        free(*it);
    }
    return flashes;
}

bool compareFlash(flash *f1, flash *f2) {
    if (f1->channel.index == f2->channel.index) {
        return f1->start < f2->start;
    }
    return f1->channel.index < f2->channel.index;
}

//bool reconstructLMS(list<flash> flashes, string outfile_path) {
//    ofstream outfile(outfile_path);
//    if (!outfile.is_open()) {
//        return false;
//    }
//    flashes.sort(compareFlash);
//    int flash_index = 0;
//    flash prev;
//    for (list<flash>::iterator it=flashes.begin(); it!=flashes.end(); ++it) {
//        flash f = (*it);
//        if (flash_index < 1) {
//            outfile << channelToString(f.channel);
//        } else if (!channelsEq(prev.channel, f.channel)) {
//            outfile << "\t\t</channel>\n";
//            outfile << channelToString(f.channel);
//        }
//        outfile << flashToString(f);
//        prev = f;
//        flash_index += 1;
//    }
//    outfile << "\t\t</channel>\n";
//    outfile.close();
//    return true;
//}
