#include "./qstm_util_variant.h"
#include "./qstm_util_date.h"
#include "./qstm_util_hash.h"
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtCborCommon>
#include <QCborStreamReader>
#include <QCborStreamWriter>
#include <QCborArray>
#include <QCborMap>
#include <QCborValue>
#include <QCborParserError>
#include <QFile>

#define charPercent QByteArrayLiteral("%")
#define charNumber QByteArrayLiteral("N")

namespace QStm {

#define __setValue(v) if(v.isValid())QVariant::setValue(v);

class VariantUtilPvt{
public:
    VariantUtil *parent=nullptr;

    explicit VariantUtilPvt(VariantUtil*v)
    {
        this->parent=v;
    }

    virtual ~VariantUtilPvt()
    {
    }

    void clear()
    {
        this->parent->setValue({});
    }

    QVariantPair toPair(const QVariant &v)const
    {
        auto makeFromJson=[&v](){
            auto bytes=v.toByteArray().trimmed();
            if(bytes.isEmpty())
                return QVariantHash{};

            return QJsonDocument::fromJson(bytes).object().toVariantHash();
        };

        if(v.isNull() || !v.isValid())
            return {};

        switch (v.typeId()) {
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        {
            auto vHash=v.toHash();
            if(vHash.isEmpty())
                return {};
            return QVariantPair{vHash.cbegin().key(), vHash.cbegin().value()};
        }
        case QMetaType::QVariantPair:
            return v.value<QVariantPair>();
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto list=v.toList();
            if(list.isEmpty())
                return {};
            return toPair(list.first());
        }
        case QMetaType::QString:
        case QMetaType::QByteArray:
            return toPair(makeFromJson());
        default:
            return {};
        }
    }

    QVariantHash toHash(const QVariant &v)const
    {
        auto makeFromJson=[&v](){
            auto bytes=v.toByteArray().trimmed();
            if(bytes.isEmpty())
                return QVariantHash{};

            return QJsonDocument::fromJson(bytes).object().toVariantHash();
        };

        if(v.isNull() || !v.isValid())
            return {};

        switch (v.typeId()) {
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
            return v.toHash();
        case QMetaType::QVariantPair:
        {
            auto pair=v.value<QVariantPair>();
            return QVariantHash{{pair.first.toString(), pair.second}};
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto list=v.toList();
            if(list.isEmpty())
                return {};
            return toHash(list.first());
        }
        case QMetaType::QString:
        case QMetaType::QByteArray:
            return makeFromJson();
        default:
            return {};
        }
    }

