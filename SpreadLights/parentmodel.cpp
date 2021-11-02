#include "parentmodel.h"

ParentModel::ParentModel(QString name, int id, QMap<int, ChannelObject *> *cs)
    : Model(name, id, cs) {
    m_children = new QList<Model *>;
}

void ParentModel::addChild(Model *child) {
    m_children->push_back(child);
}

QList<Model *> *ParentModel::childList() {
    return m_children;
}

QList<Model *> ParentModel::children() {
    return *m_children;
}
