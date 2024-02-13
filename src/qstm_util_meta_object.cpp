#include "./qstm_util_meta_object.h"
#include "./qstm_macro.h"
#include <QUuid>
#include <QUrl>
#include <QBitArray>
#include <QStringList>
#include <QJsonDocument>
#include <QDateTime>
#include <QVariant>
#include <QVariantList>
#include <QStringList>
#include <QVariantHash>
#include <QVariantMap>
#include <QMetaMethodArgument>

namespace QStm {

class MetaObjectReturnUtil:public QObject{
public:
    QVariant rVariant;
    QString rString;
    QByteArray rByteArray;
    QVariantHash rVariantHash;
    QVariantMap rVariantMap;
    QVariantList rVariantList;
    QStringList rStringList;
    QUuid rUuid;
    QUrl rUrl;
    int rInt;
    uint rUInt;
    long rLong;
    ulong rULong;
    qlonglong rLongLong;
    qulonglong rULongLong;
    bool rBool;
    QDate rDate;
    QTime rTime;
    QDateTime rDateTime;

    const QMetaMethod &method;
    explicit MetaObjectReturnUtil(QMetaMethod &method):method{method}{
    }

    QVariant getReturn(){
        switch (this->method.returnType()) {
        case QMetaType::QVariant :
            return rVariant;
        case QMetaType::QString :
            return rString;
        case QMetaType::QByteArray :
            return rByteArray;
        case QMetaType::QVariantHash :
            return rVariantHash;
        case QMetaType::QVariantMap :
            return rVariantMap;
        case QMetaType::QVariantList :
            return rVariantList;
        case QMetaType::QStringList :
            return rStringList;
        case QMetaType::QUuid :
            return rUuid;
        case QMetaType::QUrl :
            return rUrl;
        case QMetaType::Int :
            return rInt;
        case QMetaType::UInt :
            return rUInt;
        case QMetaType::Long :
            return QVariant::fromValue(rLong);
        case QMetaType::ULong :
            return QVariant::fromValue(rULong);
        case QMetaType::LongLong :
            return rLongLong;
        case QMetaType::ULongLong :
            return rULongLong;
        case QMetaType::Bool :
            return rBool;
        case QMetaType::QDate :
            return rDate;
        case QMetaType::QTime :
            return rTime;
        case QMetaType::QDateTime:
            return rDateTime;
        default:
            return rVariant;
        }
    }

    auto createReturnArg(){
        switch (this->method.returnType()) {
        case QMetaType::QVariant :
            return Q_RETURN_ARG(QVariant, rVariant);
        case QMetaType::QString :
            return Q_RETURN_ARG(QString, rString);
        case QMetaType::QByteArray :
            return Q_RETURN_ARG(QByteArray, rByteArray);
        case QMetaType::QVariantHash :
            return Q_RETURN_ARG(QVariantHash, rVariantHash);
        case QMetaType::QVariantMap :
            return Q_RETURN_ARG(QVariantMap, rVariantMap);
        case QMetaType::QVariantList :
            return Q_RETURN_ARG(QVariantList, rVariantList);
        case QMetaType::QStringList :
            return Q_RETURN_ARG(QStringList, rStringList);
        case QMetaType::QUuid :
            return Q_RETURN_ARG(QUuid, rUuid);
        case QMetaType::QUrl :
            return Q_RETURN_ARG(QUrl, rUrl);
        case QMetaType::Int :
            return Q_RETURN_ARG(int, rInt);
        case QMetaType::UInt :
            return Q_RETURN_ARG(uint, rUInt);
        case QMetaType::Long :
            return Q_RETURN_ARG(long, rLong);
        case QMetaType::ULong :
            return Q_RETURN_ARG(ulong, rULong);
        case QMetaType::LongLong :
            return Q_RETURN_ARG(qlonglong, rLongLong);
        case QMetaType::ULongLong :
            return Q_RETURN_ARG(qulonglong, rULongLong);
        case QMetaType::Bool :
            return Q_RETURN_ARG(bool, rBool);
        case QMetaType::QDate :
            return Q_RETURN_ARG(QDate, rDate);
        case QMetaType::QTime :
            return Q_RETURN_ARG(QTime, rTime);
        case QMetaType::QDateTime:
            return Q_RETURN_ARG(QDateTime, rDateTime);
        default:
            return Q_RETURN_ARG(QVariant, rVariant);
        }
    }

};


class MetaObjectUtilPvt:public QObject{
public:
    MetaObjectUtil*parent=nullptr;
    const QMetaObject objectMetaObject;
    explicit MetaObjectUtilPvt(const QMetaObject &objectMetaObject, MetaObjectUtil *parent):QObject{parent}, parent{parent},objectMetaObject{objectMetaObject}
    {
    }

