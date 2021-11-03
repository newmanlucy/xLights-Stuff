import csv
import re
import numpy as np
from np.linalg import inv

def print_channel(row):
    name = row[0]
    color = row[1]
    deviceType = row[2]
    unit = row[3]
    circuit = row[4]
    savedIndex = row[5]
    print('<channel name="%s" color="%s" deviceType="%s" unit="%s" circuit="%s" savedIndex="%s" />' % (name, color, deviceType, unit, circuit, savedIndex))

def print_rgb_channel(row):
    name = row[0][:-4]
    deviceType = row[2]
    unit = row[3]
    circuit = int(row[4])
    savedIndex = int(row[5])
    print('<channel name="%s (R)" color="255" deviceType="%s" unit="%s" circuit="%s" savedIndex="%d" />' % (name, deviceType, unit, circuit, savedIndex))
    print('<channel name="%s (G)" color="65280" deviceType="%s" unit="%s" circuit="%s" savedIndex="%d" />' % (name, deviceType, unit, circuit+1, savedIndex+1))
    print('<channel name="%s (B)" color="16711680" deviceType="%s" unit="%s" circuit="%s" savedIndex="%d" />' % (name, deviceType, unit, circuit+2, savedIndex+2))
    print("""
        <rgbChannel name="%s" savedIndex="%d">
            <channels>
                <channel savedIndex="%d" />
                <channel savedIndex="%d"/>
                <channel savedIndex="%d"/>
            </channels>
        </rgbChannel>
        """ % (name, savedIndex+3, savedIndex, savedIndex+1, savedIndex+2))

def write_lcc():
    with open('channel_config.csv') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        skip = 0
        for row in reader:
            if skip > 0:
                skip -= 1
                continue
            if len(row[0]) > 3 and row[0][-3:] == "(R)":
                print_rgb_channel(row)
                skip = 2
            else:
                print_channel(row)

def make_channel_dict(value):
    pattern = '<channel name="(.*)" color="(.*)" deviceType="LOR" unit="(.*)" circuit="(.*)" savedIndex="(.*)" \/>'
    d = {}
    with open("channels.xml") as f:
        for line in f:
            m = re.match(pattern, line)
            if m is not None:
                channel = convert_channel(int(m.group(3)), int(m.group(4)))
                d[channel] = int(m.group(value))
    return d

def make_index_dict():
    chan_dict = make_channel_dict(5)
    pattern = '(.*)<channel name="(.*)" color="(.*)" deviceType="LOR" unit="(.*)" circuit="(.*)" savedIndex="(.*)"\/>'
    d = {}
    with open("2020 New.lcc") as f:
        for line in f:
            m = re.match(pattern, line)
            if m is not None:
                channel = convert_channel(int(m.group(4)), int(m.group(5)))
                if channel in chan_dict:
                    d[int(m.group(6))] = chan_dict[channel]
                    # print(chan_dict[channel] + "\t Ch %s:%s" % (m.group(1), m.group(2)))
                else:
                    # print("channel %d not found" % channel)
                    pass
    return d

def add_devices():
    pattern = '\t\t<channel name="Ch (.*): (.*)" color="(.*)" savedIndex="(.*)"\/>'
    with open("LOR channels xlights exported.lcc") as f:
        for line in f:
            m = re.match(pattern,line)
            if m is None:
                print(line)
            else:
                unit, circuit = get_controller_and_circuit(int(m.group(1)))
                print('\t\t<channel name="Ch %s: %s" color="%s" deviceType="LOR" unit="%d" circuit="%d" savedIndex="%s"/>' % (m.group(1), m.group(2), m.group(3), unit, circuit, m.group(4)))

def print_tracks():
    print("""
<tracks>
    <track>
        <channels>
        """)
    index_dict = make_index_dict()
    for k in index_dict:
        print('\t\t\t<channel savedIndex="%d"/>' % index_dict[k])
    print("""
        </channels>
    </track>
</tracks>
    """)

def print_animation():
    begin = 0
    index_dict = make_index_dict()
    pattern = '(.*)<column index="(.*)" channel="(.*)"\/>'
    with open("2020 New.lcc") as f:
        for line in f:
            if not begin:
                if "<animation" in line:
                    begin = True
                else:
                    continue
            m = re.match(pattern, line)
            if m is None:
                print(line)
                continue
            if int(m.group(3)) not in index_dict:
                pass
                # print("index %s not found" % m.group(3))
            else:
                print('%s<column index="%s" channel="%d"/>' % (m.group(1), m.group(2), index_dict[int(m.group(3))]))


def match_channels():
    chan_dict = make_channel_dict(1)
    pattern = '.*<channel name="Ch (.*):(.*)".*'
    with open("LOR channels xlights.lcc") as f:
        for line in f:
            if " Net " in line:
                continue
            m = re.match(pattern, line)
            if m is not None:
                channel = int(m.group(1))
                if channel in chan_dict:
                    print(chan_dict[channel] + "\t Ch %s:%s" % (m.group(1), m.group(2)))
                else:
                    print("channel %d not found" % channel)

def convert_channel(controller, offset):
    if controller <= 50:
        return (controller - 1) * 16 + offset
    elif controller <= 59:
        return (16 * 49) + (controller - 50) * 24 + offset
    elif controller <= 64:
        return (16 * 49) + (24 * 9) + (controller - 59) * 16 + offset
    else:
        return (16 * 49)  + (24 * 9) + (16 * 5) + (controller - 64) * 24 + offset

