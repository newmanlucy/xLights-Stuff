"""
extend.py: Extend ERDAJT lights sequences from the rectangle to the rest of the show
"""

import xml.etree.ElementTree as ET
import re
import ntpath
import json
from shutil import copyfile

# unit 27 circuit 5 is rect 1-1
# unit 29 circuit 12 is rect 5-8
WHITE = 0
RED = 1
GRREN = 2
BLUE = 3
MIXED = 4

def unitCircuitToChannel(unit, circuit):
    """
    unitCircuitToChannel -- finds the absolute index of a channel
    unit: the number of the unit (controller) the channel is on
    circuit: the offset of the channel from the unit
    returns: the channel as an integer
    """
    if unit <= 50:
        return (unit - 1) * 16 + circuit
    elif unit <= 59:
        return (16 * 49) + (unit - 50) * 24 + circuit
    elif unit <= 64:
        return (16 * 49) + (24 * 9) + (unit - 59) * 16 + circuit
    else:
        return (16 * 49)  + (24 * 9) + (16 * 5) + (unit - 64) * 24 + circuit

def channelToUnitCircuit(channel):
    """
    channelToUnitCircuit -- finds unit and circuit from absolute channel index
        (inverse of unitCircuitToChannel)
    channel: the absolute index of a channel in the show
    returns: a tuple (unit, circuit)
    """
    if channel <= 50 * 16:
        unit = channel // 16 + 1
        circuit = channel % 16
        return unit, circuit
    channel -= 50 * 16
    if channel <= 9 * 24:
        unit = channel // 24 + 51
        circuit = channel % 24
        return unit, circuit
    channel -= 9 * 24
    if channel <= 6:
        unit = channel // 16 + 60
        circuit = channel % 16
        return unit, circuit
    channel -= 6 * 16
    unit = channel // 24 + 65
    circuit = channel % 24
    return unit, circuit


def parseLMS(filename):
    """
    parseLMS -- obtainss relevant information from an LMS file
    filename: the name of the LMS file
    returns: a tuple (root, rectEffectDict, channelDict)
        root: an XML ElementTree representing the input file
        rectEffectDict: a dictionary of the effects at each channel.
            specifically, rectEffectDict[unit][circuit] = effects
            where "effects" is a list of ElementTree objects representing the effects
        channelDict: a dictionary relating channel indexes to their ElementTree objects
            channelDict[channelNum] = channel
            where channelNum is an integer representing the absolute index of a channel
            and channel is an ElementTree object representing that channel in the input file
    """
    root = ET.parse(filename).getroot()
    rectEffectDict = {}
    channelDict = {}
    for channel in root.findall("channels/channel"):
        unit = channel.get("unit")
        circuit = channel.get("circuit")
        if circuit is  None or unit is  None:
            continue
        unit = int(unit)
        circuit = int(circuit)
        channelNum = unitCircuitToChannel(unit, circuit)
        channelDict[channelNum] = channel
        inRect = unit == 28 or (unit == 27 and circuit >= 5) or (unit == 29 and circuit <= 12)
        if inRect:
            if unit not in rectEffectDict:
                rectEffectDict[unit] = {}
            rectEffectDict[unit][circuit] = channel.findall("effect")
    return root, rectEffectDict, channelDict



def spreadEffects(assignmentFile, lmsFile, root=None, rectEffectDict=None, channelDict=None, override=False):
    """
    spreadEffects -- spreads rectangle based on the models in assignmentFile
    assignmentFile: a json file corresponding each channel in the rectangle to a list of channels
        in the rest of the show
    lmsFile: the input file
    root, rectEffectDict, channelDict: optional parameters, None by default. They can be used
        to build upon the results of a previous call to spreadEffects without writing to a file,
        for example if you are calling multiple times with different asssignment files
    returns: tuple (root, rectEffect, channelDict). Same structures as in parseLMS, but updated
        based on the spreading
    """
    with open(assignmentFile) as f:
        assignments = json.load(f)
    if root is None:
        root, rectEffectDict, channelDict = parseLMS(lmsFile)
    # spreadEffectsDict = {}
    channel = root.find("channels")
    for a in assignments:
        modelEffects = rectEffectDict[a["modelChannel"][0]][a["modelChannel"][1]]
        # spreadEffectsDict[a["modelChannel"]] = modelEffects
        for c in a["childChannels"]:
            unit = c[0]
            circuit = c[1]
            childNum = unitCircuitToChannel(unit, circuit)
            childChannel = channelDict[childNum]
            xmlSearch = "*[@unit=\"%d\"][@circuit=\"%d\"]" % (unit, circuit)
            xmlChildChannel = channel.find(xmlSearch)
            if override:
                for effect in xmlChildChannel.findall("effect"):
                    xmlChildChannel.remove(effect)
            xmlChildChannel.extend(modelEffects)
    return root, rectEffectDict, channelDict


def spreadAll(lmsFile, default=True, rgb=False, trees=False, override=False):
    """
    spreadAll -- replaces lmsFile with the result of the spreading and creates a backup
    lmsFile: the input file
    default: spread rectangle to all non-RGB and non-Tree lights if True (True by default)
    rgb: spread rectangles to RGBs if True (False by default)
    trees: spread rectangle to 15- and 16-channel trees if True (False by default)

    """
    backupFile = lmsFile[:-4] + "_backup" + lmsFile[-4:]
    copyfile(lmsFile, backupFile)
    if default:
        root, rectEffectDict, channelDict = spreadEffects("assign.json", lmsFile)
        if trees:
            root, rectEffectDict, channelDict = spreadEffects("assignTrees.json", lmsFile, root=root, rectEffectDict=rectEffectDict, channelDict=channelDict)
        if rgb:
            root, rectEffectDict, channelDict = spreadEffects("assignRGB.json", lmsFile, root=root, rectEffectDict=rectEffectDict, channelDict=channelDict)
    elif trees:
        root, rectEffectDict, channelDict = spreadEffects("assignTrees.json", lmsFile)
        if rgb:
            root, rectEffectDict, channelDict = spreadEffects("assignRGB.json", lmsFile, root=root, rectEffectDict=rectEffectDict, channelDict=channelDict)
    elif rgb:
        root, rectEffectDict, channelDict = spreadEffects("assignRGB.json", lmsFile)
    tree = ET.ElementTree(root)
    tree.write(lmsFile)


if __name__ == '__main__':
    # spreadAll("Colors.lms", rgb=True, trees=True)
    # fileName = "Blinding Lights.lms"
    fileName = "Eye of the Tiger (DansLights).lms"
    spreadAll(fileName)
