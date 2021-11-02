#include <string>
#include <iostream>
#include <QDebug>
#include <QtQuick>
#include <QComboBox>
#include <QDebug>
#include "controls.h"
#include "listofmodels.h"
#include <QMessageBox>
#include <algorithm>
#include <QFileDialog>
#include <fstream>
#include <regex>
#include "flash.h"
#include <map>
#include <utility>
#include <QMapIterator>



Controls::Controls(QObject *parent) :
    QObject(parent) {
    m_parentList = new QList<QObject *>();
    m_channels = new list<channel *>();
    m_channelList = new QList<QObject *>();
    m_channelParentMap = new QMap<int, Model *>();
    m_channelOrder = new QMap<int, int>();
    m_sequenceFile = "";

    setIsParent(true);

    const char *colors[5] = { "White", "Red", "Green", "Blue", "Mixed" };
//    const char *colors[5] = {"M1", "M2", "M3", "M4", "M5"};

//     TODO: move to main
//    for (int i=0; i<5; i++) {
//        Model *m = new Model(QString(colors[i]), i, new QMap<int, ChannelObject *>());
//        addParentModel(m);

//        channel *c = allocChannel(27, 5+i, i);
//        addChannel(c);
//    }
//    newModel();
    loadModelsFromFile(QStringLiteral("/Users/Lucy.Newman@ibm.com/Documents/saveModels_o4.json"));
    setToFirstModel();

}

void Controls::populate() {
    setIsParent(true);

    // TODO: move to main
    for (int i=0; i<5; i++) {
        Model *m = new Model(QString::fromStdString("Model " + to_string(i+1)), i, new QMap<int, ChannelObject *>());
        addParentModel(m);

        channel *c = allocChannel(27, 5+i, i);
        addChannel(c);
    }
    newModel();
    emit modelChanged();
}

QList<QObject *> *Controls::currentModelList() {
    // TODO: if child
    return m_parentList;
}

QList<QObject *> *Controls::channelList() {
    return m_channelList;
}

QList<QObject *> Controls::channels() {
    return *m_channelList;
}

Model *Controls::currentChild() {
    return m_child;
}

void Controls::setChild(Model *child) {
    m_child = child;
}

QList<QObject *> *Controls::childList() {
//    qDebug() << "child list for " << currentParent()->name();
//    for (QList<QObject *>::iterator it=currentParent()->childList()->begin(); it!=currentParent()->childList()->end(); ++it) {
//        Model *child = qobject_cast<Model *>(*it);
//        qDebug() << child->name();
//    }
    return currentParent()->childList();
}
QList<QObject *> Controls::children() {
    return currentParent()->children();
}

void Controls::setChaseIndex(int channelIndex, int chaseIndexValue) {
    QList<QObject *>::iterator it = channelList()->begin();
    advance(it, channelIndex);
    ChannelObject *co = qobject_cast<ChannelObject *>(*it);
//    qDebug() << "set chase index at " << co << " index " << channelIndex << " chaseIndexValue " << chaseIndexValue;
    co->setChaseIndex(chaseIndexValue);
}

void Controls::displayModel(Model *m) {
    setModel(m);
//    qDebug() << "m" << m->name() << "id" << m->id();
    childList();
//    qDebug() << m->name();
//    qDebug() << m->childList()->size() << " children";
    for (QList<QObject *>::iterator it2 = channelList()->begin(); it2 != channelList()->end(); ++it2) {
        ChannelObject *co = qobject_cast<ChannelObject *>(*it2);
        if (currentModel()->containsChannelId(co->id())) {
            co->setIsChecked(true);
//            qDebug() << "display channel index " << co->id() << " " << currentModel()->chaseIndexAtId(co->id()) << " at " << co;
            co->setChaseIndex(currentModel()->chaseIndexAtId(co->id()));
        } else {
            co->setIsChecked(false);
            co->setChaseIndex(0);
        }
    }
}

