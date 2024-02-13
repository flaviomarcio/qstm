#pragma once

#include <QHash>
#include <QList>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QVariantHash>
#include "./qstm_global.h"

namespace QStm {
class MetaObjectUtilPvt;
//!
//! \brief The MetaObjectUtil class
//!util class to operation objects
class Q_STM_EXPORT MetaObjectUtil:public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief MetaObjectUtil
    //!
    explicit MetaObjectUtil(QObject *parent=nullptr);

    //!
    //! \brief MetaObjectUtil
    //! \param metaObject
    //!
    explicit MetaObjectUtil(const QMetaObject &objectMetaObject, QObject *parent=nullptr);

    //!
    //! \brief invoke
    //! \param object
    //! \param method
    //! \param returnValue
    //! \param args
    //! \return
    //!
    static bool invoke(QObject *object, QMetaMethod &method, QVariant &returnValue, const QVariantHash &args={});

    //!
    //! \brief invoke
    //! \param object
    //! \param method
    //! \param returnValue
    //! \param args
    //! \return
    //!
    static bool invoke(QObject *object, const QByteArray &methodName, QVariant &returnValue, const QVariantHash &args={});

    //!
    //! \brief invoke
    //! \param method
    //! \param returnValue
    //! \param args
    //! \return
    //!
    bool invoke(QMetaMethod &method, QVariant &returnValue, const QVariantHash &args={});

    //!
    //! \brief invoke
    //! \param methodName
    //! \param returnValue
    //! \param args
    //! \return
    //!
    bool invoke(const QByteArray &methodName, QVariant &returnValue, const QVariantHash &args={});

    //!
    //! \brief newInstance
    //! \param parent
    //! \return
    //!create object using QMetaObject
    QObject *newInstance(QObject *parent=nullptr);

    //!
    //! \brief newInstance
    //! \param metaObject
    //! \param parent
    //! \return
    //!create object using QMetaObject
    QObject *newInstance(const QMetaObject &metaObject, QObject *parent);

    //!
    //! \brief method
    //! \param name
    //! \return
    //!return QMetaMethod by name
    QMetaMethod method(const QString &name) const;

    //!
    //! \brief property
    //! \param name
    //! \return
    //!return QMetaProperty by name
    QMetaProperty property(const QByteArray &name) const;

    //!
    //! \brief toHash
    //! \param object
    //! \return
    //!extract propertys values with object and set in QVariantHash
    const QVariantHash toHash(const QObject *object) const;

    //!
    //! \brief writeHash
    //! \param object
    //! \param v
    //! \return
    //!write values in property using QVariantHash
    bool writeHash(QObject *object, const QVariantHash &v);

    //!
    //! \brief reset
    //! \param object
    //! \return
    //!
    void reset(QObject *object);

    //!
    //! \brief toMethodList
    //! \param object
    //! \return
    //!
    //! return list method of object
    const QList<QMetaMethod> toMethodList(const QObject *object=nullptr) const;

    //!
    //! \brief toMethodHash
    //! \param object
    //! \return
    //!
    //! return QHash methodof object
    const QHash<QByteArray,QMetaMethod> toMethodHash(const QObject *object=nullptr) const;

    //!
    //! \brief toPropertyList
    //! \param object
    //! \return
    //!return list propertys of object
    const QList<QMetaProperty> toPropertyList(const QObject *object=nullptr) const;

    //!
    //! \brief toPropertyMap
    //! \param object
    //! \return
    //!return QHash propertys of object
    const QHash<QByteArray,QMetaProperty> toPropertyHash(const QObject *object=nullptr) const;

    //!
    //! \brief newInstance
    //! \param parent
    //! \return
    //! //!create newInstance the object and converty to type.note, object a will be deleted, when different type
    template <class T>
    T *newInstance(QObject *parent)
    {
        QMetaObject &__metaObject=T::staticMetaObject;
        if(!__metaObject.inherits(parent->metaObject()))
            return nullptr;
        if(&__metaObject != parent->metaObject())
            return nullptr;

        auto object=this->newInstance(parent);
        if(!object)
            return nullptr;

        auto __return=dynamic_cast<T>(object);
        if(!__return){
            delete object;
            return nullptr;
        }
        return __return;
    }

private:
    MetaObjectUtilPvt *p=nullptr;
};

}
