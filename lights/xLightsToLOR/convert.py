import xml.etree.ElementTree as ET
import re
import ntpath

def getChannelFromName(name):
    pattern = "Ch ([0-9]+).*"
    m = re.match(pattern, name)
    return int(m.group(1))

def convertChannel(controller, offset):
    if controller <= 50:
        return (controller - 1) * 16 + offset
    elif controller <= 59:
        return (16 * 49) + (controller - 50) * 24 + offset
    elif controller <= 64:
        return (16 * 49) + (24 * 9) + (controller - 59) * 16 + offset
    else:
        return (16 * 49)  + (24 * 9) + (16 * 5) + (controller - 64) * 24 + offset

def parseLMSfromXLights(filename):
    root = ET.parse(filename).getroot()
    channelEffectDict = {}
    for channel in root.findall('channels/channel'):
        channelName = channel.get("name")
        channelNum = getChannelFromName(channelName)
        channelEffects = channel.findall("effect")
        channelEffectDict[channelNum] = channelEffects
    return root, channelEffectDict

def parseLMSfromLOR(filename):
    root = ET.parse(filename).getroot()
    channelDict = {}
    dummyList = []
    for channel in root.findall("channels/channel"):
        unit = channel.get("unit")
        circuit = channel.get("circuit")
        if circuit is not None and unit is not None:
            channelNum = convertChannel(int(unit), int(circuit))
            channelDict[channelNum] = channel
        else:
            name = channel.get("name")
            if name is not None:
                for effect in list(channel):
                    channel.remove(effect)
                dummyList.append(channel)
    return root, channelDict, dummyList

def newChannelFromOldChannel(oldChannel):
    newChannel = ET.Element("channel")
    newChannel.set("name", oldChannel.get("name"))
    newChannel.set("color", oldChannel.get("color"))
    newChannel.set("centiseconds", oldChannel.get("centiseconds"))
    newChannel.set("deviceType", oldChannel.get("deviceType"))
    newChannel.set("unit", oldChannel.get("unit"))
    newChannel.set("circuit", oldChannel.get("circuit"))
    newChannel.set("savedIndex", oldChannel.get("savedIndex"))
    return newChannel

def reconstructSequence(xLightsFile, LORfile, outFile):
    # parse a well-formed LOR LMS file for reference    
    LORroot, channelDict, dummyList = parseLMSfromLOR(LORfile)
    # create a new sequence XML object based on the previous one
    oldRoot, effectDict = parseLMSfromXLights(xLightsFile)
    newSeq = ET.Element('sequence')
    newSeq.set("saveFileVersion", LORroot.get("saveFileVersion"))
    oldMusicFilename = oldRoot.get("musicFilename")
    newMusicFileName = ntpath.basename(oldMusicFilename)
    newSeq.set("musicFilename", newMusicFileName)
    # set channels and effects based on a combo of old xLights LMS file and well-formed LOR LMS file
    channels = ET.SubElement(newSeq, 'channels')
    for oldChannelNum in channelDict:
        oldChannel = channelDict[oldChannelNum]
        newChannel = newChannelFromOldChannel(oldChannel)
        newChannel.extend(effectDict[oldChannelNum])
        channels.append(newChannel)
    # add dummy channels
    channels.extend(dummyList)
    # add rgbChannels, cosmicColorDevices, and channelGroupLists
    LORchannel = LORroot.find("channels")
    rgbChannels = LORchannel.findall("rgbChannel")
    channels.extend(rgbChannels)
    cosmicColorDevices = LORchannel.findall("cosmicColorDevice")
    channels.extend(cosmicColorDevices)
    channelGroupLists = LORchannel.findall("channelGroupList")
    channels.extend(channelGroupLists)
    # add the timing grid from LOR
    timingGrids = LORroot.find("timingGrids")
    newSeq.append(timingGrids)
    newSeqTree = ET.ElementTree(newSeq)
    # set tracks based on the length of xLights file and saved indexes of LOR file
    tracks = LORroot.find("tracks")
    track = tracks.find("track")
    oldTrack = oldRoot.find("tracks/track")
    track.set("totalCentiseconds", oldTrack.get("totalCentiseconds"))
    newSeq.append(tracks)
    # add the animation designed in LOR
    animation = LORroot.find("animation")
    newSeq.append(animation)
    newSeqTree.write(outFile)
    return newSeq



if __name__ == '__main__':
    # parseLMSfromXLights("Hold Each Other Xlights.lms")
    # parseLMSfromLOR("Frozen Oceans.lms")
    # reconstructSequence("Hold Each Other Xlights.lms", "Frozen Oceans.lms", "Hold Each Other Reconstructed.lms")
    # fileName = "/Users/Lucy.Newman@ibm.com/Documents/ERDAJTminized/09 Blinding Lights.lms"
    fileName = "Eye of the Tiger (part xLights).lms"
    # fileName = "Eye of the Tiger (full xLights).lms"
    reconstructSequence(fileName, "Frozen Oceans.lms", "Eye of the Tiger (ERDAJTminimized).lms")

