#include "./qstm_util_map.h"
#include "./qstm_util_hash.h"
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QMetaProperty>

namespace QStm {

class MapUtilPvt:public QObject{
public:
    const QByteArray __objectName=QByteArrayLiteral("objectName");
    MapUtil *parent=nullptr;
    QVariantHash vSource;
    explicit MapUtilPvt(const QVariant &vSourceValues, MapUtil *parent)
        :
        QObject{parent},
        parent{parent},
        vSource{convertToHash(vSourceValues)}
    {
    }

    void append(const QVariant &values)
    {
        auto vHash=this->convertToHash(values);
        QHashIterator<QString, QVariant> i(vHash);
        while(i.hasNext()){
            i.next();
            this->vSource.insert(i.key().trimmed().toLower(),i.value());
        }
    }

    bool isMap(const QVariant &value)
    {
        switch (value.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            return true;
        case QMetaType::QString:
        case QMetaType::QByteArray:
        {
            auto typeId=QJsonDocument::fromJson(value.toByteArray()).toVariant().typeId();
            switch (typeId) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantPair:
                return true;
            default:
                return false;
            }
        }
        default:
            return {};
        }
    }

    QStringList convertToKeys(const QVariant &keys)
    {
        QStringList vKeys;
        switch (keys.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            vKeys=keys.toHash().keys();
            break;
        case QMetaType::QString:
        case QMetaType::QByteArray:
            vKeys={keys.toString().trimmed()};
            break;
        case QMetaType::QStringList:
            vKeys=keys.toStringList();
            break;
        case QMetaType::QVariantList:
        {
            auto vList=keys.toList();
            for(auto &v:vList){
                auto key=v.toString().trimmed().toLower();
                if(!vKeys.contains(key))
                    vKeys.append(key);
            }
            break;
        }
        default:
            vKeys.append(keys.toString().trimmed().toLower());
        }
        return vKeys;
    }

    QVariantHash convertToHash(const QVariant &v)
    {
        QVariantHash vHashIn;
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            vHashIn=v.toHash();
            break;
        case QMetaType::QString:
        case QMetaType::QByteArray:
            vHashIn=QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash();
            break;
        case QMetaType::QObjectStar:
        {
            auto object=v.value<QObject*>();
            if(!object)
                return {};

            const auto &metaObject = *object->metaObject();

            QVariantHash vHash;

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
                    value=property.read(object).toInt();
                    break;
                default:
                    value=property.read(object);
                }
                if(value.isNull() || !value.isValid())
                    continue;

                vHash.insert(QByteArray{property.name()}.toLower(), value);
            }
            return vHash;
        }
        default:
            vHashIn=v.toHash();
        }

        QVariantHash vHashOut;
        QHashIterator<QString, QVariant> i(vHashIn);
        while(i.hasNext()){
            i.next();
            vHashOut.insert(i.key().trimmed().toLower(), i.value());
        }
        return vHashOut;
    }

    void clear()
    {
        this->vSource={};
    }
};

MapUtil::MapUtil(QObject *sourceObject): QObject{sourceObject}, p{new MapUtilPvt{QVariant::fromValue<QObject*>(sourceObject),this}}
{
}

MapUtil::MapUtil(const QVariant &sourceValues, QObject *parent): QObject{parent}, p{new MapUtilPvt{sourceValues,this}}
{
}

bool MapUtil::count()
{
    return p->vSource.count();
}

bool MapUtil::isEmpty()
{
    return p->vSource.isEmpty();
}

bool MapUtil::contains(const QVariant &keys)
{
    if(p->vSource.isEmpty())
        return {};
    auto vKeys=p->convertToKeys(keys);
    for(auto &key: vKeys){
        if(p->vSource.contains(key))
            return true;
    }
    return false;
}

bool MapUtil::isMap(const QVariant &value)
{
    return p->isMap(value);
}

MapUtil &MapUtil::clear()
{
    p->clear();
    return *this;
}

