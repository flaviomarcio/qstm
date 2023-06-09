#include "./qstm_util_meta_object.h"
#include "./qstm_macro.h"
#include <QCoreApplication>
#include <QUuid>
#include <QUrl>
#include <QBitArray>
#include <QStringList>
#include <QJsonDocument>
#include <QDateTime>

namespace QStm {

class MetaObjectUtilPvt{
public:
    MetaObjectUtil*parent=nullptr;
    QList<const QMetaObject*> metaObjectList;
    explicit MetaObjectUtilPvt(MetaObjectUtil*parent)
    {
        this->parent=parent;
    }
    virtual ~MetaObjectUtilPvt()
    {
    }

    bool writeProperty(QObject*object, const QMetaProperty&property, const QVariant &value)
    {
        QVariant vValue=value;

        if(property.write(object, vValue))
            return true;

        switch (property.typeId()) {
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QChar:
        case QMetaType::QBitArray:{
            switch (property.typeId()) {
            case QMetaType::QVariantMap:
            case QMetaType::QVariantHash:
            case QMetaType::QVariantList:
            case QMetaType::QStringList:
                vValue=QJsonDocument::fromVariant(vValue).toJson(QJsonDocument::Compact);
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }

        switch (property.typeId()) {
        case QMetaType::QUrl:
            if(property.write(object, vValue.toUrl()))
                return true;
            break;
        case QMetaType::QUuid:
            if(property.write(object, vValue.toUuid()))
                return true;
            break;
        case QMetaType::QString:
            if(property.write(object, vValue.toString()))
                return true;
            break;
        case QMetaType::QByteArray:
            if(property.write(object, vValue.toByteArray()))
                return true;
            break;
        case QMetaType::QChar:
            if(property.write(object, vValue.toChar()))
                return true;
            break;
        case QMetaType::QBitArray:
            if(property.write(object, vValue.toBitArray()))
                return true;
            break;
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        {
            if(property.write(object, static_cast<qlonglong>(QLocale::c().toDouble(vValue.toString()))))
                return true;
            if(property.write(object, QLocale::c().toLongLong(vValue.toString())))
                return true;
            break;
        }
        case QMetaType::Int:
        case QMetaType::UInt:
        {
            if(property.write(object, QLocale::c().toInt(vValue.toString())))
                return true;
            if(property.write(object, QLocale::c().toInt(vValue.toString())))
                return true;
            if(property.write(object, QLocale::c().toUInt(vValue.toString())))
                return true;
            break;
        }
        case QMetaType::Double:
            if(property.write(object, QLocale::c().toDouble(vValue.toString())))
                return true;
            break;
        case QMetaType::QVariantMap:
            if(property.write(object, vValue.toHash()))
                return true;
            break;
        case QMetaType::QVariantHash:
            if(property.write(object, vValue.toHash()))
                return true;
            break;
        case QMetaType::QVariantList:
            if(property.write(object, vValue.toList()))
                return true;
            break;
        case QMetaType::QStringList:
            if(property.write(object, vValue.toStringList()))
                return true;
            break;
        case QMetaType::QDate:
            if(property.write(object, vValue.toDate()))
                return true;
            break;
        case QMetaType::QDateTime:
            if(property.write(object, vValue.toDateTime()))
                return true;
            break;
        case QMetaType::QTime:
            if(property.write(object, vValue.toTime()))
                return true;
            break;
        case QMetaType::Bool:{
            switch (property.typeId()) {
            case QMetaType::Bool:
                if(property.write(object, vValue.toBool()))
                    return true;
                break;
            default:
                break;
            }

            switch (vValue.typeId()){
            case QMetaType::Bool:
                if(property.write(object, vValue.toBool()))
                    return true;
                break;
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::ULongLong:
            case QMetaType::LongLong:
            case QMetaType::Double:
                if(property.write(object, (vValue.toInt()==1)))
                    return true;
                break;
            case QMetaType::QString:
            case QMetaType::QByteArray:
            case QMetaType::QChar:
            {
                static const auto __true=QStringLiteral("true");
                auto vv=vValue.toString().toLower();
                bool vBool=(vv==__true);
                if(property.write(object, vBool))
                    return true;
                break;
            }
            default:
                return false;
            }
        }
        default:
            break;
        }
        return false;
    }
};

MetaObjectUtil::MetaObjectUtil()
{
    this->p = new MetaObjectUtilPvt{this};
}

MetaObjectUtil::MetaObjectUtil(const QMetaObject &metaObject)
{
    this->p = new MetaObjectUtilPvt{this};

    p->metaObjectList.clear();
    p->metaObjectList.append(&metaObject);
}

MetaObjectUtil::~MetaObjectUtil()
{
    delete p;
}

MetaObjectUtil &MetaObjectUtil::operator=(const QMetaObject &v)
{

    p->metaObjectList.clear();
    p->metaObjectList.append(&v);
    return *this;
}

MetaObjectUtil &MetaObjectUtil::operator+=(const QMetaObject &v)
{

    p->metaObjectList.clear();
    p->metaObjectList.append(&v);
    return *this;
}

MetaObjectUtil &MetaObjectUtil::operator-=(const QMetaObject &v)
{

    Q_UNUSED(v)
    p->metaObjectList.clear();
    p->metaObjectList<<&QObject::staticMetaObject;
    return *this;
}

MetaObjectUtil &MetaObjectUtil::operator<<(const QMetaObject &v)
{

    p->metaObjectList.clear();
    p->metaObjectList.append(&v);
    return *this;
}

QObject *MetaObjectUtil::newInstance(QObject *parent){

    auto &__metaObject=*p->metaObjectList.first();
    auto object=__metaObject.newInstance(Q_ARG(QObject*, parent));
    return object;
}

QObject *MetaObjectUtil::newInstance(const QMetaObject &metaObject, QObject *parent)
{
    auto &__metaObject=metaObject;
    auto object=__metaObject.newInstance(Q_ARG(QObject*, parent));
    return object;
}

QMetaMethod MetaObjectUtil::method(const QString &name)
{

    auto __name=name.toLower().trimmed();
    for(auto &metaObject:p->metaObjectList){
        for(int i = 0; i < metaObject->methodCount(); ++i) {
            auto method = metaObject->method(i);
            auto name__=method.name().toLower();
            if(name__ != __name)
                continue;

            return method;
        }
    }
    return QMetaMethod();
}

QMetaProperty MetaObjectUtil::property(const QByteArray &name)
{

    auto __name=name.toLower().trimmed();
    for(auto &metaObject:p->metaObjectList){
        for(int i = 0; i < metaObject->propertyCount(); ++i) {
            auto property = metaObject->property(i);
            auto name__=QByteArray{property.name()}.toLower();
            if(name__ != __name)
                continue;

            return property;
        }
    }
    return QMetaProperty();
}

const QVariantMap MetaObjectUtil::toMap(const QObject *object) const
{
    return QVariant(this->toHash(object)).toMap();
}

const QVariantHash MetaObjectUtil::toHash(const QObject *object)const
{
    QVariantHash __return;
    auto &metaObject = *object->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        __return.insert(property.name(), property.read(object));
    }
    return __return;
}

bool MetaObjectUtil::writeMap(QObject *object, const QVariantMap &v)
{

    bool __return=false;
    auto vProperty=this->toPropertyMap(object);
    Q_V_MAP_ITERATOR(v){
        i.next();
        auto k=i.key().toUtf8();
        auto &v=i.value();
        auto property=vProperty.value(k);
        if(p->writeProperty(object, property, v))
            __return=true;
    }
    return __return;
}

bool MetaObjectUtil::writeHash(QObject *object, const QVariantHash &v)
{

    bool __return=false;
    auto vProperty=this->toPropertyMap(object);
    Q_V_HASH_ITERATOR(v){
        i.next();
        auto k=i.key().toUtf8();
        auto &v=i.value();
        auto property=vProperty.value(k);
        if(p->writeProperty(object, property, v))
            __return=true;
    }
    return __return;
}

const QList<QMetaProperty> MetaObjectUtil::toPropertyList(const QObject *object) const
{

    QList<const QMetaObject*> __metaObject;
    if(object!=nullptr){
        __metaObject<<object->metaObject();
    }
    else{
        __metaObject=p->metaObjectList;
    }
    auto &metaObject =*__metaObject.first();

    QList<QMetaProperty> __return;
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        __return<<property;
    }
    return __return;
}

const QHash<QByteArray, QMetaProperty> MetaObjectUtil::toPropertyMap(const QObject *object) const
{

    auto __metaObject=p->metaObjectList;
    if(object!=nullptr){
        __metaObject.clear();
        __metaObject<<object->metaObject();
    }
    auto &metaObject =*__metaObject.first();
    QHash<QByteArray, QMetaProperty> __return;
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        __return[property.name()]=property;
    }
    return __return;
}

}