    QVariantMap toMap(const QVariant &v)const
    {
        auto makeFromJson=[&v](){
            auto bytes=v.toByteArray().trimmed();
            if(bytes.isEmpty())
                return QVariantMap{};

            return QJsonDocument::fromJson(bytes).object().toVariantMap();
        };

        if(v.isNull() || !v.isValid())
            return {};

        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
            return v.toMap();
        case QMetaType::QVariantPair:
        {
            auto pair=v.value<QVariantPair>();
            return QVariantMap{{pair.first.toString(), pair.second}};
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto list=v.toList();
            if(list.isEmpty())
                return {};
            return toMap(list.first());
        }
        case QMetaType::QString:
        case QMetaType::QByteArray:
            return makeFromJson();
        default:
            return {};
        }
    }

    static QByteArray toByteArray(const QVariant &v)
    {
        switch (v.typeId()) {
        case QMetaType::QUuid:
            return v.toUuid().toByteArray();
        case QMetaType::QUrl:
        {
            auto url=v.toUrl();
            if(url.isEmpty())
                return {};
            return url.toString().toUtf8();
            break;
        }
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            return QString::number(v.toLongLong()).toUtf8();
        case QMetaType::Double:
            return QString::number(v.toDouble(),'f').toUtf8();
        case QMetaType::QDate:
            return v.toDate().toString(Qt::ISODate).toUtf8();
        case QMetaType::QTime:
            return v.toTime().toString(Qt::ISODateWithMs).toUtf8();
        case QMetaType::QDateTime:
            return v.toDateTime().toString(Qt::ISODateWithMs).toUtf8();
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        case QMetaType::QVariantPair:
            return QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
        default:
            return v.toByteArray();
        }
    }

    static bool canConvertJson(const QVariant &v, QVariant &vOut)
    {
        vOut={};
        if(v.isNull() || !v.isValid())
            return false;

        switch (v.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            vOut=v.toList();
            return true;
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            vOut=v.toHash();
            return true;
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QChar:
        case QMetaType::QBitArray:
        {
            QJsonParseError*error=nullptr;
            auto doc=QJsonDocument::fromJson(v.toByteArray(),error);
            if(error!=nullptr || (doc.isArray() && doc.isObject()))
                return false;

            vOut=doc.toVariant();
            switch (vOut.typeId()) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantPair:
                vOut=vOut.toHash();
                break;
            case QMetaType::QVariantList:
            case QMetaType::QStringList:
                vOut=vOut.toList();
                break;
            default:
                break;
            }
            return true;
        }
        default:
            return false;
        }
    }

    QVariant vUnion(const QVariant &v)
    {
        switch (v.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto vList=v.toList();
            QVariant vA;
            while(!vList.isEmpty()){
                auto vB=vList.takeFirst();
                if(vA.typeId()==vB.typeId())
                    vA=vUnion(vA,vB);
            }
            return vA;
        }
        default:
            return {};
        }
    }

    QVariant vUnion(const QVariant &vDestine, const QVariant &vSource)
    {
        if(vDestine.typeId()!=vSource.typeId())
            return {};

        switch (vDestine.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            return (vDestine.toList()+vSource.toList());
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        case QMetaType::QVariantPair:
        {
            auto aMap=vDestine.toHash();
            auto bMap=vSource.toHash();
            QHashIterator<QString, QVariant> i(bMap);
            while (i.hasNext()) {
                i.next();
                const auto &k=i.key();
                if(!aMap.contains(k))
                    aMap[k]=i.value();
            }
            return aMap;
        }
        default:
            QByteArray rByte;
            auto ls=QVariantList{vDestine, vSource};
            for(auto &v:ls)
                rByte.append(v.toByteArray());
            return rByte;
        }
    }

    QVariant vMerge(const QVariant &v)
    {
        QVariant vOut;
        if(!this->canConvertJson(v,vOut) || !vOut.isValid())
            return {};

        switch (vOut.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto vList=vOut.toList();
            if(vList.isEmpty())
                return {};
            auto vA=QVariant{vList.takeFirst()};
            for(const auto &vB:vList){
                if(vB.typeId()==vA.typeId())
                    vA=vMerge(vA, vB);
            }
            return vA;
        }
        default:
            return {};
        }
    }

    static QVariant vMerge(const QVariant &vDestineIn, const QVariant &vSourceIn)
    {
        QVariant vDestine;
        QVariant vSource;
        if(!canConvertJson(vDestineIn, vDestine))
            vDestine=vDestineIn;

        if(!canConvertJson(vSourceIn, vSource))
            vSource=vSourceIn;

        QMetaType::Type vSourceTypeId=QMetaType::Type(vSource.isValid()?vSource.typeId():vSourceIn.typeId());
        QMetaType::Type vDestineTypeId=QMetaType::Type(vDestine.isValid()?vDestine.typeId():vDestineIn.typeId());

        if(vSourceTypeId == QMetaType::UnknownType)
            vSourceTypeId = vDestineTypeId;

        if(vDestineTypeId == QMetaType::UnknownType)
            vDestineTypeId = vSourceTypeId;

        if(vDestineTypeId != vSourceTypeId)
            return vSourceIn;//send merge subistituimos pelo novo valor

        Q_DECLARE_HU;
        switch (vDestine.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto dst=vDestine.toList();
            auto src=vSource.toList();
            QStringList keyList;
            QVariantList lst;
            for(const auto &v:dst){//cache de chaves existentes
                if(v.isValid()){
                    switch (v.typeId()) {
                    case QMetaType::QVariantHash:
                    case QMetaType::QVariantMap:
                    case QMetaType::QVariantPair:
                    {
                        keyList.append(hu.toMd5(v));
                        continue;
                    }
                    default:
                        keyList.append(toByteArray(v));
                    }
                }
            }
            for(auto &v:src){
                if(v.isValid()){
                    QByteArray key;

                    switch (v.typeId()) {
                    case QMetaType::QVariantHash:
                    case QMetaType::QVariantMap:
                    case QMetaType::QVariantPair:
                        key=hu.toMd5(v);
                        break;
                    default:
                        key=toByteArray(v);
                    }
                    if(!lst.contains(key))
                        lst.append(v);
                }
            }
            return lst;
        }
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            auto aHash=vDestine.toHash();
            auto bHash=vSource.toHash();
            QHashIterator<QString, QVariant> i(bHash);
            while (i.hasNext()) {
                i.next();
                const auto &k=i.key();
                const auto &vb=i.value();
                auto &va=aHash[k];
                if(!va.isValid() || va.isNull())
                    va=vb;
                else
                    aHash.insert(k, vMerge(va,vb));
            }
            return aHash;
        }

        default:
            return vSourceIn;
        }
    }

    QVariant vDeduplicate(const QVariant &v)
    {
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            auto vMap=v.toHash();
            auto vAdd=QVariantHash();
            QHashIterator<QString, QVariant> i(vMap);
            while (i.hasNext()) {
                i.next();
                const auto &key=i.key();//no modifique a chave
                auto value=vDeduplicate(i.value());
                if(vAdd.contains(key))
                    vAdd.insert(key, value);
            }
            return vAdd;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            Q_DECLARE_HU;
            auto vMap=v.toList();
            auto vAdd=QVariantHash();
            for(auto &v:vMap){
                const auto value=vDeduplicate(v);
                const auto key=hu.toMd5(value);
                if(!vAdd.contains(key))
                    vAdd.insert(key,value);
            }
            return vAdd.values();
        }
        default:
            return {};
        }
    }
};