void Controls::displayModel(int index) {
//    qDebug() << "display model " << index;
    QList<QObject *>::iterator it = modelList()->begin();
    advance(it, index);
    Model *m = qobject_cast<Model *>(*it);
    displayModel(m);
}

Model *Controls::currentModel() {
    return isParent() ? currentParent() : currentChild();
}

QList<QObject *> *Controls::modelList() {
    return isParent() ? parentList() : childList();
}

void Controls::checkChannel(int index) {
    QList<QObject *>::iterator it = channelList()->begin();
    advance(it, index);
    ChannelObject *co = qobject_cast<ChannelObject *>(*it);
    currentModel()->addChannel(co);
}

void Controls::uncheckChannel(int index) {
//    qDebug() << "index: " << index;
    QList<QObject *>::iterator it = channelList()->begin();
    advance(it, index);
    ChannelObject *co = qobject_cast<ChannelObject *>(*it);
//    qDebug() << channelNames().length();
//    qDebug() << "before remove channel";
    currentModel()->removeChannel(co);
}

bool Controls::indexIsChecked(int index) {
    QList<QObject *>::iterator it = channelNames().begin();
    advance(it, index);
    ChannelObject *co = qobject_cast<ChannelObject *>(*it);
    return currentModel()->containsChannelId(co->id());
}

void Controls::onButtonClicked(QString str) {
    qDebug("hiiii");
    qDebug() << str;
//    char *m = (char *) currentModel();
//    qDebug (m);
//    saveAndNewModel();
}

void Controls::read(const QJsonObject &json) {
    m_parentList->clear();
    QJsonArray parentArr = json["parents"].toArray();
    for (int i=0; i<parentArr.size(); ++i) {
        QJsonObject parentJObject = parentArr[i].toObject();
        Model *parent = new Model();
        parent->read(parentJObject);
        addParentModel(parent);
    }
    QJsonArray channelArr = json["channels"].toArray();
    for (int i=0; i<channelArr.size(); ++i) {
        QJsonObject channelJObject = channelArr[i].toObject();
        ChannelObject *co = new ChannelObject();
        co->read(channelJObject);
        addChannel(co->channelStruct());
    }
}

void Controls::write(QJsonObject &json) {
    QJsonArray parentArr;
    for (QList<QObject *>::iterator it=parentList()->begin(); it!= parentList()->end(); ++it) {
        Model *parent = qobject_cast<Model *>(*it);
        QJsonObject parentJObject;
        parent->write(parentJObject);
        parentArr.append(parentJObject);
    }
    json["parents"] = parentArr;
    QJsonArray channelArr;
    for (QList<QObject *>::iterator it=channelList()->begin(); it!=channelList()->end(); ++it) {
        ChannelObject *co = qobject_cast<ChannelObject *>(*it);
        QJsonObject channelJObject;
        co->write(channelJObject);
        channelArr.append(channelJObject);
    }
    json["channels"] = channelArr;
}

void Controls::addModelToChannelMap(Model *m) {
    for (int i=0; i<m->channelMap()->keys().size(); ++i) {
        m_channelParentMap->insert(m->channelMap()->keys().at(i), m);
    }
}

void Controls::addParentModel(Model *m) {
//    qDebug() << 1;
    m_parentList->push_back(m);
    addModelToChannelMap(m);
    for (QList<QObject *>::iterator it=m->childList()->begin(); it!=m->childList()->end(); ++it) {
        Model *child = qobject_cast<Model *>(*it);
        addModelToChannelMap(child);
    }
//    qDebug() << 2;
}

void Controls::addChannel(channel *c) {
//    m_channels->push_back(c);
    ChannelObject *co = new ChannelObject(c, 0, false);
    m_channelList->push_back(co);
//    emit channelsChanged();
}

void Controls::sortChannels() {
    qSort(channelList()->begin(), channelList()->end(), [this](QObject *a, QObject *b) {
        ChannelObject *co1 = qobject_cast<ChannelObject *>(a);
        ChannelObject *co2 = qobject_cast<ChannelObject *>(b);
        if (!m_channelOrder->contains(co1->channelStruct()->savedIndex) || !m_channelOrder->contains(co2->channelStruct()->savedIndex)) {
            return true;
        }
        return m_channelOrder->value(co1->channelStruct()->savedIndex) < m_channelOrder->value(co2->channelStruct()->savedIndex);
    });
}

