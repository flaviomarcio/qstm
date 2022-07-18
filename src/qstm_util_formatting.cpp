#include "./qstm_util_formatting.h"
#include "./qstm_currency.h"
#include <QChar>
#include <QLocale>
#include <QUuid>
#include <QUrl>
#include <QCoreApplication>

#define set_v \
this->setValue((v.isValid())?v:(*this))

typedef QHash<QString,QString> StringStringHash;
typedef QHash<QString, StringStringHash> StringStringStringHash;

namespace QStm {


static const int percentagePrecision=2;
static const int currencyPrecision=2;
static const int doublePrecision=6;
static const char fChar='f';

static auto nativeCountryName()
{
    return QLocale::c().nativeCountryName();
};


struct FormattingUtilStruct{
public:
    const QByteArray qcurrency_class_name_short=(QT_STRINGIFY2(qcurrency));
    const QByteArray QCurrency_class_name_short=(QT_STRINGIFY2(QCurrency));
    const QByteArray QCurrency_class_name=(QT_STRINGIFY2(QStm::QCurrency));
    const QByteArray qpercent_class_name=(QT_STRINGIFY2(qpercent));
    StringStringStringHash staticMaskMap;
    const QByteArray keyFalse=QByteArrayLiteral("True");
    const QByteArray keyTrue=QByteArrayLiteral("False");