VariantUtil::VariantUtil(const QVariant &v):QVariant{v}, p{new VariantUtilPvt{this}}
{
}

VariantUtil::~VariantUtil()
{
    delete p;
}

VariantUtil&VariantUtil::operator=(const QVariant &v)
{
    this->setValue(v);
    return *this;
}

bool VariantUtil::isUuid(const QVariant &v) const
{
    Q_DECLARE_HU;
    return hu.isUuid(v);
}

bool VariantUtil::isUuid(const QVariant &v, QUuid &uuidSet) const
{
    Q_DECLARE_HU;
    return hu.isUuid(v,uuidSet);
}

bool VariantUtil::isHex(const QVariant &v)const
{
    Q_DECLARE_HU;
    return hu.isHex(v);
}

bool VariantUtil::isBase64(const QVariant &v) const
{
    Q_DECLARE_HU;
    return hu.isBase64(v);
}

const QString VariantUtil::toStr(const QVariant &v)
{
    __setValue(v);
    return p->toByteArray(*this);
}

const QByteArray VariantUtil::toAlphaNumber(const QVariant &v)
{
    QString __return;
    static auto num=QStringLiteral("0123456789");
    auto text=v.toString();
    for(auto &c:text){
        if(num.contains(c))
            __return+=c;
    }
    return __return.toUtf8();
}

const QByteArray VariantUtil::toAlphaNumeric(const QVariant &v)
{
    QString __return;
    static auto num=QStringLiteral("0123456789.,-");
    auto text=v.toString();
    for(auto &c:text){
        if(num.contains(c))
            __return+=c;
    }
    return __return.toUtf8();
}

