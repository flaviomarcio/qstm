#include "./qstm_currency.h"
#include "../3rdparty/bcmath/bcmath.h"
#include <QLocale>

namespace QStm {


class QCurrencyPvt{
public:
    QCurrency*parent=nullptr;
    QBCMath qbcmath;
    int precision=2;
    explicit QCurrencyPvt(QCurrency*parent): parent{parent}
    {
    }
    virtual ~QCurrencyPvt()
    {

    }
    auto &setValue(const QVariant &value)
    {
        auto &v=*dynamic_cast<QVariant*>(this->parent);
        this->qbcmath.setValue(value.toString());
        this->qbcmath.round(this->precision);
        v.setValue(qbcmath.toString());
        return *this->parent;
    }
};

QCurrency::QCurrency(const QVariant &v, const QVariant &precision):QVariant{v}, p{new QCurrencyPvt{this}}
{
    if(precision.isValid())
        p->precision=precision.toInt();
    p->setValue(v);
}

QCurrency::~QCurrency()
{
    delete this->p;
}

QCurrency &QCurrency::operator =(const qlonglong &v)
{

    return p->setValue(v);
}

QCurrency&QCurrency::operator =(const int &v)
{

    return p->setValue(v);
}

QCurrency &QCurrency::operator =(const double &v)
{

    return p->setValue(v);
}

QCurrency &QCurrency::operator +(const QCurrency &o)
{

    p->qbcmath+o.toDouble();
    p->setValue(p->qbcmath.toDouble());
    return *this;
}

QCurrency &QCurrency::operator -(const QCurrency &o) {

    p->qbcmath-o.toDouble();
    p->setValue(p->qbcmath.toDouble());
    return *this;
}

QCurrency &QCurrency::operator *(const QCurrency &o) {

    p->qbcmath*o.toDouble();
    p->setValue(p->qbcmath.toDouble());
    return *this;
}

QCurrency &QCurrency::operator /(const QCurrency &o) {

    p->qbcmath/o.toDouble();
    p->setValue(p->qbcmath.toDouble());
    return *this;
}

QCurrency &QCurrency::operator %(const QCurrency &o) {

    p->qbcmath%o.toDouble();
    p->setValue(p->qbcmath.toDouble());
    return *this;
}

QCurrency &QCurrency::operator ^(const QCurrency &o) {

    p->qbcmath^o.toDouble();
    p->setValue(p->qbcmath.toDouble());
    return *this;
}

void QCurrency::operator+=(const QVariant &o) {

    p->qbcmath+=o.toDouble();
    p->setValue(p->qbcmath.toDouble());
}

void QCurrency::operator-=(const QVariant &o) {

    p->qbcmath-=o.toDouble();
    p->setValue(p->qbcmath.toDouble());
}

void QCurrency::operator*=(const QVariant &o) {

    p->qbcmath*=o.toDouble();
    p->setValue(p->qbcmath.toDouble());
}

void QCurrency::operator/=(const QVariant &o) {

    p->qbcmath/=o.toDouble();
    p->setValue(p->qbcmath.toDouble());
}

void QCurrency::operator^=(const QVariant &o) {

    p->qbcmath^=o.toDouble();
    p->setValue(p->qbcmath.toDouble());
}

bool QCurrency::operator >(const QCurrency &o) {

    return p->qbcmath>o.toDouble();
}

bool QCurrency::operator >=(const QCurrency &o) {

    return p->qbcmath>=o.toDouble();
}

bool QCurrency::operator ==(const QCurrency &o) {

    return p->qbcmath==o.toDouble();
}

bool QCurrency::operator <(const QCurrency &o) {

    return p->qbcmath<o.toDouble();
}

bool QCurrency::operator <=(const QCurrency &o) {

    return p->qbcmath<=o.toDouble();
}

QCurrency &QCurrency::setValue(const QVariant &value)
{

    return p->setValue(value);
}

double QCurrency::toDouble() const
{

    return p->qbcmath.toDouble();
}

}
