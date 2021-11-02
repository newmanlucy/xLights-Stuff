#ifndef PARENTMODEL_H
#define PARENTMODEL_H


#include "model.h"


class ParentModel : public Model
{
public:
    ParentModel(QString name, int id, QMap<int, ChannelObject *> *cs);

    void addChild(Model *child);
    QList<Model *> *childList();
    QList<Model *> children();

private:
    QList<Model *> *m_children;
};

#endif // PARENTMODEL_H