    static QHash<QByteArray, QMetaMethod> toMethodHash(const QMetaObject &metaObject)
    {
        static const QByteArray __objectName=QByteArrayLiteral("objectName");
        QHash<QByteArray, QMetaMethod> __return;
        for(int col = 0; col < metaObject.methodCount(); ++col) {
            auto method = metaObject.method(col);
            if(method.name()==__objectName)
                continue;
            __return.insert(method.name(), method);
        }
        return __return;
    }

    static QList<QMetaMethod> toMethodList(const QMetaObject &metaObject)
    {
        static const QByteArray __objectName=QByteArrayLiteral("objectName");
        QList<QMetaMethod> __return;
        for(int col = 0; col < metaObject.methodCount(); ++col) {
            auto method = metaObject.method(col);
            if(method.name()==__objectName)
                continue;
            __return.append(method);
        }
        return __return;
    }

    static QHash<QByteArray, QMetaProperty> toPropertyHash(const QMetaObject &metaObject)
    {
        static const QByteArray __objectName=QByteArrayLiteral("objectName");
        QHash<QByteArray, QMetaProperty> __return;
        for(int col = 0; col < metaObject.propertyCount(); ++col) {
            auto property = metaObject.property(col);
            if(property.name()==__objectName)
                continue;
            __return.insert(property.name(), property);
        }
        return __return;
    }

    static QList<QMetaProperty> toPropertyList(const QMetaObject &metaObject)
    {
        static const QByteArray __objectName=QByteArrayLiteral("objectName");
        QList<QMetaProperty> __return;
        for(int col = 0; col < metaObject.propertyCount(); ++col) {
            auto property = metaObject.property(col);
            if(property.name()==__objectName)
                continue;
            __return.append(property);
        }
        return __return;
    }