QVariant MapUtil::value(const QVariant &key)const
{
    if(p->vSource.isEmpty())
        return {};
    auto vKeys=p->convertToKeys(key);
    for(auto &key: vKeys)
        p->vSource.value(key);
    return {};
}

QVariant MapUtil::valueFirst() const
{
    if(p->vSource.isEmpty())
        return {};
    return p->vSource.begin().value();
}

QVariant MapUtil::valueLast() const
{
    if(p->vSource.isEmpty())
        return {};
    return p->vSource.end().value();
}

QVariantList MapUtil::values()
{
    if(p->vSource.isEmpty())
        return {};
    return p->vSource.values();
}

QVariantList MapUtil::values(const QVariant &keys)
{
    if(p->vSource.isEmpty())
        return {};
    QVariantList vValues;
    if(p->vSource.isEmpty())
            return {};
    auto vKeys=p->convertToKeys(keys);
    QStringList vKeyMatch;
    QHashIterator<QString, QVariant> i(p->vSource);
    while(i.hasNext()){
        i.next();
        if(vKeys.contains(i.key().trimmed().toLower()))
            vKeyMatch.append(i.key());
    }
    for(auto &key: vKeyMatch)
        vValues.append(p->vSource.value(key));
    return vValues;
}

QVariant MapUtil::take(const QVariant &key)
{
    if(p->vSource.isEmpty())
        return {};
    auto vKeys=p->convertToKeys(key);
    for(auto &key: vKeys)
        p->vSource.value(key);
    return {};
}

QVariantList MapUtil::takeValues(const QVariant &keys)
{
    if(p->vSource.isEmpty())
        return {};
    QVariantList vValues;
    if(p->vSource.isEmpty())
        return {};
    auto vKeys=p->convertToKeys(keys);
    QStringList vKeyMatch;
    QHashIterator<QString, QVariant> i(p->vSource);
    while(i.hasNext()){
        i.next();
        if(vKeys.contains(i.key().trimmed().toLower()))
            vKeyMatch.append(i.key());
    }
    for(auto &key: vKeyMatch)
        vValues.append(p->vSource.take(key));
    return vValues;
}

QVariant MapUtil::takeFirst() const
{
    if(p->vSource.isEmpty())
        return {};
    auto key=p->vSource.begin().key();
    return p->vSource.take(key);
}

QVariant MapUtil::takeLast() const
{
    if(p->vSource.isEmpty())
        return {};
    auto key=p->vSource.end().key();
    return p->vSource.take(key);
}

MapUtil &MapUtil::append(const QVariant &value)
{
    p->append(value);
    return *this;
}

MapUtil &MapUtil::append(QObject *value)
{
    p->append(QVariant::fromValue<QObject*>(value));
    return *this;
}

MapUtil &MapUtil::append(const QString &key, const QVariant &value)
{
    p->append(QVariantHash{{key, value}});
    return *this;
}

MapUtil &MapUtil::remove(const QVariant &keys)
{
    if(p->vSource.isEmpty())
        return *this;
    auto vKeys=p->convertToKeys(keys);

    QStringList vKeyMatch;
    QHashIterator<QString, QVariant> i(p->vSource);
    while(i.hasNext()){
        i.next();
        if(vKeys.contains(i.key().trimmed().toLower()))
            vKeyMatch.append(i.key());
    }
    for(auto &key: vKeyMatch)
        p->vSource.remove(key);
    return *this;
}

const QVariant MapUtil::toVariant() const
{
    if(p->vSource.isEmpty())
        return {};
    return p->vSource;
}

const QByteArray MapUtil::toJson() const
{
    if(p->vSource.isEmpty())
        return {};
    return QJsonDocument::fromVariant(p->vSource).toJson(QJsonDocument::Compact);
}

const QByteArray MapUtil::toMd5() const
{
    if(p->vSource.isEmpty())
        return {};
    Q_DECLARE_HU;
    return hu.toMd5(this->toJson());
}

const QUuid MapUtil::toMd5Uuid() const
{
    if(p->vSource.isEmpty())
        return {};
    Q_DECLARE_HU;
    return hu.toMd5Uuid(this->toJson());
}

} // namespace QStm
