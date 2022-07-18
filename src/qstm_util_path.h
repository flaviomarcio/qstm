#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QVariantHash>
#include "./qstm_global.h"

namespace QStm {
class PathUtilPvt;
class Q_STM_EXPORT PathUtil{
public:
    explicit PathUtil(const QVariant &v={});
    virtual ~PathUtil();

    PathUtil &operator=(const QVariant &v);
    PathUtil &operator+=(const QVariant &v);
    PathUtil &operator-=(const QVariant &v);
    PathUtil &operator<<(const QVariant &v);

    //!
    //! \brief exists
    //! \return
    //!
    bool exists()const;

    //!
    //! \brief exists
    //! \param v
    //! \return
    //!
    bool exists(const QVariant &v={})const;

    //!
    //! \brief isValid
    //! \param v
    //! \return
    //!
    bool isValid(const QVariant &v={})const;

    //!
    //! \brief clear
    //! \return
    //!
    PathUtil &clear();

    //!
    //! \brief setPath
    //! \param v
    //! \return
    //!
    PathUtil &setPath(const QVariant &v={});

    //!
    //! \brief append
    //! \param v
    //! \return
    //!
    PathUtil &append(const QVariant &v={});

    //!
    //! \brief remove
    //! \param v
    //! \return
    //!
    PathUtil &remove(const QVariant &v={});

    //!
    //! \brief mkPath
    //! \param v
    //! \return
    //!
    bool mkPath(const QVariant &v={});

    //!
    //! \brief toLower
    //! \return
    //!
    QString toLower()const;

    //!
    //! \brief toString
    //! \return
    //!
    QString toString()const;

    //!
    //! \brief toByteArray
    //! \return
    //!
    QByteArray toByteArray()const;

    //!
    //! \brief toList
    //! \return
    //!
    QVariantList toList()const;

    //!
    //! \brief toStringList
    //! \return
    //!
    QStringList toStringList()const;

    //!
    //! \brief toMd5
    //! \return
    //!
    QByteArray toMd5()const;

    //!
    //! \brief toUuid
    //! \return
    //!
    QUuid toUuid()const;

    //!
    //! \brief toUrl
    //! \return
    //!
    QUrl toUrl();

    //!
    //! \brief arg
    //! \param v
    //! \return
    //!
    QString arg(const QVariant &v);

private:
    PathUtilPvt *p=nullptr;
};

}

typedef QStm::PathUtil PathUtil;