const QString VariantUtil::toAlphaText(const QVariant &v)
{
    QString __return;
    static auto num=QStringLiteral("0123456789");
    auto text=v.toString();
    for(auto &c:text){
        if(!num.contains(c))
            __return+=c;
    }
    return __return;
}

const QByteArray VariantUtil::toByteArray(const QVariant &v)
{
    __setValue(v);
    return p->toByteArray(*this);
}

const QChar VariantUtil::toChar(const QVariant &v)
{
    __setValue(v);
    QString bytes;
    switch (this->typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantPair:
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        bytes=QJsonDocument::fromVariant(*this).toJson(QJsonDocument::Compact);
        break;
    case QMetaType::QUuid:
        bytes=QVariant::toUuid().toString();
        break;
    case QMetaType::QUrl:
    {
        auto url=QVariant::toUrl();
        if(url.isEmpty())
            return {};
        bytes=url.toString();
        break;
    }
    default:
        bytes=QVariant::toByteArray();
    }
    if(bytes.isEmpty())
        return {};
    return bytes.at(0);
}

int VariantUtil::toInt(const QVariant &v)
{
    __setValue(v);
    return QVariant::toUInt();
}

qlonglong VariantUtil::toLongLong(const QVariant &v)
{
    if(v.isValid()) QVariant::setValue(v);
    return QVariant::toLongLong();
}

const QDate VariantUtil::toDate(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_DU;
    return du.toDate(*this);
}

const QTime VariantUtil::toTime(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_DU;
    return du.toTime(*this);
}

const QDateTime VariantUtil::toDateTime(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_DU;
    return du.toDateTime(*this);
}

double VariantUtil::toDouble(const QVariant &v)
{
    __setValue(v);
    switch (this->typeId()) {
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Double:
        return QVariant::toDouble();
    case QMetaType::UnknownType:
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::QChar:
    case QMetaType::QBitArray:
    {
        auto vText=QVariant::toString().trimmed();
        if(vText=="" || vText==QStringLiteral("0"))
            return 0;
        if(vText==QStringLiteral("-1"))
            return -1;

        if(vText.endsWith(charPercent)){
            auto v = vText.replace(charPercent,"").toDouble();
            v/=100;
            return v;
        }
        else if(vText.endsWith(charNumber)){
            auto v = vText.replace(charNumber,"").toDouble();
            return v;
        }
        else{
            bool ok=false;
            auto vv = QVariant::toDouble(&ok);
            if(ok)
                return vv;
            return 0;
        }
        break;
    }
    default:
        return 0;
    }
}

bool VariantUtil::toBool(const QVariant &v)
{
    const static auto listBool=QVector<QByteArray>{
        "1",
        "true",
        "yes",
        "sim",
        "verdadeiro",
        "t",
        "s",
        "v",
        "y",
    };

    __setValue(v);

    if(this->typeId()==QMetaType::Bool)
        return QVariant::toBool();

    auto bytes=QVariant::toString().toLower().trimmed();
    if(bytes.isEmpty())
        return false;
    bytes=bytes.at(0);
    if(listBool.contains(bytes))
        return true;
    return false;
}

bool VariantUtil::canConvertJson(const QVariant &v) const
{
    QVariant vOut;

    return p->canConvertJson(v, vOut);
}

bool VariantUtil::canConvertJson(const QVariant &v, QVariant &vOut) const
{
    return p->canConvertJson(v, vOut);
}

const QByteArray VariantUtil::toMd5(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_HU;
    return hu.toMd5(*this);
}

const QByteArray VariantUtil::toHex(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_HU;
    return hu.toHex(*this);
}

const QUuid VariantUtil::toUuid(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_HU;
    return hu.toUuid(*this);
}

const QString VariantUtil::toUuidSimple(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_HU;
    return hu.toUuidSimple(*this);
}

