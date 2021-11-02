#ifndef CONTROLS_H
#define CONTROLS_H

#include <QObject>
#include <QString>
#include <QMap>
#include <qqml.h>
#include <QtQml>
#include "model.h"
#include "listofmodels.h"
#include "channel.h"
#include "channelobject.h"


class Controls : public QObject
{
//    static Controls* instance;
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE updateModelName NOTIFY nameChanged)
    Q_PROPERTY(bool isParent READ isParent WRITE setIsParent NOTIFY isParentChanged)
    Q_PROPERTY(int numCheckedChannels READ numCheckedChannels NOTIFY numCheckedChannelsChanged)
//    Q_PROPERTY(QStringList parentNames READ parentNames NOTIFY parentNamesChanged)
    Q_PROPERTY(QList<QObject *> parents READ parents NOTIFY parentListChanged)
    Q_PROPERTY(QList<QObject *> channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(Model *currentParent READ currentParent WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(Model *currentChild READ currentChild WRITE setChild NOTIFY childChanged)
    Q_PROPERTY(QList<QObject *> children READ children NOTIFY childListChanged)
    QML_ELEMENT

public:
//    static Controls *getInstance();
    explicit Controls(QObject *parent = nullptr);
    void populate();
    void setModel(Model *m);
    Q_INVOKABLE void saveAndNewModel();
    Q_INVOKABLE void setChaseIndex(int channelIndex, int chaseIndexValue);
    void newModel();
    Model *currentParent();
//    Model currModel()
    QList<QObject *> *currentModelList();

    void setSpinNumber(QObject spinBox);
    QList<QObject *> *parentList();
    QList<QObject *> parents();

    Model *currentChild();
    void setChild(Model *child);
    QList<QObject *> *childList();
    QList<QObject *> children();

    Model *currentModel();
    QList<QObject *> *modelList();

    void addParentModel(Model *m);
    QStringList parentNames() {
        QStringList dataList = QStringList();
        for (list<Model *>::iterator it=m_parentModels->begin(); it !=m_parentModels->end(); ++it) {
            dataList.push_back((*it)->name());
        }
        return dataList;
    }


    void addChannel(channel *c);
    QList<QObject *> *channelList();
    QList<QObject *> channels();
    Q_INVOKABLE QList<QObject *> channelNames() {
        QList<QObject *> dataList = QList<QObject *>();
        int i = 0;
        for (list<channel *>::iterator it=m_channels->begin(); it != m_channels->end(); ++it, ++i) {
            ChannelObject *co = new ChannelObject((*it), 0, false);
            dataList.push_back(co);
        }
        return dataList;
    }

    int numCheckedChannels();

    void saveModelToList(QList<QObject *> *models, Model *m);
    bool deleteFromModelList(QList<QObject *> *models, Model *model);
    Q_INVOKABLE bool saveModel();
    Q_INVOKABLE bool deleteModel();
    QString name();
    void updateModelName(const QString &name);
    Q_INVOKABLE void checkChannel(int index);
    Q_INVOKABLE void uncheckChannel(int index);
    Q_INVOKABLE bool indexIsChecked(int index);
    bool saveModelsToFile();
    Q_INVOKABLE bool loadModelsFromFile();
    Q_INVOKABLE bool readChannelsFromLMS();
    Q_INVOKABLE bool readSequenceFromLMS();
    Q_INVOKABLE void spreadSequence();

    bool isParent();
    void setIsParent(bool isParent);
    void read(const QJsonObject &json);
    void write(QJsonObject &json);
    bool loadModelsFromFile(QString filename);
    Model *parentFromChannelId(int chid);
    void reconstructLMS();
    QMap<int, Model*> *channelChildMap();
    Model *getModelFromChannelId(int coid);

public slots:
    void onButtonClicked(QString str);
    void displayModel(Model *m);
    void displayModel(int index);


private:
    int maxId();
    void setToFirstModel();
    void addModelToChannelMap(Model *m);
    void sortChannels();
    QMap<int, int> *m_channelOrder;
    QList<QObject *> *m_parentList;
    QList<QObject *> *m_channelList;
    QMap<int, QObject *> *m_checkedChannelMap;
    QMap<int, Model *> *m_channelParentMap;
    Model *m_model;
    QString m_name;
    bool m_isParent;
    list<Model *> *m_parentModels;
    list<channel *> *m_channels;
    Model *m_child;
    string m_sequenceFile;
//    ListOfModels *m_parentModels;
//    QMap<int, ChannelObject*> *m_channelMap;

signals:
    void nameChanged();
    void isParentChanged();
    void numCheckedChannelsChanged();
//    void parentNamesChanged();
    void parentListChanged();
    void modelChanged();
    void channelsChanged();
    void childChanged();
    void childListChanged();
};

#endif // CONTROLS_H
