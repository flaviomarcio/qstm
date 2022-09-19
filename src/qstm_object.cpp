#include "./qstm_object.h"
#include "./qstm_util_variant.h"
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QUuid>
#include <QDateTime>
#include <QUrl>

namespace QStm {

Q_GLOBAL_STATIC_WITH_ARGS(QByteArray,__static_uuid_base_data,(QUuid::createUuid().toString().toUtf8()))

static const auto __static_uuid_delimer="|";
static const auto __underline1="_";
static const auto __underline2="__";
static const auto __QMLTYPE="_QMLTYPE_";
static const auto __obj="obj";

class ObjectPvt:public QObject{
public:
    CachePool *_cachePool=nullptr;
    QByteArray storedMd5;
    ResultValue result;
    QObject *parent=nullptr;
    explicit ObjectPvt(QObject *parent):QObject{parent}, result{parent}
    {
        this->parent=parent;
    }


    CachePool &cachePool()
    {
        if(this->_cachePool==nullptr)
            this->_cachePool=&CachePool::instance();
        return*this->_cachePool;
    }

    static const QByteArray toMd5(const QVariant &value)
    {
        Q_DECLARE_VU;
        return vu.toMd5(value);
    }

    QByteArray storedMd5Make() const
    {
        auto &metaObject = *this->parent->metaObject();
        QVariantHash vBody;
        for(int col = 0; col < metaObject.propertyCount(); ++col) {
            auto property = metaObject.property(col);
            vBody[property.name()]=property.read(this->parent);
        }
        return this->toMd5(vBody);
    }

    template<class T>
    T toDictionary(bool nullValuesAdd)const
    {
        T __return;
        Q_DECLARE_VU;
        auto &metaObject = *this->parent->metaObject();
        for(int col = 0; col < metaObject.propertyCount(); ++col) {
            auto property = metaObject.property(col);
            QVariant value;
            switch (property.typeId()) {
            case QMetaType::User:
            case 65538/*CustomType*/:
                value=property.read(this).toInt();
                break;
            default:
                value=property.read(this);
            }
            if(!nullValuesAdd && vu.vIsEmpty(value))
                continue;
            __return.insert(property.name(), value);
        }
        return __return;
    }


};

Object::Object(QObject *parent):QObject{parent}
{
    this->p = new ObjectPvt(this);
}

CachePool &Object::cachePool()
{
    return p->cachePool();
}

ResultValue &Object::lr()const
{
    return p->result;
}

const QDateTime Object::now()
{
    return QDateTime::currentDateTime();
}

const QByteArray Object::toMd5(const QByteArray &value)
{
    return ObjectPvt::toMd5(value);
}

const QByteArray Object::toMd5(const QString &value)
{
    return ObjectPvt::toMd5(value);
}

const QByteArray Object::toMd5(const QVariant &value)
{
    return ObjectPvt::toMd5(value);
}

const QUuid Object::uuidGenerator()
{
    auto __uuid_base_bytes=*__static_uuid_base_data+
            __static_uuid_delimer+randomGenerator()+
            __static_uuid_delimer+
            QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch()).toUtf8()+
            __static_uuid_delimer;
    return QUuid::createUuidV5(QUuid::createUuid(), __uuid_base_bytes);
}

const QUuid Object::uuidGenerator(const QString &uuidBase)
{
    auto __uuid_base_bytes=*__static_uuid_base_data+
            __static_uuid_delimer+randomGenerator()+
            __static_uuid_delimer+QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch()).toUtf8()+
            __static_uuid_delimer+uuidBase;
    return QUuid::createUuidV5(QUuid::createUuid(), __uuid_base_bytes);
}

const QUuid Object::uuidGenCheck(const QUuid &uuid)
{
    return uuid.isNull()?uuidGenerator():uuid;
}

const QUuid Object::uuidGenCheck(const QUuid &uuid, const QUuid &uuidDefault)
{
    return !uuid.isNull()?uuid:(uuidDefault.isNull()?uuidGenerator():uuidDefault);
}

const QByteArray Object::hashGenerator()
{
    auto value=uuidGenerator().toString().toUtf8();
    return QCryptographicHash::hash(value, QCryptographicHash::Md5).toHex();
}

const QString Object::makeObjectName(const QVariant &v)
{
    static const auto listChar=QStringList{"(",")","."," ","{","}","-"};
    static int seedRandon=1;
    QString name=v.toString();
    if(name.contains(__QMLTYPE))
        name=name.split(__QMLTYPE).first().trimmed();

    if(name.isEmpty())
        name=__obj;
    name+=__underline2;
    {
        QRandomGenerator r;
        r.seed(++seedRandon);
        auto v=r.generate();
        name+=QString::number(v);
    }
    for(auto &v:listChar){
        name=name.replace(v, __underline1);
    }

    while(name.contains(__underline2))
        name=name.replace(__underline2, __underline1);
    return name;
}

const QByteArray Object::randomGenerator()
{
    static int seedRandon=1;
    QRandomGenerator r;
    r.seed(++seedRandon);
    auto v=r.generate();
    return QString::number(v).toUtf8();
}

QVariantMap Object::toMap()const
{
    return p->toDictionary<QVariantMap>(true);
}

QVariantHash Object::toHash() const
{
    return p->toDictionary<QVariantHash>(true);
}

QVariant Object::toVar()const
{
    return p->toDictionary<QVariantHash>(true);
}

bool Object::fromVar(const QVariant &v)
{
    QVariantHash vHash;
    switch (v.typeId()) {
    case QMetaType::QString:
    case QMetaType::QByteArray:
        vHash=QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash();
        break;
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        vHash=v.toHash();
        break;
    default:
        break;
    }
    return this->fromHash(vHash);
}

bool Object::fromMap(const QVariantMap&map)
{
    bool __return=false;
    auto &metaObject = *this->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        if(!property.write(this, map.value(property.name())))
            continue;
        __return=true;
    }
    return __return;
}

bool Object::fromHash(const QVariantHash &map)
{
    bool __return=false;
    auto &metaObject = *this->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        if(!property.write(this, map.value(property.name())))
            continue;
        __return=true;
    }
    return __return;
}

ResultValue &Object::storedProperty()
{
    p->storedMd5=this->storedMd5Make();
    return this->lr(p->storedMd5);
}

bool Object::storedIsChanged()const
{
    auto v1=this->storedMd5();
    auto v2=this->storedMd5Make();
    if(v1==v2)
        return false;
    return true;
}

QByteArray &Object::storedMd5()const
{
    return p->storedMd5;
}

QByteArray Object::storedMd5Make() const
{
    return p->storedMd5Make();
}

}

