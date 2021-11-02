#include "model.h"
#include <string>
#include <QDebug>
#include <QtAlgorithms>
#include <QJsonArray>

Model::Model() {
    m_children = new QList<QObject *>;
    m_channels = new QMap<int, ChannelObject*>();
    m_flashes = new list<flash *>();
//    qDebug() << m_flashes->size() << "flashes";
}

Model::Model(QString name, int id, QMap<int, ChannelObject *> *cs) {

    setName(name);
    m_channels = cs;
    m_id = id;
    m_children = new QList<QObject *>;
    m_flashes = new list<flash *>();
}

int Model::id() {
    return m_id;
}

QString Model::name(){
    return m_name;
}

void Model::setName(QString sname) {
    m_name = sname;
}

bool Model::containsChannelId(int id) {
    return m_channels->find(id) != m_channels->end();
}

ChannelObject *Model::channelAtId(int id) {
    return m_channels->value(id);
}

int Model::chaseIndexAtId(int coid) {
    if (!containsChannelId(coid)) {
        return -1;
    }
    QMap<int, ChannelObject*>::iterator it = m_channels->find(coid);
    return (*it)->chaseIndex();
}

int Model::numChannels() {
    return m_channels->size();
}


//void Model::setChannels(QList<ChannelObject *> *cs) {
//    sort(cs->begin(), cs->end(), [](ChannelObject *co1, ChannelObject *co2) {
//        return co1->chaseIndex() < co2->chaseIndex();
//    });
//    m_channels = cs;
//}

void Model::addChannel(ChannelObject *c) {
//    qDebug() << "channel " << c->id() << " chase index " << c->chaseIndex();
    ChannelObject *newC = new ChannelObject(c->channelStruct(), c->chaseIndex(), c->isChecked());
//    qDebug() << "new chase index " << newC->chaseIndex();
    m_channels->insert(getCOid(*c->channelStruct()), newC);
}

bool Model::removeChannel(ChannelObject *c) {
//    qDebug() << "in remove channel";
//    qDebug() << "id: " << c->id();
//    qDebug() << "contains id?" <<  containsChannelId(c->id());
//    qDebug() << "num channels:" << numChannels();
    return m_channels->remove(c->id());
}

void Model::addChild(Model *child) {
    m_children->push_back(child);
}

QList<QObject *> *Model::childList() {
    return m_children;
}

QList<QObject *> Model::children() {
    return *m_children;
}

void Model::addFlash(flash *f) {
//    m_flashes->push_back(f);
    int coid = getCOid(f->channel);
    ChannelObject *co = channelMap()->value(coid);
    co->addFlash(f);
    f->channel.index = co->channelStruct()->index;
}

list<flash *> *Model::flashList() {
    list<flash *> *flashes = new list<flash *>();
    for (QMap<int, ChannelObject*>::iterator it=channelMap()->begin(); it!=channelMap()->end(); ++it) {
        flashes->insert(flashes->end(), it.value()->flashList()->begin(), it.value()->flashList()->end());
    }
    return flashes;
}

void Model::setFlashes(list<flash *> *flashes) {
    for (list<flash *>::iterator it=flashes->begin(); it!=flashes->end(); ++it) {
        flash *f = *it;
        addFlash(f);
    }
}


void Model::write(QJsonObject &json) const {
    QJsonArray channelArray;
    for (QMap<int, ChannelObject *>::iterator it=m_channels->begin(); it!=m_channels->end(); ++it) {
        ChannelObject *co = *it;
        QJsonObject channelJObject;
        co->write(channelJObject);
        channelArray.append(channelJObject);
    }

    QJsonArray modelArray;
    for (QList<QObject *>::iterator it=m_children->begin(); it!=m_children->end(); ++it) {
        Model *m = qobject_cast<Model *>(*it);
        QJsonObject modelJObject;
        m->write(modelJObject);
        modelArray.append(modelJObject);
    }

//    QJsonArray flashArray;
//    for (list<flash *>::iterator it=m_flashes->begin(); it!=m_flashes->end(); ++it) {
//        flash *flash = *it;
//        string flashStr = flashToString(*flash);
//        QJsonValue flashJStr = QJsonValue::fromVariant(QString::fromStdString((flashStr)));
//        flashArray.append(flashJStr);
//    }
    json["channels"] = channelArray;
    json["children"] = modelArray;
//    json["flashes"] = flashArray;
    json["id"] = m_id;
    json["name"] = m_name;
}

void Model::read(const QJsonObject &json) {
    m_channels->clear();
    QJsonArray channelsArr = json["channels"].toArray();
    for (int i=0; i < channelsArr.size(); ++i) {
        QJsonObject channelJObject = channelsArr[i].toObject();
        ChannelObject *co = new ChannelObject();
        co->read(channelJObject);
        addChannel(co);
    }
    m_children->clear();
    QJsonArray modelsArr = json["children"].toArray();
    for (int i=0; i<modelsArr.size(); ++i) {
        QJsonObject modelJObject = modelsArr[i].toObject();
        Model *child = new Model();
        child->read(modelJObject);
        addChild(child);
    }
//    m_flashes = new list<flash *>();
//    QJsonArray flashArr = json["flashes"].toArray();
//    for (int i=0; i<flashArr.size(); ++i) {
//        flash *f = parseFlash(flashArr[i].toString().toStdString());
//        addFlash(f);
//    }
    m_name = json["name"].toString();
    m_id = json["id"].toInt();
}

QMap<int, ChannelObject *> *Model::channelMap() {
    return m_channels;
}


void Model::spreadFlashesWithModulus() {
    int numParentChannels = numChannels();
    for (QList<QObject *>::iterator it1=m_children->begin(); it1!=m_children->end(); ++it1) {
        list<flash *> *childFlashes = new list<flash *>;
        Model *child = qobject_cast<Model *>(*it1);
        int numChildChannels = child->numChannels();
        int reps = 0;
        while (reps * numChildChannels < numParentChannels) {
            childFlashes->insert(childFlashes->end(), flashList()->begin(), flashList()->end());
            reps++;
        }
        int remainder = 0;
        list<flash *>::iterator it2 = flashList()->begin();
        while (reps * numChildChannels + remainder < numParentChannels) {
            childFlashes->insert(childFlashes->end(), it2, ++it2);
            remainder++;
        }
        child->setFlashes(childFlashes);
    }
}

void Model::spreadFlashesToChildren() {
    for (QList<QObject *>::iterator it1=m_children->begin(); it1!=m_children->end(); ++it1) {
        Model *child = qobject_cast<Model *>(*it1);
        channel *child_channels = (channel *) malloc(sizeof(channel)  * child->numChannels());
        int i=0;
        for (QMap<int, ChannelObject*>::iterator it2=child->channelMap()->begin(); it2!=child->channelMap()->end(); ++it2, ++i) {
            copyChannel((*it2)->channelStruct(), &child_channels[i]);

        }
        list<flash *> *child_flashes = expandFlashes(flashList(), channelMap()->size(), child->numChannels(), child_channels);

        child->setFlashes(child_flashes);

    }
}

