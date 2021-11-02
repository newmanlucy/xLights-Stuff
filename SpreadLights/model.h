#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QString>
#include <QMap>
#include <qqml.h>
#include <QtQml>
#include <string>
#include "channelobject.h"
#include "flash.h"
#include <iostream>
#include <cstdlib>

using namespace std;


class Model : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
//    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
//    Q_PROPERTY(bool isParent READ isParent WRITE setIsParent NOTIFY isParentChanged)
//    QML_ELEMENT

public:
    Model();

    explicit Model(QString name, int id, QMap<int, ChannelObject *> *channels);

    QString name();
    void setName(const QString name);
//    QList<ChannelObject *> *channels();
//    void setChannels(QList<ChannelObject *> *cs);
    void addChannel(ChannelObject *c);
    bool removeChannel(ChannelObject *c);
    bool containsChannelId(int id);
    ChannelObject *channelAtId(int id);
    int chaseIndexAtId(int coid);
    int numChannels();


    static bool isParent;

//    bool operator == (const Model &Ref) const
//    {
//        return(this->id == Ref.id);
//    }

    int id();

    void addChild(Model *child);
    QList<QObject *> *childList();
    QList<QObject *> children();
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
    void addFlash(flash *f);
    void spreadFlashesToChildren();
    QMap<int, ChannelObject *> *channelMap();
    list<flash *> *flashList();
    void setFlashes(list<flash *> *flashes);
    void spreadFlashesWithModulus();
//    void addFlashesToMap(map<int, pair<channel, list<flash *> > > *lmsMap);

protected:
    QString m_name;
    QMap<int, ChannelObject *> *m_channels;
    int m_id;
    QList<QObject *> *m_children;
    list<flash *> *m_flashes;
};

//Q_DECLARE_OPAQUE_POINTER(Model *);

Q_DECLARE_METATYPE(Model *);

#endif // MODEL_H
