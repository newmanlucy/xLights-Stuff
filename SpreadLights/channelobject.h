#ifndef CHANNELOBJECT_H
#define CHANNELOBJECT_H

#include <QObject>
#include "channel.h"
#include "flash.h"

class ChannelObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(int chaseIndex READ chaseIndex WRITE setChaseIndex NOTIFY chaseIndexChanged)
    Q_PROPERTY(bool isChecked READ isChecked WRITE setIsChecked NOTIFY isCheckedChanged)
public:
    ChannelObject();
    ChannelObject(channel *c, int index, bool isChecked);
    QString name();
    channel *channelStruct();
    int chaseIndex();
    Q_INVOKABLE void setChaseIndex(int index);
    bool isChecked();
    Q_INVOKABLE void setIsChecked(bool newIsChecked);
    int id();
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
    void addFlash(flash *f);
    list<flash *> *flashList();
    void setFlashes(list<flash *> *flashes);

signals:
    void chaseIndexChanged();
    void isCheckedChanged();

private:
    channel *m_channel;
    int m_chaseIndex;
    bool m_isChecked;
    int m_unit;
    int m_circuit;
    QString m_name;
    list<flash *> *m_flashes;

};

#endif // CHANNELOBJECT_H
