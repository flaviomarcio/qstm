#pragma once

#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QUrl>
#include <QDateTime>
#include <QVariantList>
#include <QVariantHash>
#include "./qstm_global.h"

#define Q_DECLARE_VU QStm::VariantUtil vu

namespace QStm {
class VariantUtilPvt;
//!
//! \brief The VariantUtil class
//!
class Q_STM_EXPORT VariantUtil:public QVariant{
public:
    explicit VariantUtil(const QVariant &v={});
    virtual ~VariantUtil();
    //!
    //! \brief operator =
    //! \param v
    //! \return
    //!
    VariantUtil&operator=(const QVariant &v);

    //!
    //! \brief isUuid
    //! \param v
    //! \return
    //!
    virtual bool isUuid(const QVariant &v={}) const;

    //!
    //! \brief isUuid
    //! \param v
    //! \param uuidSet
    //! \return
    //!
    virtual bool isUuid(const QVariant &v, QUuid &uuidSet) const;

    //!
    //! \brief isHex
    //! \param v
    //! \return
    //!
    virtual bool isHex(const QVariant &v={}) const;

    //!
    //! \brief isBase64
    //! \param v
    //! \return
    //!
    virtual bool isBase64(const QVariant &v={}) const;

    //!
    //! \brief toStr
    //! \param v
    //! \return
    //!
    virtual const QString toStr(const QVariant &v={});

    //!
    //! \brief toAlphaNumber
    //! \param v
    //! \return
    //!
    virtual const QByteArray toAlphaNumber(const QVariant &v={});

    //!
    //! \brief toAlphaNumeric
    //! \param v
    //! \return
    //!
    virtual const QByteArray toAlphaNumeric(const QVariant &v={});


    //!
    //! \brief toAlphaNumeric
    //! \param v
    //! \return
    //!
    virtual const QString toAlphaText(const QVariant &v={});

    //!
    //! \brief toByteArray
    //! \param v
    //! \return
    //!
    virtual const QByteArray toByteArray(const QVariant &v={});

    //!
    //! \brief toChar
    //! \param v
    //! \return
    //!
    virtual const QChar toChar(const QVariant &v={});

    //!
    //! \brief toInt
    //! \param v
    //! \return
    //!
    virtual int toInt(const QVariant &v={});

    //!
    //! \brief toLongLong
    //! \param v
    //! \return
    //!
    virtual qlonglong toLongLong(const QVariant &v={});

    //!
    //! \brief toDate
    //! \param v
    //! \return
    //!
    virtual const QDate toDate(const QVariant &v={});

    //!
    //! \brief toTime
    //! \param v
    //! \return
    //!
    virtual const QTime toTime(const QVariant &v={});

    //!
    //! \brief toDateTime
    //! \param v
    //! \return
    //!
    virtual const QDateTime toDateTime(const QVariant &v={});

    //!
    //! \brief toDouble
    //! \param v
    //! \return
    //!
    virtual double toDouble(const QVariant &v={});

    //!
    //! \brief toBool
    //! \param v
    //! \return
    //!
    virtual bool toBool(const QVariant &v={});

    //!
    //! \brief canConvertJson
    //! \param v
    //! \return
    //!
    virtual bool canConvertJson(const QVariant &v={})const;

    //!
    //! \brief canConvertJson
    //! \param v
    //! \param vOut
    //! \return
    //!
    virtual bool canConvertJson(const QVariant &v, QVariant &vOut)const;

    //!
    //! \brief toMd5
    //! \param v
    //! \return
    //! se for um QVariantHash ou QVariantList convertera para json para entao tirar o md5
    //! se nao for md5 sera tirado o md5 dos bytes
    //! se nao for uuid um md5 sera convertido em string para md5
    //! se a string enviada for um md ou md5uui entao nada ocorrera retornando o md5 ja existe nao gerando outro
    virtual const QByteArray toMd5(const QVariant &v={});

    //!
    //! \brief toHex
    //! \param v
    //! \return
    //!
    virtual const QByteArray toHex(const QVariant &v={});

    //!
    //! \brief toUuid
    //! \param v
    //! \return
    //! se for md5 sera convertido para uuidMd5
    //! se for uuid nada ocorrera retornando o uuid
    virtual const QUuid toUuid(const QVariant &v={});

    //!
    //! \brief toUuidSimple
    //! \param v
    //! \return
    //! remove {} do uuid gerado pelo qt
    virtual const QString toUuidSimple(const QVariant &v={});

    //!
    //! \brief toMd5Uuid
    //! \param v
    //! \return
    //!se a string enviada for um md ou mduui entao nada ocorrera retornando o md5 ja existe nao gerando outro
    virtual const QUuid toMd5Uuid(const QVariant &v={});

    //!
    //! \brief takeList
    //! \param keyName
    //! \return
    //!
    virtual const QVariantList takeList(const QByteArray &keyName);

    //!
    //! \brief toStringList
    //! \return
    //!
    virtual const QStringList toStringList();

    //!
    //! \brief toStringList
    //! \param v
    //! \return
    //!
    virtual const QStringList toStringList(const QVariant &v);

    //!
    //! \brief toList
    //! \param v
    //! \return
    //!
    virtual const QVariantList toList(const QVariant &v={});

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantMap toMap()const;

    //!
    //! \brief toMap
    //! \param v
    //! \return
    //!
    virtual const QVariantMap toMap(const QVariant &v);