    static bool writeProperty(QObject *object, const QMetaProperty &property, const QVariant &value)
    {
        if(!property.isWritable())
            return true;

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

MetaObjectUtil::MetaObjectUtil(QObject *parent):QObject{parent}, p{new MetaObjectUtilPvt{QObject::staticMetaObject, this}}
{
}

MetaObjectUtil::MetaObjectUtil(const QMetaObject &objectMetaObject, QObject *parent): QObject{parent}, p{new MetaObjectUtilPvt{objectMetaObject, this}}
{
}

bool MetaObjectUtil::invoke(QObject *object, QMetaMethod &method, QVariant &returnValue, const QVariantHash &args)
{

    if(object==nullptr){
        sWarning()<<"Invalid object: ";
        return false;
    }

    if(!method.isValid()){
        sWarning()<<"Invalid method: "<<method.name();
        return false;
    }

    returnValue={};

    MetaObjectReturnUtil uReturn(method);

    auto argReturn = uReturn.createReturnArg();

    if(method.parameterNames().empty()){
        if (!method.invoke(object, Qt::AutoConnection, argReturn))
            return false;
    }
    else{

        QHash<int,QMetaMethodArgument> vars;
        for (int var = 0; var < method.parameterNames().count(); ++var) {
            auto name=method.parameterNames().at(var);
            auto typeName=method.parameterTypeName(var);
            auto value=args.value(name);
            auto arg=QtPrivate::Invoke::argument(typeName, value);
            vars.insert(vars.count(), arg);
        }


        auto arg00=vars.value(0);
        auto arg01=vars.value(1);
        auto arg02=vars.value(2);
        auto arg03=vars.value(3);
        auto arg04=vars.value(4);
        auto arg05=vars.value(5);
        auto arg06=vars.value(6);
        auto arg07=vars.value(7);
        auto arg08=vars.value(8);
        auto arg09=vars.value(9);

        if (!method.invoke(object, arg00, arg01, arg02, arg03, arg04, arg05, arg06, arg07, arg08, arg09, Qt::DirectConnection, argReturn))
            return false;
    }

    returnValue=uReturn.getReturn();

    return true;
}

bool MetaObjectUtil::invoke(QObject *object, const QByteArray &methodName, QVariant &returnValue, const QVariantHash &args)
{
    MetaObjectUtil metaObjectUtil(*object->metaObject());
    auto method=metaObjectUtil.method(methodName);
    return invoke(object, method, returnValue, args);
}

bool MetaObjectUtil::invoke(QMetaMethod &method, QVariant &returnValue, const QVariantHash &args)
{
    QScopedPointer<QObject> obj(this->newInstance());
    return this->invoke(obj.data(), method, returnValue, args);
}

bool MetaObjectUtil::invoke(const QByteArray &methodName, QVariant &returnValue, const QVariantHash &args)
{
    QScopedPointer<QObject> obj(this->newInstance());
    return this->invoke(obj.data(), methodName, returnValue, args);
}

QObject *MetaObjectUtil::newInstance(QObject *parent)
{
    return p->objectMetaObject.newInstance(Q_ARG(QObject *, parent));
}

QObject *MetaObjectUtil::newInstance(const QMetaObject &metaObject, QObject *parent)
{
    return metaObject.newInstance(Q_ARG(QObject *, parent));
}

QMetaMethod MetaObjectUtil::method(const QString &name)const
{
    auto __name=name.toLower().trimmed();
    for(int i = 0; i < p->objectMetaObject.methodCount(); ++i) {
        auto method = p->objectMetaObject.method(i);
        auto name__=method.name().toLower();
        if(name__ != __name)
            continue;
        return method;
    }
    return {};
}

QMetaProperty MetaObjectUtil::property(const QByteArray &name)const
{
    static const QByteArray __objectName=QByteArrayLiteral("objectName");
    auto __name=name.toLower().trimmed();
    for(int i = 0; i < p->objectMetaObject.propertyCount(); ++i) {
        auto property = p->objectMetaObject.property(i);
        if(property.name()==__objectName)
            continue;
        auto name__=QByteArray{property.name()}.toLower();
        if(name__ != __name)
            continue;
        return property;
    }
    return {};
}

const QVariantHash MetaObjectUtil::toHash(const QObject *object)const
{
    if(!object)
        return {};
    QVariantHash __return;
    auto &metaObject = *object->metaObject();
    auto vProperty=p->toPropertyList(metaObject);
    for(auto &property:vProperty){
        auto value=(property.isReadable())
                         ?property.read(object)
                         :QVariant{};
        __return.insert(property.name(), value);
    }
    return __return;
}

bool MetaObjectUtil::writeHash(QObject *object, const QVariantHash &v)
{
    bool __return=false;
    auto &metaObject = *object->metaObject();
    auto vProperty=p->toPropertyHash(metaObject);
    QHashIterator<QString, QVariant> i(v);
    while (i.hasNext()){
        i.next();
        const auto k=i.key().toUtf8();
        const auto &v=i.value();
        auto property=vProperty.value(k);
        if(p->writeProperty(object, property, v))
            __return=true;
    }
    return __return;
}

void MetaObjectUtil::reset(QObject *object)
{
    if(object==nullptr)
        return;
    auto &metaObject = *object->metaObject();
    auto propertyList=p->toPropertyList(metaObject);
    for(auto &property: propertyList){
        if(property.isReadable())
            property.reset(object);
    }
}

const QList<QMetaMethod> MetaObjectUtil::toMethodList(const QObject *object) const
{
    const auto &metaObject=(object!=nullptr)
                                 ?(*object->metaObject())
                                 :p->objectMetaObject;
    return p->toMethodList(metaObject);
}

const QHash<QByteArray, QMetaMethod> MetaObjectUtil::toMethodHash(const QObject *object) const
{
    const auto &metaObject=(object!=nullptr)
                                 ?(*object->metaObject())
                                 :p->objectMetaObject;
    return p->toMethodHash(metaObject);
}

const QList<QMetaProperty> MetaObjectUtil::toPropertyList(const QObject *object) const
{
    const auto &metaObject=(object!=nullptr)
                                 ?(*object->metaObject())
                                 :p->objectMetaObject;
    return p->toPropertyList(metaObject);
}

const QHash<QByteArray, QMetaProperty> MetaObjectUtil::toPropertyHash(const QObject *object) const
{
    const auto &metaObject=(object!=nullptr)
                                 ?(*object->metaObject())
                                 :p->objectMetaObject;
    return p->toPropertyHash(metaObject);
}

}