Model *Controls::currentParent() {
    return m_model;
}

void Controls::setModel(Model *m) {

//    qDebug() << "child list size " << m->childList()->size();
    if (isParent()) {
        m_model = m;
        m_name = m->name();
        if (m->childList()->size() != 0) {
            QList<QObject *>::iterator it = m->childList()->begin();
//            qDebug() << "child list" << (*it);
            m_child = qobject_cast<Model *>(*it);
        } else {
            QString nname = currentParent()->name() + QString(" child 1");
            m_child = new Model(nname, currentParent()->children().size(), new QMap<int, ChannelObject*>());
        }
    } else {
        m_child = m;
        m_name = m->name();
    }
    emit nameChanged();
    emit modelChanged();
    emit childListChanged();
}

QList<QObject *> *Controls::parentList() {
//    for (QList<QObject *>::iterator it=m_parentList->begin(); it != m_parentList->end(); ++it) {
//        Model *m = dynamic_cast<Model *>(*it);
//        qDebug(m->name().toUtf8());
//    }
    return m_parentList;
}

QList<QObject *> Controls::parents() {
    return *m_parentList;
}

int Controls::maxId() {
    int current_id = 0;
    for (QList<QObject *>::iterator it=parentList()->begin(); it!= parentList()->end(); ++it) {
        Model *m = qobject_cast<Model *>(*it);
        if (m->id() > current_id) {
            current_id = m->id();
        }
    }
    return current_id;
}



void Controls::newModel() {
    Model *m;
    if (isParent()) {
        m = new Model(QString::fromStdString("Model " + to_string(maxId() + 2)), maxId() + 1, new QMap<int, ChannelObject *>());
        setModel(m);
        emit modelChanged();
    } else {
        int mid = currentParent()->childList()->size();
        QString nname = currentParent()->name() + QString::fromStdString(" child " + to_string(mid+1));
//        qDebug() << "new  model name" << nname;
        m = new Model(nname, mid, new QMap<int, ChannelObject *>());
        setChild(m);
        emit childChanged();
    }
    displayModel(m);
}

void Controls::saveAndNewModel() {
    // TODO: save what you had before
    if (!saveModel()) {
        return;
    }
    newModel();
}

void Controls::saveModelToList(QList<QObject *> *models, Model *model) {
//    qDebug() << "save model to list " << model->id();
    QList<QObject *>::iterator it;
    for (it=models->begin(); it!=models->end(); ++it) {
        QObject *qo = (*it);
        Model *m = qobject_cast<Model *>(qo);
//        qDebug() << model->name();
//        qDebug() << m->name();
        Q_ASSERT( m != nullptr );
        if (m->id() == model->id()) {
            it = models->erase(it);
            models->insert(it, model);
            return;
        }
    }
    models->push_back(model);
    addModelToChannelMap(model);
}

bool Controls::saveModel() {
    // The boolean indicates whether they want to proceed
    QMessageBox msgBox;
    msgBox.setText("Would you like to save the model?");
    msgBox.setInformativeText("If you don't save, any modifications you have made will be discarded.");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch (ret) {
        case QMessageBox::Save:
            saveModelToList(modelList(), currentModel());
            if (isParent()) {
                emit parentListChanged();
            }  else {
                emit childListChanged();
            }
            saveModelsToFile();
            return true;
        case QMessageBox::Discard:
          // Don't Save was clicked
             return true;
        case QMessageBox::Cancel:
          // Cancel was clicked
            return false;
        default:
        // shouldn't happen
             break;
    }
    return false;
}