def get_circuit(channel, num_chan):
    if channel % num_chan == num_chan:
        return num_chan
    return channel % num_chan

def get_controller_and_circuit(channel):
    if channel <= 49 * 16:
        circuit = channel % 16
        return (1 + channel / 16, get_circuit(channel, 16))
    channel -= 49 * 16
    if channel <= 9 * 24:
        return (50 + channel / 24, get_circuit(channel, 24))
    channel -= 24 * 9
    if channel <= 5 * 16:
        return (59 + channel / 16, get_circuit(channel, 16))
    channel -= 16 * 5
    return (64 + channel / 24, get_circuit(channel, 24))

def name_to_color(name):
    if "red" in name.lower():
        return "255"
    if "green" in name.lower():
        return "65280"
    if "blue" in name.lower():
        return "16711680"
    if "white" in name.lower():
        return "16777215"
    if "gold" in name.lower() or "north rail" in name.lower() in "south rail" in name.lower():
        return "65535"
    if "purple" in name.lower():
        return "16711808"
    if "pink" in name.lower():
        return "16744703"
    return "16777215"

def fix_file():
    pattern = '\s*<channel name="(.*)" color="(.*)" savedIndex="(.*)"\/>'
    with open("LOR channels xlights.lcc") as f:
        for line in f:
            m = re.match(pattern, line)
            if m is not None and m.group(2) == "16777215":
                print('<channel name="%s" color="%s" savedIndex="%s" />' % (m.group(1), name_to_color(m.group(1)), m.group(3)))
            else:
                print(line)

def make_ac_dict1():
    pattern = '.*<channel .* unit="(.*)" circuit="(.*)" savedIndex="(.*)".*>'
    d = {}
    prev_line = None
    with open("2020 New.lcc") as f:
        for line in f:
            m = re.match(pattern, line)
            if m is not None:
                channel = convert_channel(int(m.group(1)), int(m.group(2)))
                d[int(m.group(3))] = channel 
    return d

def make_ac_dict2():
    pattern = '.*<channel name="Ch (.*):.* savedIndex="(.*)".*'
    d = {}
    with open("devices_added.lcc") as f:
        for line in f:
            m = re.match(pattern, line)
            if m is not None:
                d[int(m.group(1))] = int(m.group(2))
    return d

def make_rgb_dict1():
    with open("2020 New.lcc") as f:
        d1 = {}
        d2 = {}
        prev_line = None
        pattern = '.*<rgbChannel name=".*" savedIndex="(.*)">'
        prev_pattern = '.*unit="(.*)" circuit="([0-9]+)".*'
        for line in f:
            m1 = re.match(pattern, line)
            if m1 is not None:
                m2 = re.match(prev_pattern, prev_line)
                if m2 is not None:
                    d1[int(m1.group(1))] = convert_channel(int(m2.group(1)), int(m2.group(2)))
                    d2[convert_channel(int(m2.group(1)), int(m2.group(2)))] = int(m1.group(1))
            prev_line = line
        return d1, d2

def make_rgb_dict2():
    d = {}
    with open("devices_added.lcc") as f:
        prev_line = None
        pattern = '.*<rgbChannel name=".*" savedIndex="(.*)">'
        prev_pattern = '.*name="Ch (.*):.*'
        for line in f:
            m1 = re.match(pattern, line)
            if m1 is not None:
                m2 = re.match(prev_pattern, prev_line)
                if m2 is not None:
                    d[int(m2.group(1))] = int(m1.group(1))
            prev_line = line
    return d

def fix_animation():
    begin = False
    d1, d2 = make_rgb_dict1()
    d3 = make_rgb_dict2()
    d4 = make_ac_dict1()
    d5 = make_ac_dict2()
    pattern = '(.*)<column index="(.*)" channel="(.*)"\/>'
    with open("2020 New.lcc") as f:
        for line in f:
            if "</animation" in line:
                begin = False
            if not begin:
                if "<animation" in line:
                    begin = True
                else:
                    continue
            m = re.match(pattern, line)
            if m is None:
                print(line)
                continue
            index = int(m.group(3))
            # if index in d1 and d1[index] in d3:
            #     print('%s<column index="%s" channel="%d"/>' % (m.group(1), m.group(2), d3[d1[index]]))
            # elif index in d4 and d4[index] in d5:
            #     print('%s<column index="%s" channel="%d"/>' % (m.group(1), m.group(2), d5[d4[index]]))
            # else:
            #     print(line)

            if index not in d1 and index not in d4:
                continue
                # print("1 index %d not found" % index)
            elif index in d1 and d1[index] not in d3:
                pass
                # print("2 RGB channel %d not found" % d1[index])
            elif index in d4 and d4[index] not in d5:
                # print("3 AC channel %d not found" % d4[index])
                pass
            elif index in d1:
                print('%s<column index="%s" channel="%d"/>' % (m.group(1), m.group(2), d3[d1[index]])) 
                # pass
                # print("4 %d\t%d\t%d" % (index, d1[index], d3[d1[index]]))
            elif index in d4:
                # print("5 %d\t%d\t%d" % (index, d4[index], d5[d4[index]]))
                print('%s<column index="%s" channel="%d"/>' % (m.group(1), m.group(2), d5[d4[index]]))


if __name__ == '__main__':
    fix_animation()

