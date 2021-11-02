#include "listofmodels.h"
#include <QDebug>

ListOfModels::ListOfModels(QObject *parent) {
    m_modelsList = list<Model *>();
}

QHash<int, QByteArray> ListOfModels::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    return roles;
}

void ListOfModels::addModel(Model *m) {
    beginInsertRows( QModelIndex(), 0, 0 );
    m_modelsList.push_front(m);
    endInsertRows();
    QString count = QString::fromStdString(to_string(rowCount()) + " rows");

}

bool ListOfModels::insertRows(int row, int count, const QModelIndex &parent) {
     list<Model *>::iterator it = m_modelsList.begin();
     advance(it, row);
     beginInsertRows(parent, row, row+count);
     for (int i=0 ;it != m_modelsList.end() && i < count; ++it, ++i) {

     }
     endInsertRows();
}


int ListOfModels::rowCount(const QModelIndex &parent) const {
    return m_modelsList.size();
}

QVariant ListOfModels::data(const QModelIndex &index, int role) const {
    auto it = m_modelsList.begin();
    advance(it, index.row());
    QString qs;
    if (role == NameRole) {
        string s = "row " + to_string(index.row());
        qs = QString::fromStdString(s);
    }
    return qs;
}

//QVariant ListOfModels::headerData(int section, Qt::Orientation orientation, int role) const {

//}