const QUuid VariantUtil::toMd5Uuid(const QVariant &v)
{
    __setValue(v);
    Q_DECLARE_HU;
    return hu.toMd5Uuid(*this);
}

const QVariantList VariantUtil::takeList(const QByteArray &keyName)
{
    QVariantList la;
    QVariantList lb;

    switch (this->typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        la=this->toList();
        break;
    default:
        la.append(this->toHash());
        break;
    }

    for(auto &v:la){
        switch (this->typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            auto vm=v.toHash();
            auto vv=vm.value(keyName);
            if(vv.isValid())
                lb.append(vv);
            break;
        }
        default:
            break;
        }
    }
    return lb;
}

const QStringList VariantUtil::toStringList()
{
    return this->toStringList(*this);
}

const QStringList VariantUtil::toStringList(const QVariant &v)
{
    if(v.isNull() || !v.isValid())
        return {};

    switch (v.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    {
        QStringList outList;
        auto vList=v.toList();
        for(auto &v:vList){
            switch (v.typeId()) {
            case QMetaType::QVariantList:
            case QMetaType::QStringList:
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantPair:
                outList.append(QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact));
                break;
            case QMetaType::QUuid:
                outList.append(v.toUuid().toString());
                break;
            case QMetaType::QUrl:
                outList.append(v.toUrl().toString());
                break;
            default:
                outList.append(v.toByteArray());
                break;
            }
        }
        return outList;
    }
    case QMetaType::QString:
    case QMetaType::QByteArray:
    {
        if(v.toString().trimmed().isEmpty())
            return {};
        QJsonParseError*error=nullptr;
        auto doc=QJsonDocument::fromJson(v.toByteArray(), error);
        if(!doc.isEmpty() && !doc.isNull())
            return doc.toVariant().toStringList();
        break;
    }
    default:
        break;
    }
    return QStringList{v.toString().trimmed()};

}

const QVariantList VariantUtil::toList(const QVariant &v)
{
    __setValue(v);
    auto newV=this->toVariantObject(*this);
    switch (newV.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return newV.toList();
    default:
        return QVariantList{{v}};
    }
}

QVariantMap VariantUtil::toMap() const
{
    return p->toMap(*this);
}

const QVariantMap VariantUtil::toMap(const QVariant &v)
{
    __setValue(v)
    return p->toMap(*this);
}

const QVariantMap VariantUtil::toMap(const QVariant &key, const QVariant &value)
{
    QVariant v=QVariantHash{{key.toString(), value}};
    __setValue(v)
    return p->toMap(*this);
}

QVariantPair VariantUtil::toPair() const
{
    return p->toPair(*this);
}

const QVariantPair VariantUtil::toPair(const QVariant &v)
{
    __setValue(v);
    return p->toPair(*this);
}

const QVariantPair VariantUtil::toPair(const QVariant &key, const QVariant &value)
{
    QVariant v=QVariant::fromValue(QVariantPair{key, value});
    __setValue(v);
    return p->toPair(*this);
}

QVariantHash VariantUtil::toHash()const
{
    return p->toHash(*this);
}

const QVariantHash VariantUtil::toHash(const QVariant &v)
{
    __setValue(v);
    return p->toHash(*this);
}

const QVariantHash VariantUtil::toHash(const QVariant &key, const QVariant &value)
{
    QVariant v=QVariantHash{{key.toString(), value}};
    __setValue(v)
    return p->toHash(*this);
}

QMultiHash<QString, QVariant> VariantUtil::toMultiHash() const
{
    return QMultiHash<QString, QVariant>{this->toHash()};
}

const QMultiHash<QString, QVariant> VariantUtil::toMultiHash(const QVariant &v)
{
    return QMultiHash<QString, QVariant>{this->toHash(v)};
}

const QMultiHash<QString, QVariant> VariantUtil::toMultiHash(const QVariant &key, const QVariant &value)
{
    return QMultiHash<QString, QVariant>{this->toHash(key, value)};
}

