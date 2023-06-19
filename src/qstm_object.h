#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantHash>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include "./qstm_global.h"
#include "./qstm_result.h"
#include "./qstm_cache_pool.h"

#define Q_DECLARE_INSTANCE(ClassName)\
public:\
static ClassName&instance()\
{\
    static ClassName*static_##ClassName=nullptr;\
    if(static_##ClassName==nullptr)\
    static_##ClassName=new ClassName(nullptr);\
    return *static_##ClassName;\
}\
static ClassName&i()\
{\
    return instance();\
}


namespace QStm {
class ObjectPvt;
//!
//! \brief The Object class
//!
//! basic object class
class Q_STM_EXPORT Object: public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit Object(QObject *parent=nullptr);

    //!
    //! \brief lr
    //! \return
    //! result value class
    virtual ResultValue &lr() const;
    __QSTM_DECLARE_RESULT_READ_LR(ResultValue&)
    __QSTM_DECLARE_RESULT_READ_LR(QVariant &)

public:
    //!
    //! \brief cachePool
    //! \return
    //!
    CachePool &cachePool();

    //!
    //! \brief now
    //! \return
    //!
    static const QDateTime now();

    //!
    //! \brief toMd5
    //! \param value
    //! \return
    //!
    static const QByteArray toMd5(const QByteArray &value);

    //!
    //! \brief toMd5
    //! \param value
    //! \return
    //!
    static const QByteArray toMd5(const QString &value);

    //!
    //! \brief toMd5
    //! \param value
    //! \return
    //!
    static const QByteArray toMd5(const QVariant &value);

    //!
    //! \brief uuidGenerator
    //! \return
    //!
    static const QUuid uuidGenerator();

    //!
    //! \brief uuidGenerator
    //! \param uuidBase
    //! \return
    //!
    static const QUuid uuidGenerator(const QString &uuidBase);

    //!
    //! \brief uuidGenCheck
    //! \param uuid
    //! \return
    //!
    static const QUuid uuidGenCheck(const QUuid &uuid);

    //!
    //! \brief uuidGenCheck
    //! \param uuid
    //! \param uuidDefault
    //! \return
    //!
    static const QUuid uuidGenCheck(const QUuid &uuid, const QUuid &uuidDefault);

    //!
    //! \brief hashGenerator
    //! \return
    //!
    static const QByteArray hashGenerator();

    //!
    //! \brief randomGenerator
    //! \return
    //!
    static const QByteArray randomGenerator();

    //!
    //! \brief makeObjectName
    //! \param v
    //! \return
    //!
    static const QString makeObjectName(const QVariant &v);

    //!
    //! \brief toMap
    //! \return
    //!
    Q_INVOKABLE virtual QVariantMap toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    Q_INVOKABLE virtual QVariantHash toHash() const;

    //!
    //! \brief toVar
    //! \return
    //!
    Q_INVOKABLE virtual QVariant toVar() const;

    //!
    //! \brief fromVar
    //! \param map
    //! \return
    //!
    Q_INVOKABLE virtual bool fromVar(const QVariant &v);

    //!
    //! \brief fromMap
    //! \param map
    //! \return
    //!
    Q_INVOKABLE virtual bool fromMap(const QVariantMap &value);

    //!
    //! \brief fromMap
    //! \param map
    //! \return
    //!
    Q_INVOKABLE virtual bool fromHash(const QVariantHash &value);

    //!
    //! \brief storedProperty
    //! \return
    //!
    virtual ResultValue &storedProperty();

    //!
    //! \brief storedIsChanged
    //! \return
    //!
    Q_INVOKABLE virtual bool storedIsChanged() const;

    //!
    //! \brief storedMd5
    //! \return
    //!
    Q_INVOKABLE virtual QByteArray &storedMd5() const;

    //!
    //! \brief storedMd5Make
    //! \return
    //!
    Q_INVOKABLE virtual QByteArray storedMd5Make() const;


private:
    ObjectPvt *p=nullptr;
};

}

