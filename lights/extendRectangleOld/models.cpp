#include "flashes.h"
#include "models.h"

struct model {
	string name;
	list<channel> leader;
	list< list<channel> > followers;
};

typedef struct model model;

model makeModel(string name, list<channel> leader, list< list<channel> > followers) {
	model m;
	m.name = name;
	m.leader = leader;
	m.followers = followers;
	return m;
}

string modelToString(model m) {
	string res = "Model " + m.name + "\nLeader:\n";
	for (list<channel>::iterator it = m.leader.begin(); it != m.leader.end(); ++it) {
		res += channelToString(*it);
	}
	int i = 0;
	for (list< list<channel> >::iterator it1 = m.followers.begin(); it1 != m.followers.end(); ++it1) {
		list<channel> follower = *it1;
		res += "Follower " + to_string(i) + ":\n";
		for (list<channel>::iterator it2 = follower.begin(); it2 != follower.end(); ++it2) {
			res += channelToString(*it2);
		}
	}
	return res;
}

void printModel(model m) {
	cout << modelToString(m);
}

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
	list<channel> new_channels;
	for (int i=0; i<16; i++) {
		channel c = makeChannel("channel x", 123, 10000, "LOR", 0, i, i, i);
		new_channels.push_back(c);
	}
	list< list<channel> > followers;
	followers.push_back(new_channels);

	model mod = makeModel("White", channels, followers);
	printModel(mod);

	return 0;
}