const QVariantHash VariantUtil::toAttributes(const QVariant &v, const QVariant &attributeNames)
{
    QVariant value;
    QVariantList vRecordList;
    {//records
        switch (v.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        case QMetaType::QVariantList:
            value=v;
            break;
        case QMetaType::QString:
        case QMetaType::QByteArray:
            value=QJsonDocument::fromJson(v.toByteArray()).toVariant();
            break;
        default:
            return {};
        }

        switch (value.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            vRecordList.append(value);
            break;
        case QMetaType::QVariantList:
            vRecordList=value.toList();
            break;
        default:
            return {};
        }
    }

    QStringList attributeNameList;
    {//attribute
        switch (attributeNames.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            QHashIterator<QString, QVariant> i(attributeNames.toHash());
            while(i.hasNext()){
                i.next();
                auto s=i.value().toString().toLower().trimmed();
                if(!s.isEmpty())
                    attributeNameList.append(s);
            }
            break;
        }
        case QMetaType::QStringList:
        case QMetaType::QVariantList:
        {
            auto vList=attributeNames.toList();
            for(auto &v : vList){
                auto s=v.toString().toLower().trimmed();
                if(!s.isEmpty())
                    attributeNameList.append(s);
            }
            break;
        }
        default:
            auto s=attributeNames.toString().toLower().trimmed();
            if(!s.isEmpty())
                attributeNameList.append(s);
        }
    }

    if(vRecordList.isEmpty())
        return {};

    QVariantHash __return;
    for(auto &vItem:vRecordList){
        auto vHash=vItem.toHash();
        if(vHash.isEmpty())
            continue;
        QHashIterator<QString, QVariant> i(vHash);
        while(i.hasNext()){
            i.next();
            auto key=i.key().trimmed().toLower();
            if(key.isEmpty() || (!attributeNameList.isEmpty() && !attributeNameList.contains(key)))
                continue;

            auto row=__return.value(key).toList();
            if(!row.contains(i.value()))
                row.append(i.value());

            __return.insert(key,row.size()==1?row.first():row);
        }
    }
    return __return;
}

const QVariant VariantUtil::toType(int typeId, const QVariant &v)
{
    __setValue(v);
    auto __typeId=QMetaType::Type(typeId);
    switch (__typeId) {
    case QMetaType::QVariant:
        return *this;
    case QMetaType::QUuid:
        return this->toUuid(*this);
    case QMetaType::QUrl:
        return this->toUrl(*this);
    case QMetaType::QString:
        return this->toStr(*this);
    case QMetaType::QByteArray:
    case QMetaType::QBitArray:
    case QMetaType::QChar:
        return this->toByteArray(*this);
    case QMetaType::QVariantHash:
        return this->toHash(*this);
    case QMetaType::QVariantMap:
        return this->toMap(*this);
    case QMetaType::QVariantPair:
        return QVariant::fromValue(this->toPair(*this));
    case QMetaType::QStringList:
        return this->toStringList(*this);
    case QMetaType::QVariantList:
        return this->toList(*this);
    case QMetaType::Int:
    case QMetaType::UInt:
        return this->toInt(*this);
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return this->toLongLong(*this);
    case QMetaType::Double:
        return this->toDouble(*this);
    case QMetaType::QDate:
        return this->toDate(*this);
    case QMetaType::QTime:
        return this->toTime(*this);
    case QMetaType::QDateTime:
        return this->toDateTime(*this);
    case QMetaType::Bool:
        return this->toBool(*this);
    default:
        return *this;
    }
}

const QVariant VariantUtil::toVariant(const QVariant &v)
{
    __setValue(v);
    switch (this->typeId()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantPair:
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return *this;
    case QMetaType::QString:
    case QMetaType::QByteArray:
        return this->toVariantJson(*this);
    default:
        return *this;
    }
}

