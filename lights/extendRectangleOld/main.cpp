#include "flashes.h"

int main() {
	color_model m;
	string line = "<channel name=\"Unit 19.06     Conn 6-2 Bl/Gr\" color=\"4227072\" centiseconds=\"28066\" deviceType=\"LOR\" unit=\"27\" circuit=\"5\" savedIndex=\"1655\"/>";
	channel c;
	uint unit = 27;
	list<channel> channels;
	for (int i=0; i<8; i++) {
		c = makeChannel("channel y", 128, 20000, "LOR", unit, 5 + i, i, i);
		channels.push_back(c);
	}
	m.model_channels = channels;

	list<flash> flashes = readRectangle("sample.xml", m);

	m.model_flashes = flashes;
	list<flash_group *> fgs = findFlashGroups(m, 10, true);
	channel *new_channels = (channel *) malloc(sizeof(channel) * 16);
	for (int i=0; i<16; i++) {
		channel c = makeChannel("channel x", 123, 10000, "LOR", 0, i, i, i);
		new_channels[i] = c;
	}
	list<flash> exp_flashes = expandFlashes(m, 7, new_channels);
	reconstructLMS(exp_flashes, "out.txt");

	return 0;
}