#include "./qstm_util_variant.h"
#include "./qstm_util_date.h"
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

namespace QStm {

#define __setValue(v) if(v.isValid()) {p->vvm.clear();this->setValue(v);}

class VariantUtilPvt{
public:
    VariantUtil *parent=nullptr;
    QVVM vvm;

    explicit VariantUtilPvt(VariantUtil*v)
    {
        this->parent=v;
    }

    virtual ~VariantUtilPvt()
    {
    }

    void clear()
    {
        vvm.clear();
        this->parent->setValue({});
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
        case QMetaType::QVariantList:
        {
            auto list=v.toList();
            return list.isEmpty()?QVariantHash{}:list.first().toHash();
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
            auto doc=QJsonDocument::fromJson(bytes);
            return doc.object().toVariantMap();
        };

        if(v.isNull() || !v.isValid())
            return {};

        switch (v.typeId()) {
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
            return v.toMap();
        case QMetaType::QString:
        case QMetaType::QByteArray:
            return makeFromJson();
        default:
            return {};
        }
    }

    static QByteArray toMd5(const QVariant &v)
    {
        //if(v.isValid()) vBase->setValue(v);
        if(!v.isValid())
            return {};

        if(v.typeId()==QMetaType::QUuid){
            auto uuid=v.toUuid();
            auto suuid=uuid.toByteArray();
            auto md5=suuid.replace(QByteArrayLiteral("{"), "").replace(QByteArrayLiteral("}"),{}).replace(QByteArrayLiteral("-"), "");
            if(md5.length()==32)
                return md5;

            return QCryptographicHash::hash(suuid, QCryptographicHash::Md5).toHex();
        }

        auto bytes=v.toByteArray().trimmed();
        if(bytes.isEmpty())
            return {};
        //se ja for md5 ou uuid manteremos o uuid como md5 logo ja e um
        if(QMetaTypeUtilObjectsString.contains(v.typeId())){
            auto md5=bytes.replace(QByteArrayLiteral("{"), "").replace(QByteArrayLiteral("}"),{}).replace(QByteArrayLiteral("-"),{});
            if(md5.length()==32)
                return md5;
        }
        return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
    }

    static QByteArray toByteArray(const QVariant &v)
    {
        auto typeId=v.typeId();
        switch (typeId) {
        case QMetaType::QUuid:
            return v.toUuid().toByteArray();
        case QMetaType::QUrl:
            return v.toUrl().toString().toUtf8();
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
        default:
            if(QMetaTypeUtilObjectMetaData.contains(typeId))
                return QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
            return v.toByteArray();
        }
    }

    static bool canConvertJson(const QVariant &v, QVariant &vOut)
    {
        vOut={};
        if(v.isNull() || !v.isValid())
            return false;

        auto typeId=v.typeId();

        if(QMetaTypeUtilObjects.contains(typeId)){
            vOut=v;
            return true;
        }

        if(QMetaTypeUtilObjectsString.contains(typeId)){
            QJsonParseError*error=nullptr;
            auto doc=QJsonDocument::fromJson(v.toByteArray(),error);
            if(error!=nullptr && (doc.isArray() || doc.isObject())){
                vOut=doc.toVariant();
                return true;
            }
        }
        return false;
    }

    bool md5ParserUuid(const QString & vtext, QString&outText)const
    {
        QByteArray suuid;
        auto text=vtext;
        text.replace(QStringLiteral("-"),"").replace(QStringLiteral("{"),"").replace(QStringLiteral("}"),"");
        if(text.length()==32){
            int i=0;
            for(auto &c:text){
                ++i;
                suuid.append(c.toLatin1());
                if(i==8 || i==12 || i==16 || i==20)
                    suuid.append(QByteArrayLiteral("-"));
            }
            outText=suuid;
            return true;
        }
        outText={};
        return false;
    }

    QUuid md5toUuid(const QString &md5)const
    {
        auto smd5=md5;
        if(md5ParserUuid(smd5, smd5))
            return QUuid::fromString(QStringLiteral("{")+smd5+QStringLiteral("}"));

        smd5=this->toMd5(md5);

        if(md5ParserUuid(smd5, smd5))
            return QUuid::fromString(QStringLiteral("{")+smd5+QStringLiteral("}"));

        return {};
    }