const QVariant VariantUtil::toVariantObject(const QVariant &v)
{
    __setValue(v);
    switch (this->typeId()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantPair:
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return *this;
    case QMetaType::QString:
    case QMetaType::QByteArray:
        return this->toVariantJson(*this);
    default:
        return *this;
    }
}

const QVariant VariantUtil::toVariantJson(const QVariant &v)
{
    __setValue(v);
    switch (this->typeId()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantPair:
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return *this;
    case QMetaType::QString:
    case QMetaType::QByteArray:
    {
        auto bytes=p->toByteArray(*this);
        if(QFile::exists(bytes)){
            QFile file(bytes);
            if(file.open(file.ReadOnly)){
                bytes=file.readAll();
                file.close();
            }
        }
        auto vv=QJsonDocument::fromJson(bytes).toVariant();
        return vv.isValid()?vv:v;
    }
    default:
        return *this;
    }
}

const QUrl VariantUtil::toUrl(const QVariant &v)
{
    if(v.isValid()) QVariant::setValue(v);
    if(v.typeId()==QMetaType::QUrl)
        return v.toUrl();

    QUrl url;
    auto surl=v.toString();
    if(QFile::exists(surl))
        url=QUrl::fromLocalFile(surl);
    else
        url=QUrl(surl);
    QVariant::setValue(url);
    return url;
}

VariantUtil &VariantUtil::makeMap(const QVariant &key, const QVariant &value)
{
    auto map=QVariant::toMap();
    map.insert(key.toString(), value);
    this->setValue(map);
    return *this;
}

VariantUtil &VariantUtil::mMap(const QVariant &key, const QVariant &value)
{
    return this->makeMap(key,value);
}

VariantUtil &VariantUtil::makeHash(const QVariant &key, const QVariant &value)
{
    auto map=QVariant::toHash();
    map.insert(key.toString(), value);
    this->setValue(map);
    return *this;
}

VariantUtil &VariantUtil::mHash(const QVariant &key, const QVariant &value)
{
    return this->makeHash(key,value);
}

VariantUtil &VariantUtil::makeList(const QVariant &value)
{
    auto list=QVariant::toList();
    list.append(value);
    this->setValue(list);
    return *this;
}

VariantUtil &VariantUtil::mList(const QVariant &value)
{
    return this->makeList(value);
}

const QUuid VariantUtil::toUuidCompuser(const QVariant &value)
{
    QStringList compuserList;
    QVariantList compuserValues;
    Q_DECLARE_VU;

    switch (value.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        compuserValues=value.toList();
        break;
    default:
        compuserValues.append(value);
    }

    if(compuserValues.isEmpty())
        return {};

    for(auto &v:compuserValues){
        QString text;
        switch (v.typeId()) {
        case QMetaType::QUuid:
            text=v.toUuid().toString().toLower().replace(QStringLiteral("{"), "").replace(QStringLiteral("}"), "").replace(QStringLiteral("-"), "");
            break;
        case QMetaType::QUrl:
            text=v.toUrl().toString().toLower().toUtf8();
            break;
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            text=QString::number(v.toLongLong(),'f',0).toUtf8();
            break;
        case QMetaType::Double:
            text=QString::number(v.toDouble(),'f').toUtf8();
            break;
        case QMetaType::QDate:
            text=v.toDate().toString(Qt::ISODate).toUtf8();
            break;
        case QMetaType::QTime:
            text=v.toTime().toString(Qt::ISODateWithMs).toUtf8();
            break;
        case QMetaType::QDateTime:
            text=v.toDateTime().toString(Qt::ISODateWithMs).toUtf8();
            break;
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            text=QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
            break;
        default:
            text=v.toByteArray();
            break;
        }
        text=text.trimmed();
        if(text.isEmpty())
            continue;
        compuserList.append(text);
    }
    if(compuserList.isEmpty())
        return {};
    auto join=compuserList.join(QStringLiteral("."));
    auto uuid=vu.toMd5Uuid(join);
    QVariant::setValue(uuid);
    return uuid;
}