bool Controls::deleteFromModelList(QList<QObject *> *models, Model *model) {
    qDebug() << "delete from model list " << model->id();
    QList<QObject *>::iterator it;
    for (it=models->begin(); it!=models->end(); ++it) {
        QObject *qo = (*it);
        Model *m = qobject_cast<Model *>(qo);
        qDebug() << m->name();
        Q_ASSERT( m != nullptr );
        if (m->id() == model->id()) {
            it = models->erase(it);
            return true;
        }
    }
    return false;
}

bool Controls::deleteModel() {
    // The boolean indicates whether they want to proceed
    QMessageBox msgBox;
    msgBox.setText("Are you sure  you want to delete " + currentModel()->name() + "?");
    if (isParent()) {
        msgBox.setInformativeText("If you delete this model, all channel information associated with this it and its children will be lost.");
    } else {
        msgBox.setInformativeText("If you delete this model, all channel information will be lost.");
    }
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();

    switch (ret) {
        case QMessageBox::Ok:
            deleteFromModelList(modelList(), currentModel());
            setToFirstModel();
            if (isParent()) {
                emit parentListChanged();
            }  else {
                emit childListChanged();
            }
            return true;
        case QMessageBox::Cancel:
          // Cancel was clicked
            return false;
        default:
        // shouldn't happen
             break;
    }
    return false;
}

void Controls::setToFirstModel() {
    if (modelList()->length() == 0) {
        return;
    }
    QList<QObject *>::iterator it = modelList()->begin();
    Model *m = qobject_cast<Model *>(*it);
    displayModel(m);
}


void Controls::updateModelName(const QString &name) {
    if (isParent()) {
        m_model->setName(name);
    } else {
        m_child->setName(name);
    }
}

QString Controls::name() {
    return isParent() ? currentParent()->name() : currentChild()->name();
}

bool Controls::isParent() {
    return m_isParent;

}

void Controls::setIsParent(bool ip) {
    m_isParent = ip;
}

int Controls::numCheckedChannels() {
    Model *currModel = isParent() ? currentParent() : currentChild();
    int count = currModel->numChannels();
//    qDebug() << "num checked channels " << count;
    return count;
}


bool Controls::saveModelsToFile() {
    QFile saveFile(QStringLiteral("/Users/Lucy.Newman@ibm.com/Documents/saveModels.json"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonObject modelsJObject;
    write(modelsJObject);
    QJsonDocument saveDoc(modelsJObject);
    saveFile.write(saveDoc.toJson());

    return true;
}

bool Controls::loadModelsFromFile() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Json (*.json)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    if (fileNames.length()==0) {
        return false;
    }
    QFile loadFile(fileNames[0]);

    return loadModelsFromFile(fileNames[0]);
}

bool Controls::loadModelsFromFile(QString filename) {
    QFile loadFile(filename);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    m_channels->clear();
    emit channelsChanged();
//    qDebug() << m_channels->size() << "channels";
    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    read(loadDoc.object());
    setToFirstModel();
    emit modelChanged();
    emit parentListChanged();
    emit childListChanged();
    emit channelsChanged();
    return true;
}

bool Controls::readChannelsFromLMS() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Light-o-Rama Sequence (*.lms *.lma *.xml)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    if (fileNames.size()==0) {
        return false;
    }
    ifstream lmsFile(fileNames[0].toStdString());

    string line;
    smatch match_res;
    regex rgb("\\([RGB]\\)");
    uint index = 0;
    channel *c = new channel();
    if (!lmsFile.is_open()) {
        return false;
    }
    m_channelList->clear();
    emit channelsChanged();
    int orderIndex = 0;
    bool seenTracks = false;
    int channelGroup = -1;

    QMap<int, QList<int>* > channelGroups;
    regex pattern1 ("<channelGroupList .* savedIndex=\"([0-9]+)\">");
//    regex pattern2 ("<channelGroup savedIndex=\"([0-9]+)\"/>");
    regex pattern2 ("<channelGroup savedIndex=\"([0-9]+)\"");
    regex pattern3 ("<channel savedIndex=\"([0-9]+)\"");
    while (getline(lmsFile, line)) {
        if (parseChannel(line, c, index) && !regex_search(c->name, rgb)) {
            addChannel(c);
            index++;
            c = new channel();
        }
        else {
            if (regex_search(line, match_res, pattern1)) {
                channelGroup = stoi(match_res[1]);
                channelGroups[channelGroup] = new QList<int>();
//                qDebug() << "matches";
            } else if (line.find("</channelGroupList")!=string::npos) {
                channelGroup = -1;
            }
            if (channelGroup != -1) {
//                qDebug() << "channel group: " << channelGroup;
//                qDebug() << QString::fromStdString(line);
                if (regex_search(line, match_res, pattern2)) {
//                    qDebug() << stoi(match_res[1]);
                    channelGroups[channelGroup]->push_back(stoi(match_res[1]));
                }
            }

            if (line.find("<tracks")!=string::npos) {
                seenTracks = true;
            }
            if (seenTracks && regex_search(line, match_res, pattern3)) {
//                qDebug() << stoi(match_res[1]);
                m_channelOrder->insert(stoi(match_res[1]), orderIndex);
                orderIndex += 100;
            }
        }
    }
    for (QMap<int, QList<int>*>::iterator it=channelGroups.begin(); it!=channelGroups.end(); ++it) {
        int i=1;
        for (QList<int>::iterator it2=it.value()->begin(); it2!=it.value()->end(); ++it2, ++i) {
            m_channelOrder->insert(*it2, it.key() + i);
        }
//        qDebug() << "channel group key: " << it.key();
//        qDebug() << *channelGroups[it.key()];
        delete it.value();
    }
//    qDebug()<<channelGroups;
//    qDebug()<<*m_channelOrder;
    lmsFile.close();
    sortChannels();
//    qDebug() <<8;
    emit channelsChanged();
    return true;
}

