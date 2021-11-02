#ifndef LISTOFMODELS_H
#define LISTOFMODELS_H

#include <QAbstractListModel>
#include <QObject>
#include <list>
#include "model.h"

class ListOfModels : public QAbstractListModel
{
    Q_OBJECT
public:
    ListOfModels(QObject *parent = nullptr);

    enum ModelRoles {
        NameRole = Qt::UserRole + 1,
    };

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;
    void addModel(Model *m);

private:
    int m_numModels;
    list<Model *> m_modelsList;
};

#endif // LISTOFMODELS_H