    QVariant vUnion(const QVariant &v)
    {
        if(!v.isValid())
            return {};

        if(QMetaTypeUtilVariantList.contains(v.typeId())){
            auto vList=v.toList();
            QVariant vA;
            while(!vList.isEmpty()){
                auto vB=vList.takeFirst();
                if(vA.typeId()==vB.typeId())
                    vA=vUnion(vA,vB);
            }
            return vA;
        }
        return {};
    }

    QVariant vUnion(const QVariant &vDestine, const QVariant &vSource)
    {
        if(vDestine.typeId()!=vSource.typeId())
            return {};

        if(QMetaTypeUtilVariantList.contains(vDestine.typeId()))
            return (vDestine.toList()+vSource.toList());

        if(QMetaTypeUtilVariantDictionary.contains(vDestine.typeId())){
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

        QByteArray rByte;
        auto ls=QVariantList{vDestine, vSource};
        for(auto &v:ls){
            rByte.append(v.toByteArray());
        }
        return rByte;
    }

    QVariant vMerge(const QVariant &v)
    {
        QVariant vOut;
        if(this->canConvertJson(v,vOut)){
            if(!vOut.isValid())
                return {};

            if(QMetaTypeUtilVariantList.contains(vOut.typeId())){
                auto vList=vOut.toList();
                if(!vList.isEmpty()){
                    auto vA=QVariant{vList.takeFirst()};
                    for(const auto &vB:vList){
                        if(vB.typeId()==vA.typeId())
                            vA=vMerge(vA, vB);
                    }
                    return vA;
                }
            }
        }
        return {};
    }

    static QVariant vMerge(const QVariant &vDestineIn, const QVariant &vSourceIn)
    {
        QVariant vDestine;
        QVariant vSource;
        if(!canConvertJson(vDestineIn, vDestine))
            vDestine=vDestineIn;

        if(!canConvertJson(vSourceIn, vSource))
            vSource=vSourceIn;

        if(vDestine.typeId()!=vSource.typeId())
            return {};

        if(QMetaTypeUtilVariantList.contains(vDestine.typeId())){
            auto dst=vDestine.toList();
            auto src=vSource.toList();
            QStringList keyList;
            QVariantList lst;
            for(const auto &v:dst){//cache de chaves existentes
                if(v.isValid()){
                    if(QMetaTypeUtilVariantDictionary.contains(v.typeId())){
                        keyList.append(toMd5(v));
                        continue;
                    }
                    keyList.append(toByteArray(v));
                }
            }
            for(auto &v:src){
                if(v.isValid()){
                    QByteArray key;
                    if(QMetaTypeUtilVariantDictionary.contains(v.typeId()))
                        key=toMd5(v);
                    else
                        key=toByteArray(v);

                    if(!lst.contains(key))
                        lst.append(v);
                }
            }
            return lst;
        }

        if(QMetaTypeUtilVariantDictionary.contains(vDestine.typeId())){
            auto aMap=vDestine.toHash();
            auto bMap=vSource.toHash();
            QHashIterator<QString, QVariant> i(bMap);
            while (i.hasNext()) {
                i.next();
                const auto &k=i.key();
                const auto &vb=i.value();
                auto &va=aMap[k];
                if(!va.isValid() || va.isNull()){
                    va=vb;
                }
                else{
                    aMap.insert(k, vMerge(va,vb));
                }
            }
            return aMap;
        }

        QByteArray rByte;
        auto ls=QVariantList{vDestine, vSource};
        for(auto &v:ls){
            rByte.append(v.toByteArray());
        }
        return rByte;
    }

    QVariant vDeduplicate(const QVariant &v){
        QVariant vRet;
        if(QMetaTypeUtilVariantDictionary.contains(v.typeId())){
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
            vRet=vAdd;
        }

        if(QMetaTypeUtilVariantList.contains(v.typeId())){
            auto vMap=v.toList();
            auto vAdd=QVariantHash();
            for(auto &v:vMap){
                const auto value=vDeduplicate(v);
                const auto key=toMd5(value);
                if(!vAdd.contains(key))
                    vAdd.insert(key,value);
            }
            vRet=vAdd.values();
        }
        return vRet;
    }

};

VariantUtil::VariantUtil(const QVariant &v):QVariant{v}
{
    this->p = new VariantUtilPvt{this};
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
    switch (v.typeId()) {
    case QMetaType::QUuid:
        return !v.toUuid().isNull();
    default:
        VariantUtil vu(v);
        auto uuid=vu.toUuid();
        return !uuid.isNull();
    }
}

bool VariantUtil::isUuid(const QVariant &v, QUuid &uuidSet) const
{

    switch (v.typeId()) {
    case QMetaType::QUuid:
    {
        uuidSet=v.toUuid();
        return !uuidSet.isNull();
    }
    default:
        QString text=v.toString();
        QUuid uuid;
        if(p->md5ParserUuid(text,text))
            uuid=QUuid::fromString(text);

        if(uuid.isNull())
            return false;

        uuidSet=uuid;
        return true;
    }
}

bool VariantUtil::isHex(const QVariant &v)const
{
    bool __isHex=false;
    for(auto &c:v.toString()){
        if (!isxdigit(c.toLatin1()))
            return false;
        __isHex=true;
    }
    return __isHex;
}

bool VariantUtil::isBase64(const QVariant &v) const
{
    auto bytes=QByteArray::fromBase64(v.toByteArray());
    return !bytes.isEmpty();
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
    return QVariant::toChar();
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
    DateUtil du(*this);
    return du.toDate();
}

const QTime VariantUtil::toTime(const QVariant &v)
{
    __setValue(v);
    DateUtil du(*this);
    return du.toTime();
}

const QDateTime VariantUtil::toDateTime(const QVariant &v)
{
    __setValue(v);
    DateUtil du(*this);
    return du.toDateTime();
}

double VariantUtil::toDouble(const QVariant &v)
{
    __setValue(v);
    return QVariant::toDouble();
}

bool VariantUtil::toBool(const QVariant &v)
{
    const static auto listBool=QVector<QByteArray>{
            QByteArrayLiteral("1"),
            QByteArrayLiteral("true"),
            QByteArrayLiteral("yes"),
            QByteArrayLiteral("sim"),
            QByteArrayLiteral("verdadeiro"),
            QByteArrayLiteral("t"),
            QByteArrayLiteral("s"),
            QByteArrayLiteral("v"),
            QByteArrayLiteral("y")};

    __setValue(v);

    if(this->typeId()==QMetaType::Bool)
        return QVariant::toBool();

    if(listBool.contains(QVariant::toByteArray().toLower().trimmed()))
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
    if(!this->isValid())
        return {};

    if(this->typeId()==QMetaType::QUuid){
        auto uuid=QVariant::toUuid();
        auto suuid=uuid.toByteArray();
        auto md5=suuid.replace(QByteArrayLiteral("{"), "").replace(QByteArrayLiteral("}"), "").replace(QByteArrayLiteral("-"), "");
        if(md5.length()==32)
            return md5;

        return QCryptographicHash::hash(suuid, QCryptographicHash::Md5).toHex();
    }

    auto bytes=this->toByteArray().trimmed();
    if(bytes.isEmpty())
        return {};
    //se ja for md5 ou uuid manteremos o uuid como md5 logo ja e um
    if(QMetaTypeUtilObjectsString.contains(this->typeId())){
        auto md5=bytes.replace(QByteArrayLiteral("{"), "").replace(QByteArrayLiteral("}"), "").replace(QByteArrayLiteral("-"), "");
        if(md5.length()==32)
            return md5;
    }
    return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
}

const QByteArray VariantUtil::toHex(const QVariant &v)
{
    __setValue(v);
    return this->toByteArray().toHex();
}

const QUuid VariantUtil::toUuid(const QVariant &v)
{
    __setValue(v);
    if(this->typeId()==QMetaType::QUuid)
        return QVariant::toUuid();
    return p->md5toUuid(this->toStr(v));
}

const QString VariantUtil::toUuidSimple(const QVariant &v)
{
    __setValue(v);
    auto __return=this->toUuid().toString();
    return __return.toLower().replace(QStringLiteral("{"), QString{}).replace(QStringLiteral("}"), QString{});
}

const QUuid VariantUtil::toMd5Uuid(const QVariant &v)
{
    auto md5=this->toMd5(v);
    return this->toUuid(md5);
}

QVVM VariantUtil::toVVM() const
{
    auto vvm=p->vvm;
    p->clear();
    return vvm;
}

const QVVM VariantUtil::toVVM(const QVariant &v)
{
    if(v.isValid())
        p->clear();
    auto map=VariantUtil::toHash();
    QHashIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        p->vvm.insert(i.key(),i.value());
    }
    return QVVM{p->vvm};
}

const QVVM VariantUtil::toVVM(const QVariant &key, const QVariant &value)
{
    p->vvm.clear();
    p->vvm.insert(key, value);
    this->setValue({});
    return p->vvm;
}

const QVariantList VariantUtil::takeList(const QByteArray &keyName)
{
    QVariantList la;
    QVariantList lb;
    if(QMetaTypeUtilVariantList.contains(this->typeId()))
        la=this->toList();
    else
        la.append(this->toHash());
    for(auto &v:la){
        if(QMetaTypeUtilVariantDictionary.contains(v.typeId())){
            auto vm=v.toHash();
            auto vv=vm.value(keyName);
            if(vv.isValid())
                lb.append(vv);
        }
    }
    return lb;
}

int VariantUtil::typeId() const
{
    if(!p->vvm.isEmpty())
        return QMetaType::QVariantMap;

    return QVariant::typeId();
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
        return v.toStringList();
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
    return {};

}

const QVariantList VariantUtil::toList(const QVariant &v)
{
    __setValue(v);
    auto newV=this->toVariantObject(v);
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
    if(!p->vvm.isEmpty()){
        QVariantMap map;
        QHashIterator<QString, QVariant> i(p->vvm);
        while (i.hasNext()) {
            i.next();
            map[i.key()]=i.value();
        }
        return map;
    }
    auto v=p->toMap(*this);
    return v;
}

const QVariantMap VariantUtil::toMap(const QVariant &v)
{
    __setValue(v);
    return p->toMap(v);
}

const QVariantMap VariantUtil::toMap(const QVariant &key, const QVariant &value)
{
    p->clear();
    this->makeMap(key,value);
    return this->toMap();
}

QVariantHash VariantUtil::toHash()const
{
    if(!p->vvm.isEmpty()){
        QVariantHash map;
        QHashIterator<QString, QVariant> i(p->vvm);
        while (i.hasNext()) {
            i.next();
            map[i.key()]=i.value();
        }
        return map;
    }

    auto v=p->toHash(*this);
    return v;
}

const QVariantHash VariantUtil::toHash(const QVariant &v)
{
    __setValue(v);
    return p->toHash(v);
}

const QVariantHash VariantUtil::toHash(const QVariant &key, const QVariant &value)
{
    p->clear();
    this->makeHash(key,value);
    return this->toHash();
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
            for(auto&v : vList){
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
    for(auto&vItem:vRecordList){
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
    switch (typeId) {
    case QMetaType::QUuid:
        return this->toUuid(v);
    case QMetaType::QUrl:
        return this->toUrl(v);
    case QMetaType::QString:
        return this->toStr(v);
    case QMetaType::QByteArray:
    case QMetaType::QBitArray:
    case QMetaType::QChar:
        return this->toByteArray(v);
    case QMetaType::QVariantHash:
        return this->toHash(v);
    case QMetaType::QVariantMap:
        return this->toMap(v);
    case QMetaType::QStringList:
        return this->toStringList(v);
    case QMetaType::QVariantList:
        return this->toList(v);
    case QMetaType::Int:
    case QMetaType::UInt:
        return this->toInt(v);
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return this->toLongLong(v);
    case QMetaType::Double:
        return this->toDouble(v);
    case QMetaType::QDate:
        return this->toDate(v);
    case QMetaType::QTime:
        return this->toTime(v);
    case QMetaType::QDateTime:
        return this->toDateTime(v);
    case QMetaType::Bool:
        return this->toBool(v);
    default:
        return {};
    }
}

const QVariant VariantUtil::toVariant(const QVariant &v)
{
    __setValue(v);
    switch (this->typeId()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
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
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return *this;
    case QMetaType::QString:
    case QMetaType::QByteArray:
    {
        auto vv=QJsonDocument::fromJson(p->toByteArray(*this)).toVariant();
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

VariantUtil &VariantUtil::makeVVM(const QVariant &key, const QVariant &value)
{
    p->vvm.insert(key, value);
    this->setValue({});
    return *this;
}

VariantUtil &VariantUtil::insert(const QVariant &key, const QVariant &value)
{
    return this->makeVVM(key, value);
}

VariantUtil &VariantUtil::makeMap(const QVariant &key, const QVariant &value)
{
    p->vvm.clear();
    auto map=QVariant::toMap();
    map[key.toString()]=value;
    this->setValue(map);
    return *this;
}

VariantUtil &VariantUtil::mMap(const QVariant &key, const QVariant &value)
{
    return this->makeMap(key,value);
}

VariantUtil &VariantUtil::makeHash(const QVariant &key, const QVariant &value)
{
    p->vvm.clear();
    auto map=QVariant::toHash();
    map[key.toString()]=value;
    this->setValue(map);
    return *this;
}

VariantUtil &VariantUtil::mHash(const QVariant &key, const QVariant &value)
{
    return this->makeHash(key,value);
}

VariantUtil &VariantUtil::makeList(const QVariant &value)
{
    p->vvm.clear();
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
    bool invalid=vv.isValid() && !v.isNull();
    if(!invalid)
        return false;

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
        if(QByteArray(vv.typeName())==QT_STRINGIFY2(QVVM))
            return vv.toHash().isEmpty();
        return vv.isNull();
    }
}

bool VariantUtil::vIsObject(const QVariant &v)
{
    p->clear();
    this->setValue(v);
    if(QMetaTypeUtilObjects.contains(this->typeId()))
        return true;
    return false;
}

bool VariantUtil::vIsList(const QVariant &v)
{
    p->clear();
    this->setValue(v);
    if(QMetaTypeUtilVariantList.contains(this->typeId()))
        return true;
    return false;
}

bool VariantUtil::vIsMap(const QVariant &v)
{
    p->clear();
    this->setValue(v);
    if(QMetaTypeUtilVariantDictionary.contains(this->typeId()))
        return true;
    if(QByteArray(v.typeName())==QT_STRINGIFY2(QVVM))
        return true;
    return false;
}

bool VariantUtil::vIsString(const QVariant &v)
{
    p->clear();
    this->setValue(v);
    if(QMetaTypeUtilString.contains(v.typeId()))
        return true;
    return false;
}

QVariant VariantUtil::convertTo(const QVariant &v, int typeId)
{
    VariantUtil vu(v);
    switch (typeId) {
    case QMetaType::QUuid:
        return vu.toUuid();
    case QMetaType::Int:
    case QMetaType::UInt:
        return vu.toInt();
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return vu.toLongLong();
    case QMetaType::QDateTime:
        return vu.toDateTime();
    case QMetaType::QDate:
        return vu.toDate();
    case QMetaType::QTime:
        return vu.toTime();
    case QMetaType::QUrl:
        return vu.toUrl();
    case QMetaType::Bool:
        return vu.toBool();
    case QMetaType::QVariantHash:
        return vu.toHash();
    case QMetaType::QVariantMap:
        return vu.toMap();
    case QMetaType::QVariantList:
        return vu.toList();
    case QMetaType::QStringList:
        return vu.toStringList();
    default:
        return v;
    }
}

}