Model *Controls::parentFromChannelId(int chid) {
    if (m_channelParentMap->contains(chid)) {
        return m_channelParentMap->value(chid);
    }
    return nullptr;
}

bool Controls::readSequenceFromLMS() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Light-o-Rama Sequence (*.lms *.lma *.xml)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    if (fileNames.size()==0) {
        return false;
    }
    m_sequenceFile = fileNames[0].toStdString();
    ifstream lmsFile(m_sequenceFile);

    string line;
    regex pattern1 ("<effect type=\"(.*)\" startCentisecond=\"(.*)\" endCentisecond=\"(.*)\" startIntensity=\"(.*)\" endIntensity=\"(.*)\"");
    regex pattern2 ("<effect type=\"(.*)\" startCentisecond=\"(.*)\" endCentisecond=\"(.*)\" intensity=\"(.*)\"");
    smatch match_res;
    uint index = 0;
    channel c;
    for (QList<QObject *>::iterator it=parentList()->begin(); it!=parentList()->end(); ++it) {
        Model *parent = qobject_cast<Model *>(*it);
        for (QMap<int, ChannelObject *>::iterator it2 = parent->channelMap()->begin(); it2!=parent->channelMap()->end(); ++it2) {
            ChannelObject *co = *it2;
            co->setFlashes(new list<flash *>());
        }
    }
    Model *parent = nullptr;
    if (lmsFile.is_open()) {
        while (getline(lmsFile, line)) {
            if (parent == nullptr) {
                if (parseChannel(line, &c, index)) {
//                    ChannelObject *co = new ChannelObject(&c, 0, false);
                    parent = parentFromChannelId(getCOid(c));
//                    qDebug() << parent;
//                    qDebug() << QString::fromStdString(channelToString(*co->channelStruct()));
                }
                continue;
            }
            if (line.find("</channel>") != string::npos) {
                parent = nullptr;
                continue;
            }
            flash *f;
            if (regex_search(line, match_res, pattern1)) {
//                qDebug() << QString::fromStdString(match_res[1]) << 1;
                f = allocFlash(match_res[1], stoi(match_res[2]), stoi(match_res[3]), stoi(match_res[4]), stoi(match_res[5]), c);
//                qDebug() << QString::fromStdString(channelToString(f->channel));
                parent->addFlash(f);
            } else if (regex_search(line, match_res, pattern2)) {
//                qDebug() << QString::fromStdString(match_res[1]) << 2;
                f = allocFlash(match_res[1], stoi(match_res[2]), stoi(match_res[3]), stoi(match_res[4]), stoi(match_res[4]), c);
                parent->addFlash(f);
            }
        }
        lmsFile.close();
    }
    return true;
}

