#pragma once

#include <QObject>
#include <QLocale>
#include <QVariant>
#include <QDateTime>
#include <QVariantList>
#include <QVariantHash>
#include "./qstm_global.h"

#define Q_DECLARE_FU \
    QStm::FormattingUtil fu

namespace QStm {
class FormattingUtilPvt;
//!
//! \brief The FormattingUtil class
//!util class to format string values
class Q_STM_EXPORT FormattingUtil:public QVariant{
public:

    //!
    //! \brief The Masks class
    //!
    class Masks{
        friend class FormattingUtil;
    public:
        //!
        //! \brief Masks
        //!
        explicit Masks();

        //!
        //! \brief ~Masks
        //!
        virtual ~Masks();

        //!
        //! \brief date
        //! \return
        //!
        virtual QString&date() const;

        //!
        //! \brief time
        //! \return
        //!
        virtual QString&time() const;

        //!
        //! \brief timeShort
        //! \return
        //!
        virtual QString &timeShort() const;

        //!
        //! \brief dateTime
        //! \return
        //!
        virtual QString&dateTime() const;

        //!
        //! \brief currency
        //! \return
        //!
        virtual QString&currency() const;

        //!
        //! \brief number
        //! \return
        //!
        virtual QString&number() const;

        //!
        //! \brief numeric
        //! \return
        //!
        virtual QString&numeric() const;

        //!
        //! \brief percent
        //! \return
        //!
        virtual QString&percent() const;

        //!
        //! \brief boolean
        //! \param v
        //! \return
        //!
        virtual QString&boolean(const bool&v)const;
    private:
        FormattingUtilPvt *p=nullptr;
    };

    //!
    //! \brief FormattingUtil
    //! \param v
    //!
    explicit FormattingUtil(const QVariant &v={});

    //!
    //! \brief ~FormattingUtil
    //!
    virtual ~FormattingUtil();

    //!
    //! \brief operator =
    //! \param v
    //! \return
    //!
    FormattingUtil&operator=(const QVariant &v);

    //!
    //! \brief toDate
    //! \param v
    //! \return
    //!
    virtual const QString toDate(const QVariant &v={});

    //!
    //! \brief toTime
    //! \param v
    //! \return
    //!
    virtual const QString toTime(const QVariant &v={});

    //!
    //! \brief toTimeShort
    //! \param v
    //! \return
    //!
    const virtual QString toTimeShort(const QVariant &v);

    //!
    //! \brief toDateTime
    //! \param v
    //! \return
    //!
    virtual const QString toDateTime(const QVariant &v={});

    //!
    //! \brief toCurrency
    //! \param v
    //! \param prec
    //! \return
    //!
    virtual const QString toCurrency(const QVariant &v={}, int prec=-1);

    //!
    //! \brief toInt
    //! \param v
    //! \return
    //!
    virtual const QString toInt(const QVariant &v={});

    //!
    //! \brief toDouble
    //! \param v
    //! \param prec
    //! \return
    //!
    virtual const QString toDouble(const QVariant &v={}, int prec=-1);

    //!
    //! \brief toPercent
    //! \param v
    //! \param prec
    //! \return
    //!
    virtual const QString toPercent(const QVariant &v={}, int prec=-1);

    //!
    //! \brief toBool
    //! \param v
    //! \return
    //!
    virtual const QString toBool(const QVariant &v={});

    //!
    //! \brief toString
    //! \return
    //!
    virtual const QString toString();

    //!
    //! \brief v
    //! \param v
    //! \param prec
    //! \return
    //!
    virtual const QString v(const QVariant &v={}, int prec=-1);

    //!
    //! \brief currencySymbol
    //! \param v
    //! \return
    //!
    static const QString currencySymbol(const QVariant &v={});

    //!
    //! \brief masks
    //! \return
    //!
    virtual Masks&masks() const;

private:
    FormattingUtilPvt *p=nullptr;
};

}

typedef QStm::FormattingUtil FormattingUtil;

Q_DECLARE_METATYPE(FormattingUtil)
Q_DECLARE_METATYPE(FormattingUtil*)
