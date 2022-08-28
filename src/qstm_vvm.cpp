#include "./qstm_vvm.h"
#include "./qstm_util_variant.h"
#include <QJsonDocument>

const static QString toBytes(const QVariant &v)
{
    switch (v.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
    case QMetaType::QUuid:
        return v.toUuid().toString();
    case QMetaType::QUrl:
        return v.toUrl().toString();
    default:
        return v.toByteArray();
    }
}

const static QVariant toVariant(const QVariant &v)
{
    switch (v.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
    case QMetaType::QUuid:
        return v.toUuid().toString();
    case QMetaType::QUrl:
        return v.toUrl().toString();
    default:
        return v;
    }
}

QVVM::QVVM():QVariantHash{}
{
}

QVVM::QVVM(const QVariant &other):QVariantHash{other.toHash()}
{
}

QVVM::QVVM(const QVariant &key, const QVariant &value):QVariantHash{}
{
    QVariantHash::insert(toBytes(key), toVariant(value));
}

QVVM &QVVM::operator=(const QVariant &v)
{
    Q_DECLARE_VU;
    auto vMap=vu.toHash(v);
    QVariantHash::clear();
    QHashIterator<QString, QVariant> i(vMap);
    while (i.hasNext()) {
        i.next();
        this->insert(i.key(), i.value());
    }
    return *this;
}

QVVM &QVVM::operator+=(const QVariantHash &v)
{
    QHashIterator<QString, QVariant> i(v);
    while (i.hasNext()) {
        i.next();
        this->insert(i.key(), i.value());
    }
    return *this;
}

QVVM &QVVM::operator+=(const QVariantMap &v)
{
    QMapIterator<QString, QVariant> i(v);
    while (i.hasNext()) {
        i.next();
        this->insert(i.key(), i.value());
    }
    return *this;
}

QVVM QVVM::from(const QVariantHash &v)
{
    QVVM vvm;
    vvm+=v;
    return vvm;
}

QVVM QVVM::from(const QVariantMap &v)
{
    QVVM vvm;
    vvm+=v;
    return vvm;
}

QVVM &QVVM::insert(const QVariant &key, const QVariant &value)
{
    QVariantHash::insert(toBytes(key), toVariant(value));
    return *this;
}

QVVM &QVVM::remove(const QVariant &key)
{
    QVariantHash::remove(toBytes(key));
    return *this;
}

QVariant QVVM::value(const QVariant &key) const
{
    return QVariantHash::value(toBytes(key));
}