void Controls::spreadSequence() {
    for (QList<QObject *>::iterator it=parentList()->begin(); it!=parentList()->end(); ++it) {
        Model *parent = qobject_cast<Model *>(*it);
        parent->spreadFlashesWithModulus();
        // TODO: fix "spreadFlashesToChildren"
//        parent->spreadFlashesToChildren();
    }
    reconstructLMS();
}

//TODO: construct this as channels are added & in read function
QMap<int, Model*> *Controls::channelChildMap() {
    QMap<int, Model *> *channelMapP = new QMap<int, Model *>();
    for (QList<QObject *>::iterator it=parentList()->begin(); it!=parentList()->end(); ++it) {
        Model *parent = qobject_cast<Model *>(*it);
        for (QList<QObject *>::iterator it2=parent->children().begin(); it2!=parent->children().end(); ++it2) {
            Model *child = qobject_cast<Model *>(*it2);
            for (QMap<int, ChannelObject *>::iterator it3=parent->channelMap()->begin(); it3!=parent->channelMap()->end(); ++it3) {
                channelMapP->insert(it3.key(), child);
            }
        }
    }
    return channelMapP;
}

Model *Controls::getModelFromChannelId(int coid) {
    if (channelChildMap()->contains(coid)) {
        return channelChildMap()->value(coid);
    } else if (m_channelParentMap->contains(coid)) {
        return m_channelParentMap->value(coid);
    }
    return nullptr;
}

void Controls::reconstructLMS() {
//    qDebug() << 1;
    if (m_sequenceFile == "") {
        return;
    }
    ifstream lmsFile(m_sequenceFile);
    if (!lmsFile) {
        return;
    }
//    qDebug() << 1;
    string line;
    bool inChannel = false;
    bool inSeq = false;
    channel *c = new channel;
    ofstream outFile;
    outFile.open("/Users/Lucy.Newman@ibm.com/Documents/expanded.lms");
    QMap<int, Model *> *childMap = channelChildMap();
    while (getline(lmsFile, line)) {
        if (inSeq && line.find("<effect")!=string::npos) {
            continue;
        } else {
            outFile << line;
        }
        if (!inSeq) {
            if (line.find("<sequence")!=string::npos) {
                inSeq = true;
            }
        } else {
            if (line.find("</sequence")!=string::npos) {
                inSeq = false;
            }
        }
        if (inSeq && parseChannel(line, c, 0)) {
            int coid = getCOid(*c);
//            qDebug() << "coid: " << coid;
            Model *m = nullptr;
//            if (childMap->contains(coid)) {
//                m = childMap->value(coid);
//            } else
            if (m_channelParentMap->contains(coid)) {
//                qDebug() << "here 1";
//                qDebug() << m_channelParentMap->size();
                m = m_channelParentMap->value(coid);
//                qDebug() << "here 2";
            }
            if (m == nullptr) {
                continue;
            }
//            qDebug() << m->name();
//            qDebug() << QString::fromStdString(channelToString(*c));
//            qDebug() << "flash list size: " << m->flashList()->size();
            ChannelObject *co = m->channelAtId(coid);
//            qDebug() << "co: " << co;
//            qDebug() << "flash list size for CO: " << co->flashList()->size();
            for (list<flash *>::iterator it=co->flashList()->begin(); it!=co->flashList()->end(); ++it) {
                flash *f = *it;
                outFile << flashToString(*f);
//                qDebug() << QString::fromStdString(flashToString(*f));
            }
//            qDebug() << 4;
        }
    }
    outFile.close();
    qDebug() << 5;
}