    //!
    //! \brief toMap
    //! \param key
    //! \param value
    //! \return
    //!
    virtual const QVariantMap toMap(const QVariant &key, const QVariant &value);

    //!
    //! \brief toPair
    //! \return
    //!
    virtual QVariantPair toPair() const;

    //!
    //! \brief toPair
    //! \param v
    //! \return
    //!
    virtual const QVariantPair toPair(const QVariant &v);

    //!
    //! \brief toPair
    //! \param key
    //! \param value
    //! \return
    //!
    virtual const QVariantPair toPair(const QVariant &key, const QVariant &value);


    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash toHash() const;

    //!
    //! \brief toHash
    //! \param v
    //! \return
    //!
    virtual const QVariantHash toHash(const QVariant &v);

    //!
    //! \brief toHash
    //! \param key
    //! \param value
    //! \return
    //!
    virtual const QVariantHash toHash(const QVariant &key, const QVariant &value);

    //!
    //! \brief toMultiHash
    //! \return
    //!
    virtual QMultiHash<QString, QVariant> toMultiHash() const;

    //!
    //! \brief toMultiHash
    //! \param v
    //! \return
    //!
    const virtual QMultiHash<QString, QVariant> toMultiHash(const QVariant &v);

    //!
    //! \brief toMultiHash
    //! \param key
    //! \param value
    //! \return
    //!
    const virtual QMultiHash<QString, QVariant> toMultiHash(const QVariant &key, const QVariant &value);

    //!
    //! \brief toAttributes
    //! \param v
    //! \param attributeName
    //! \return
    //!
    const QVariantHash toAttributes(const QVariant &v, const QVariant &attributeNames={});

    //!
    //! \brief toType
    //! \param type
    //! \param v
    //! \return
    //!
    const QVariant toType(int typeId, const QVariant &v={});

    //!
    //! \brief toVariant
    //! \param v
    //! \return
    //!
    virtual const QVariant toVariant(const QVariant &v);

    //!
    //! \brief toVariantObject
    //! \param v
    //! \return
    //!
    virtual const QVariant toVariantObject(const QVariant &v);

    //!
    //! \brief toVariantJson
    //! \param v
    //! \return
    //!
    virtual const QVariant toVariantJson(const QVariant &v);

    //!
    //! \brief toUrl
    //! \param v
    //! \return
    //!
    virtual const QUrl toUrl(const QVariant &v={});

    //!
    //! \brief makeMap
    //! \param key
    //! \param value
    //! \return
    //!
    virtual VariantUtil &makeMap(const QVariant &key, const QVariant &value);

    //!
    //! \brief mMap
    //! \param key
    //! \param value
    //! \return
    //!
    virtual VariantUtil &mMap(const QVariant &key, const QVariant &value);

    //!
    //! \brief makeHash
    //! \param key
    //! \param value
    //! \return
    //!
    virtual VariantUtil &makeHash(const QVariant &key, const QVariant &value);

    //!
    //! \brief mHash
    //! \param key
    //! \param value
    //! \return
    //!
    virtual VariantUtil &mHash(const QVariant &key, const QVariant &value);

    //!
    //! \brief makeList
    //! \param value
    //! \return
    //!
    virtual VariantUtil &makeList(const QVariant &value);

    //!
    //! \brief mList
    //! \param value
    //! \return
    //!
    virtual VariantUtil &mList(const QVariant &value);

    //!
    //! \brief toUuidCompuser
    //! \param value
    //! \return
    //!
    virtual const QUuid toUuidCompuser(const QVariant &value);

    //!
    //! \brief clear
    //! \return
    //!
    virtual VariantUtil &clear();

    //!
    //! \brief vUnion
    //! \param v
    //! \return
    //!
    virtual QVariant vUnion(const QVariant &v);

    //!
    //! \brief vUnion
    //! \param vDestine
    //! \param vSource
    //! \return
    //!
    virtual QVariant vUnion(const QVariant &vDestine, const QVariant &vSource);

    //!
    //! \brief vMerge
    //! \param v
    //! \return
    //!
    virtual QVariant vMerge(const QVariant &v);

    //!
    //! \brief vMerge
    //! \param vDestine
    //! \param vSource
    //! \return
    //!
    virtual QVariant vMerge(const QVariant &vDestine, const QVariant &vSource);

    //!
    //! \brief vDeduplicate
    //! \param v
    //! \return
    //!
    virtual QVariant vDeduplicate(const QVariant &v);

    //!
    //! \brief vIsEmpty
    //! \param v
    //! \return
    //!
    virtual bool vIsEmpty(const QVariant &v={});

    //!
    //! \brief vIsObject
    //! \param v
    //! \return
    //!
    virtual bool vIsObject(const QVariant &v={});

    //!
    //! \brief vIsList
    //! \param v
    //! \return
    //!
    virtual bool vIsList(const QVariant &v={});

    //!
    //! \brief vIsMap
    //! \param v
    //! \return
    //!
    virtual bool vIsMap(const QVariant &v={});

    //!
    //! \brief vIsString
    //! \param v
    //! \return
    //!
    virtual bool vIsString(const QVariant &v={});

    //!
    //! \brief convertTo
    //! \param v
    //! \param typeId
    //! \return
    //!
    virtual QVariant convertTo(const QVariant &v, int typeId);

private:
    VariantUtilPvt *p=nullptr;
};

}

