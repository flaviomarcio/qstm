#pragma once

#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QUuid>
#include <QVariant>
#include "./qstm_global.h"


#define Q_DECLARE_HU QStm::HashUtil hu

namespace QStm {
class HashUtilPvt;
//!
//! \brief The HashUtil class
//!
class Q_STM_EXPORT HashUtil:public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit HashUtil(QObject *parent=nullptr);

    //!
    //! \brief isUuid
    //! \param v
    //! \return
    //!
    static bool isUuid(const QVariant &v);

    //!
    //! \brief isUuid
    //! \param v
    //! \param uuidSet
    //! \return
    //!
    static bool isUuid(const QVariant &v, QUuid &uuidSet);

    //!
    //! \brief isHex
    //! \param v
    //! \return
    //!
    static bool isHex(const QVariant &v);

    //!
    //! \brief isBase64
    //! \param v
    //! \return
    //!
    static bool isBase64(const QVariant &v);

    //! se for um QVariantHash ou QVariantList convertera para json para entao tirar o md5
    //! se nao for md5 sera tirado o md5 dos bytes
    //! se nao for uuid um md5 sera convertido em string para md5
    //! se a string enviada for um md ou md5uui entao nada ocorrera retornando o md5 ja existe nao gerando outro
    static QByteArray toMd5(const QVariant &v);

    //!
    //! \brief toHex
    //! \param v
    //! \return
    //!
    static QByteArray toHex(const QVariant &v);

    //!
    //! \brief toUuid
    //! \param v
    //! \return
    //! se for md5 sera convertido para uuidMd5
    //! se for uuid nada ocorrera retornando o uuid
    static QUuid toUuid(const QVariant &v);

    //!
    //! \brief toUuidSimple
    //! \param v
    //! \return
    //! remove {} do uuid gerado pelo qt
    static QString toUuidSimple(const QVariant &v);

    //!
    //! \brief toMd5Uuid
    //! \param v
    //! \return
    //!se a string enviada for um md ou mduui entao nada ocorrera retornando o md5 ja existe nao gerando outro
    static QUuid toMd5Uuid(const QVariant &v);

    //!
    //! \brief toUuidCompuser
    //! \param value
    //! \return
    //!
    static QUuid toUuidCompuser(const QVariant &value);

private:
    HashUtilPvt *p=nullptr;
};

}
