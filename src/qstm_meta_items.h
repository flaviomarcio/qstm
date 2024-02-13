#pragma once

#include "./qstm_global.h"
#include <QVariant>
#include <QVariantList>
#include <QVariantHash>

#define Q_STM_OBJECT_PROPERTY_ID_NAME(PROPERTY_NAME)\
public:\
static QByteArray propertyId(){\
    static const auto __propertyIdName=QByteArray(#PROPERTY_NAME);\
    return __propertyName);\
}

namespace PrivateQStm {
class MetaItemsPvt;
//!
//! \brief The MetaItems class
//!
class Q_STM_EXPORT MetaItems:public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief MetaItems
    //!
    explicit MetaItems(const QMetaObject &itemMetaObject, QObject *parent=nullptr);

    //!
    //! \brief propertyId
    //! \return
    //!
    QString &propertyId()const;
    MetaItems &propertyId(const QString &newPropertyId);

    //!
    //! \brief clear
    //! \return
    //!
    MetaItems &clear();

    //!
    //! \brief clean
    //! \return
    //!
    MetaItems &clean();

    //!
    //! \brief item
    //! \param v
    //! \return
    //!
    MetaItems &item(const QVariant &v);

    //!
    //! \brief items
    //! \return
    //!
    MetaItems &items(const QVariant &v);
    //!
    //! \brief itemsUpdate
    //! \param v
    //! \return
    //!
    MetaItems &itemsUpdate(const QVariant &v);

    //!
    //! \brief count
    //! \return
    //!
    int count();

    //!
    //! \brief item
    //! \param index
    //! \return
    //!
    QObject *item(int index);

    //!
    //! \brief itemById
    //! \param v
    //! \return
    //!
    QObject *itemById(const QVariant &id);

    //!
    //! \brief itemJson
    //! \param index
    //! \return
    //!
    QString itemJson(int index);

    //!
    //! \brief itemJsonById
    //! \param id
    //! \return
    //!
    QString itemJsonById(const QVariant &id);

    //!
    //! \brief take
    //! \param index
    //! \return
    //!
    QObject *take(int index);

    //!
    //! \brief takeById
    //! \param v
    //! \return
    //!
    QObject *takeById(const QVariant &id);

    //!
    //! \brief toJsonList
    //! \return
    //!
    QString toJsonList();

    //!
    //! \brief toJsonHash
    //! \return
    //!
    QString toJsonHash();

private:
    MetaItemsPvt *p=nullptr;
};
}

namespace QStm {

template <typename T>
class Q_STM_EXPORT MetaItems:public PrivateQStm::MetaItems{
public:
    explicit MetaItems(QObject *parent=nullptr)
        :PrivateQStm::MetaItems{T::staticMetaObject, parent}{
    }

    //!
    //! \brief items
    //! \return
    //!
    const QList<T*> &items(){
        QList<T*> __return;
        for (int index = 0; index < this->count(); ++index) {
            auto item=this->item(index);
            if(item)
                __return.append(item);
        }
        return this->_items=__return;
    }

    //!
    //! \brief item
    //! \param index
    //! \return
    //!
    T *item(int index){
        auto o=PrivateQStm::MetaItems::item(index);
        return dynamic_cast<T*>(o);
    }

    //!
    //! \brief itemById
    //! \param v
    //! \return
    //!
    T *itemById(const QVariant &id){
        auto o=PrivateQStm::MetaItems::itemById(id);
        return dynamic_cast<T*>(o);
    }

    //!
    //! \brief take
    //! \param index
    //! \return
    //!
    T *take(int index){
        auto o=PrivateQStm::MetaItems::take(index);
        return dynamic_cast<T*>(o);
    }

    //!
    //! \brief takeById
    //! \param v
    //! \return
    //!
    T *takeById(const QVariant &id){
        auto o=PrivateQStm::MetaItems::takeById(id);
        return dynamic_cast<T*>(o);
    }
private:
    QList<T*> _items;
};

} // namespace QStm
