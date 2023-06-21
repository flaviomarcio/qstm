#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QVariantHash>
#include <QVariantList>
#include <QByteArray>
#include "./qstm_global.h"

namespace QStm {

#define Q_DECLARE_MU QStm::MapUtil du

class MapUtilPvt;

//!
//! \brief The MapUtil class
//!
class Q_STM_EXPORT MapUtil : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief MapUtil
    //! \param sourceObject
    //!
    Q_INVOKABLE explicit MapUtil(QObject *sourceObject = nullptr);
    explicit MapUtil(const QVariant &sourceValues, QObject *parent = nullptr);

    //!
    //! \brief isEmpty
    //! \param keys
    //! \return
    //!
    bool isEmpty();

    //!
    //! \brief count
    //! \return
    //!
    bool count();

    //!
    //! \brief contains
    //! \param key
    //! \return
    //!
    bool contains(const QVariant &keys);

    //!
    //! \brief isMap
    //! \param keys
    //! \return
    //!
    bool isMap(const QVariant &value);

    //!
    //! \brief clear
    //! \return
    //!
    MapUtil &clear();

    //!
    //! \brief value
    //! \param key
    //! \return
    //!
    QVariant value(const QVariant &key) const;

    //!
    //! \brief valueFirst
    //! \return
    //!
    QVariant valueFirst() const;

    //!
    //! \brief valueLast
    //! \return
    //!
    QVariant valueLast() const;

    //!
    //! \brief values
    //! \param value
    //! \return
    //!
    QVariantList values();
    QVariantList values(const QVariant &keys);

    //!
    //! \brief take
    //! \return
    //!
    QVariant take(const QVariant &key);

    //!
    //! \brief takeValues
    //! \param keys
    //! \return
    //!
    QVariantList takeValues(const QVariant &keys);

    //!
    //! \brief takeFirst
    //! \return
    //!
    QVariant takeFirst()const;

    //!
    //! \brief takeLast
    //! \return
    //!
    QVariant takeLast()const;

    //!
    //! \brief append
    //! \param value
    //! \return
    //!
    MapUtil &append(const QVariant &value);
    MapUtil &append(QObject *value);
    MapUtil &append(const QString &key, const QVariant &value);

    //!
    //! \brief remove
    //! \param key
    //! \return
    //!
    MapUtil &remove(const QVariant &keys);

    //!
    //! \brief toHash
    //! \return
    //!
    const QVariantHash toHash()const;

    //!
    //! \brief toMap
    //! \return
    //!
    const QVariantMap toMap()const;

    //!
    //! \brief toList
    //! \return
    //!
    const QVariantList toList()const;

    //!
    //! \brief toVariant
    //! \return
    //!
    const QVariant toVariant()const;

    //!
    //! \brief toJson
    //! \return
    //!
    const QByteArray toJson() const;

    //!
    //! \brief toMd5
    //! \return
    //!
    const QByteArray toMd5() const;

    //!
    //! \brief toMd5Uuid
    //! \return
    //!
    const QUuid toMd5Uuid() const;
private:
    MapUtilPvt *p=nullptr;
};

} // namespace QStm