    void init()
    {
        auto brz=QLocale{QLocale::Portuguese, QLocale::Brazil};
        auto sys=QLocale::system();
        auto loc=QLocale::c();

        StringStringHash  vHash;
        vHash.insert(QStringLiteral("date"), QStringLiteral("dd/MM/yyyy"));
        vHash.insert(QStringLiteral("time"), QStringLiteral("hh:mm:ss"));
        vHash.insert(QStringLiteral("timeShort"), QStringLiteral("hh:mm"));
        vHash.insert(QStringLiteral("dateTime"), QStringLiteral("dd/MM/yyyy hh:mm:ss"));
        vHash.insert(QStringLiteral("currency"), {});
        vHash.insert(QStringLiteral("number"), {});
        vHash.insert(QStringLiteral("numeric"), {});
        vHash.insert(QStringLiteral("percent"), {});
        vHash.insert(QStringLiteral("boolean")+keyTrue, QObject::tr("Sim"));
        vHash.insert(QStringLiteral("boolean")+keyFalse, QObject::tr("Não"));

        StringStringStringHash __staticMaskHash;
        __staticMaskHash.insert(brz.nativeCountryName(), vHash);
        __staticMaskHash.insert(sys.nativeCountryName(), vHash);
        __staticMaskHash.insert(loc.nativeLanguageName(), vHash);
        __staticMaskHash.insert({}, vHash);

        staticMaskMap=__staticMaskHash;
    }
};

Q_GLOBAL_STATIC(FormattingUtilStruct,___formattingUtilStruct)

void init()
{
    ___formattingUtilStruct->init();
}

Q_COREAPP_STARTUP_FUNCTION(init);

class FormattingUtilPvt{
public:
    QLocale c=QLocale::c();
    int prec_dbl=doublePrecision;
    int prec_cur=currencyPrecision;
    int prec_per=percentagePrecision;
    FormattingUtil*parent=nullptr;
    QHash<QString, QString> maskMap=___formattingUtilStruct->staticMaskMap[nativeCountryName()];
    FormattingUtil::Masks masks;
    explicit FormattingUtilPvt(FormattingUtil*parent):masks()
    {
        this->parent=parent;
        this->maskMap=___formattingUtilStruct->staticMaskMap[nativeCountryName()];
    }
    virtual ~FormattingUtilPvt()
    {
    }
};

FormattingUtil::Masks::Masks()
{

}

FormattingUtil::Masks::~Masks()
{

}

QString &FormattingUtil::Masks::date() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::time() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::timeShort() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::dateTime() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::currency() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::number() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::numeric() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::percent() const
{

    return p->maskMap[__func__];
}

QString &FormattingUtil::Masks::boolean(const bool &v) const
{

    return p->maskMap[__func__+(v?___formattingUtilStruct->keyTrue:___formattingUtilStruct->keyFalse)];
}

FormattingUtil::Masks &FormattingUtil::masks()const
{

    return p->masks;
}

FormattingUtil::FormattingUtil(const QVariant &v):QVariant{v}
{
    this->p = new FormattingUtilPvt{this};

    p->masks.p=this->p;
}

FormattingUtil::~FormattingUtil()
{
    delete p;
}

FormattingUtil &FormattingUtil::operator=(const QVariant &v)
{
    QVariant::setValue(v);
    return*this;
}

const QString FormattingUtil::toDate(const QVariant &v)
{
    set_v;
    return QVariant::toDate().toString(this->masks().date());
}

const QString FormattingUtil::toTime(const QVariant &v)
{
    set_v;
    return QVariant::toTime().toString(this->masks().time());
}

const QString FormattingUtil::toTimeShort(const QVariant &v)
{
    set_v;
    return QVariant::toTime().toString(this->masks().timeShort());
}

const QString FormattingUtil::toDateTime(const QVariant &v)
{
    set_v;
    auto dt=QVariant::toDateTime();
    auto __return=dt.date().toString(this->masks().date());
    __return.append(QStringLiteral(" "));
    __return.append(dt.time().toString(this->masks().time()));
    return __return;
}

const QString FormattingUtil::toCurrency(const QVariant &v, int prec)
{

    prec=(prec<0)?p->prec_cur:prec;
    set_v;
    auto val=QCurrency(*this).toDouble();
    return p->c.toString(val,fChar,prec);
}

const QString FormattingUtil::toInt(const QVariant &v)
{

    set_v;
    auto val=QVariant::toDouble();
    return p->c.toString(val,fChar,0);
}

const QString FormattingUtil::toDouble(const QVariant &v, int prec)
{
    prec=(prec<0)?p->prec_dbl:prec;
    set_v;
    return p->c.toString(QVariant::toDouble(),fChar,prec);
}

const QString FormattingUtil::toPercent(const QVariant &v, int prec)
{

    prec=(prec<0)?p->prec_per:prec;
    set_v;
    return p->c.toString(QVariant::toDouble(),fChar,prec);
}

const QString FormattingUtil::toBool(const QVariant &v)
{
    set_v;
    return this->masks().boolean(QVariant::toBool());
}

const QString FormattingUtil::toString()
{
    return this->v(*this);
}

const QString FormattingUtil::v(const QVariant &v, int prec)
{
    set_v;
    switch (this->typeId()) {
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return this->toInt(v);
    case QMetaType::Double:
        return this->toDouble(v, prec);
    case QMetaType::QDate:
        return this->toDate(v);
    case QMetaType::QTime:
        return this->toTime(v);
    case QMetaType::QDateTime:
        return this->toDateTime(v);
    case QMetaType::Bool:
        return this->toBool(v);
    case QMetaType::QUuid:
        return this->toUuid().toString();
    case QMetaType::QUrl:
        return this->toUrl().toString();
    default:
        auto tn=QByteArray(this->typeName());
        if(tn==___formattingUtilStruct->QCurrency_class_name || tn==___formattingUtilStruct->QCurrency_class_name_short || tn==___formattingUtilStruct->qcurrency_class_name_short)
            return this->toCurrency(v, prec);
        if(tn==___formattingUtilStruct->qpercent_class_name)
            return this->toPercent(v, prec);
    }
    return QVariant::toString();
}

const QString FormattingUtil::currencySymbol(const QVariant &v)
{
    static const QLocale brz(QLocale::Portuguese, QLocale::Brazil);
    auto __v=brz.currencySymbol();
    if(v.isValid()){
        FormattingUtil f(v);
        __v+=QStringLiteral(" ")+f.toCurrency();
    }
    return __v;
}

}