VariantUtil &VariantUtil::clear()
{
    p->clear();
    return *this;
}

QVariant VariantUtil::vUnion(const QVariant &v)
{
    p->clear();
    auto newV=p->vUnion(v);
    this->setValue(newV);
    return newV;
}

QVariant VariantUtil::vUnion(const QVariant &vDestine, const QVariant &vSource)
{
    p->clear();
    auto newV=p->vUnion(vDestine,vSource);
    this->setValue(newV);
    return newV;
}

QVariant VariantUtil::vMerge(const QVariant &v)
{
    p->clear();
    auto newV=p->vMerge(v);
    this->setValue(newV);
    return newV;
}

QVariant VariantUtil::vMerge(const QVariant &vDestine, const QVariant &vSource)
{
    p->clear();
    auto newV=p->vMerge(vDestine,vSource);
    this->setValue(newV);
    return newV;
}

QVariant VariantUtil::vDeduplicate(const QVariant &v)
{
    p->clear();
    auto newV=p->vDeduplicate(v);
    this->setValue(newV);
    return newV;
}

bool VariantUtil::vIsEmpty(const QVariant &v)
{
    p->clear();
    this->setValue(v);
    auto vv=QVariant(v.isValid()?v:*this);
    if(!vv.isValid() || vv.isNull())
        return true;

    switch (this->typeId()) {
    case QMetaType::QUuid:
        return vv.toUuid().isNull();
    case QMetaType::QUrl:
        return vv.toUrl().isEmpty();
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::QBitArray:
    case QMetaType::QChar:
        return vv.toByteArray().trimmed().isEmpty();
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantPair:
        return vv.toHash().isEmpty();
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return vv.toList().isEmpty();
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Double:
        return vv.toDouble()==0;
    case QMetaType::QDate:
        return vv.toDate().isNull();
    case QMetaType::QTime:
        return vv.toTime().isNull();
    case QMetaType::QDateTime:
        return vv.toDateTime().isNull();
    default:
        return vv.isNull();
    }
}

bool VariantUtil::vIsObject(const QVariant &v)
{
    this->setValue(v);
    this->setValue(v);
    switch (this->typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantPair:
        return true;
    default:
        return false;
    }
}

bool VariantUtil::vIsList(const QVariant &v)
{
    this->setValue(v);
    switch (this->typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return true;
    default:
        return false;
    }
}

bool VariantUtil::vIsMap(const QVariant &v)
{
    this->setValue(v);
    switch (this->typeId()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantPair:
        return true;
    default:
        return false;
    }
}

bool VariantUtil::vIsString(const QVariant &v)
{
    this->setValue(v);
    switch (this->typeId()) {
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::QChar:
    case QMetaType::QBitArray:
        return true;
    default:
        return false;
    }
}

QVariant VariantUtil::convertTo(const QVariant &v, int typeId)
{
    Q_DECLARE_VU;
    switch (typeId) {
    case QMetaType::QUuid:
        return vu.toUuid(v);
    case QMetaType::Int:
    case QMetaType::UInt:
        return vu.toInt(v);
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return vu.toLongLong(v);
    case QMetaType::QDateTime:
        return vu.toDateTime(v);
    case QMetaType::QDate:
        return vu.toDate(v);
    case QMetaType::QTime:
        return vu.toTime(v);
    case QMetaType::QUrl:
        return vu.toUrl(v);
    case QMetaType::Bool:
        return vu.toBool(v);
    case QMetaType::QVariantPair:
        return vu.toHash(v);
    case QMetaType::QVariantHash:
        return vu.toHash(v);
    case QMetaType::QVariantMap:
        return vu.toMap(v);
    case QMetaType::QVariantList:
        return vu.toList(v);
    case QMetaType::QStringList:
        return vu.toStringList(v);
    default:
        return v;
    }
}

}
