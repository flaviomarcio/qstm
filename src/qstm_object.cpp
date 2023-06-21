#include "./qstm_object.h"
#include "./qstm_util_hash.h"
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
    const QByteArray __objectName=QByteArrayLiteral("objectName");
    QObject *parent=nullptr;
    ResultValue result;
    CachePool *cachePool=nullptr;
    QByteArray storedMd5;
    explicit ObjectPvt(QObject *parent)
        :
        QObject{parent},
        parent{parent},
        result{parent},
        cachePool{&CachePool::instance()}
    {
    }

    QByteArray storedMd5Make() const
    {
        auto value=toDictionary<QVariantHash>(true);
        Q_DECLARE_HU;
        return hu.toMd5(value);
    }

    template<class T>
    T toDictionary(bool nullValuesAdd)const
    {
        T __return;
        Q_DECLARE_VU;
        auto &metaObject = *this->parent->metaObject();
        for(int col = 0; col < metaObject.propertyCount(); ++col) {
            auto property = metaObject.property(col);
            if(!property.isReadable())
                continue;

            if(property.name()==__objectName)
                continue;

            QVariant value;
            switch (property.typeId()) {
            case QMetaType::User:
            case 65538/*CustomType*/:
                value=property.read(this->parent).toInt();
                break;
            default:
                value=property.read(this->parent);
            }

            if(value.isNull())
                continue;

            switch (value.typeId()) {
            case QMetaType::Bool:
                if(!value.toBool())
                    continue;
                break;
            case QMetaType::QUuid:
                if(value.toUuid().isNull())
                    continue;
                break;
            case QMetaType::QUrl:
                if(value.toUrl().isEmpty())
                    continue;
                break;
            case QMetaType::QDate:
                if(value.toDateTime().isNull())
                    continue;
                break;
            case QMetaType::QTime:
                if(value.toTime().isNull())
                    continue;
                break;
            case QMetaType::QDateTime:
                if(value.toDateTime().isNull())
                    continue;
                break;
            case QMetaType::QString:
            case QMetaType::QByteArray:
            case QMetaType::QChar:
            case QMetaType::QBitArray:
                if(value.toString().trimmed().isEmpty())
                    continue;
                break;
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::Long:
            case QMetaType::LongLong:
            case QMetaType::ULong:
            case QMetaType::ULongLong:
            case QMetaType::Double:
                if(value.toDouble()==0)
                    continue;
                break;
            case QMetaType::QVariantList:
            case QMetaType::QStringList:
                if(value.toList().isEmpty())
                    continue;
                break;
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantPair:
                if(value.toHash().isEmpty())
                    continue;
                break;
            default:
                break;
            }

            if(!nullValuesAdd && vu.vIsEmpty(value))
                continue;
            __return.insert(property.name(), value);
        }
        return __return;
    }

    bool fromHash(const QVariantHash &value)
    {
        bool __return=false;
        auto &metaObject = *this->parent->metaObject();
        Q_DECLARE_VU;
        static const auto __objectName=QByteArrayLiteral("objectName");
        for(int col = 0; col < metaObject.propertyCount(); ++col) {
            auto property = metaObject.property(col);
            if(!property.isWritable())
                continue;

            if(property.name()==__objectName)
                continue;

            auto valueSet=value.contains(property.name())
                                ?value.value(property.name())
                                :value.value(QByteArray(property.name()).toLower());
            if(valueSet.isNull() || !valueSet.isValid()){
                if(property.isResettable())
                    property.reset(this->parent);
                else
                    property.write(this->parent, {});
            }
            else{
                valueSet=vu.toType(property.typeId(), valueSet);
                if(!property.write(this->parent, valueSet))
                    continue;
                __return=true;
            }
        }
        return __return;
    }

};

Object::Object(QObject *parent):QObject{parent}, p{new ObjectPvt(this)}
{

}

CachePool &Object::cachePool()
{
    return *p->cachePool;
}

ResultValue &Object::lr()const
{
    return p->result;
}

const QDateTime Object::now()
{
    return QDateTime::currentDateTime();
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

QVariantHash Object::toHash() const
{
    return p->toDictionary<QVariantHash>(false);
}

QVariant Object::toVar()const
{
    return p->toDictionary<QVariantHash>(false);
}

QByteArray Object::toJson() const
{
    return QJsonDocument::fromVariant(this->toHash()).toJson(QJsonDocument::Compact);
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
    return p->fromHash(vHash);
}

bool Object::fromMap(const QVariantMap &value)
{
    return p->fromHash(QVariant{value}.toHash());
}

bool Object::fromHash(const QVariantHash &value)
{
    return p->fromHash(value);
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

