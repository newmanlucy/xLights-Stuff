#include "channelobject.h"
#include "channel.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

ChannelObject::ChannelObject() {

}

ChannelObject::ChannelObject(struct channel *c, int index, bool isChecked) {
    m_channel = c;
    m_chaseIndex = index;
    m_isChecked = isChecked;
    m_unit = c->unit;
    m_circuit = c->circuit;
    m_flashes = new list<flash *>();
//    qDebug(c->name);
}

int ChannelObject::id() {
    return channelStruct()->unit * 24 + channelStruct()->circuit;
}

QString ChannelObject::name() {
    return QString(m_channel->name);
}

channel *ChannelObject::channelStruct() {
    return m_channel;
}
int ChannelObject::chaseIndex() {
    return m_chaseIndex;
}


void ChannelObject::setChaseIndex(int index) {
    if (m_chaseIndex != index) {
        m_chaseIndex = index;
        qDebug() << "new index changed " << index;
        m_channel->index = index-1;
        emit chaseIndexChanged();
    }
}

bool ChannelObject::isChecked() {
    return m_isChecked;
}

void ChannelObject::setIsChecked(bool newIsChecked) {
    if (newIsChecked != m_isChecked) {
    m_isChecked = newIsChecked;
    emit isCheckedChanged();
    }
}

void ChannelObject::read(const QJsonObject &json) {
//    QString name = json["name"].toString();
    m_unit = json["unit"].toInt();
    m_circuit = json["circuit"].toInt();
//    m_chaseIndex = json["chaseIndex"].toInt();
    m_channel = new channel();
    parseChannel(json["string"].toString().toStdString(), m_channel, 0);
    setChaseIndex(json["chaseIndex"].toInt());
    m_flashes = new list<flash *>();
    QJsonArray flashArr = json["flashes"].toArray();
    for (int i=0; i<flashArr.size(); ++i) {
        flash *f = parseFlash(flashArr[i].toString().toStdString());
        f->channel = *m_channel;
//        qDebug() << QString::fromStdString(channelToString(f->channel));
        addFlash(f);
    }
}

void ChannelObject::write(QJsonObject &json) const {
    json["unit"] = m_unit;
    json["circuit"] = m_circuit;
    json["chaseIndex"] = m_chaseIndex;
    json["string"] = QString::fromStdString(channelToString(*m_channel));
    QJsonArray flashArray;
    for (list<flash *>::iterator it=m_flashes->begin(); it!=m_flashes->end(); ++it) {
        flash *flash = *it;
        string flashStr = flashToString(*flash);
        QJsonValue flashJStr = QJsonValue::fromVariant(QString::fromStdString((flashStr)));
        flashArray.append(flashJStr);
    }
    json["flashes"] = flashArray;
}

void ChannelObject::addFlash(flash *f) {
    m_flashes->push_back(f);
}

list<flash *> *ChannelObject::flashList() {
    return m_flashes;
}

void ChannelObject::setFlashes(list<flash *> *flashes) {
    m_flashes = flashes;
}
