#include "./qstm_util_hash.h"
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QVariantList>
#include <QStringList>
#include <QVariantHash>
#include <QVariantMap>

namespace QStm {

#define __setValue(v) if(v.isValid())QVariant::setValue(v);

class HashUtilPvt:public QObject{
public:
    HashUtil *parent=nullptr;

    explicit HashUtilPvt(HashUtil *parent):QObject{parent}
    {
    }

    static QByteArray toMd5(const QVariant &v)
    {
        if(!v.isValid() || v.isNull())
            return {};

        QByteArray bytes;

        //se ja for md5 ou uuid manteremos o uuid como md5 logo ja e um
        switch (v.typeId()) {
        case QMetaType::QUuid:
        {
            auto uuid=v.toUuid();
            if(uuid.isNull())
                return {};
            auto suuid=uuid.toByteArray();
            return suuid.replace(QByteArrayLiteral("{"), "").replace(QByteArrayLiteral("}"), "").replace(QByteArrayLiteral("-"), "");
        }
        case QMetaType::QUrl:{
            auto url=v.toUrl();
            if(!url.isValid())
                return {};
            bytes=url.toString().toUtf8();
            break;
        }
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QBitArray:
        case QMetaType::QChar:
        {
            bytes=v.toByteArray();
            auto md5=QByteArray{bytes}.replace(QByteArrayLiteral("{"), "").replace(QByteArrayLiteral("}"), "").replace(QByteArrayLiteral("-"), "");
            if(md5.length()==32){
                QString smd5;
                if(md5ParserUuid(md5, smd5)){
                    auto uuid=QUuid::fromString(QStringLiteral("{")+smd5+QStringLiteral("}"));
                    if(!uuid.isNull())
                        return md5;
                }
            }
            break;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        case QMetaType::QVariantPair:
            bytes=QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
            break;
        default:
            bytes=v.toByteArray().trimmed();
            break;
        }
        if(bytes.isEmpty())
            return {};
        return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
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

    static bool md5ParserUuid(const QString & vtext, QString &outText)
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

    static QUuid md5toUuid(const QString &md5)
    {
        auto smd5=md5;
        if(md5ParserUuid(smd5, smd5))
            return QUuid::fromString(QStringLiteral("{")+smd5+QStringLiteral("}"));

        smd5=HashUtilPvt::toMd5(md5);

        if(md5ParserUuid(smd5, smd5))
            return QUuid::fromString(QStringLiteral("{")+smd5+QStringLiteral("}"));

        return {};
    }

};

HashUtil::HashUtil(QObject *parent):QObject{parent}, p{new HashUtilPvt{this}}
{
}

bool HashUtil::isUuid(const QVariant &v)
{
    switch (v.typeId()) {
    case QMetaType::QUuid:
        return !v.toUuid().isNull();
    case QMetaType::QByteArray:
    case QMetaType::QString:
    {
        auto uuid=HashUtilPvt::md5toUuid(HashUtilPvt::toByteArray(v));
        return !uuid.isNull();
    }
    default:
        return false;
    }
}

bool HashUtil::isUuid(const QVariant &v, QUuid &uuidSet)
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
        if(HashUtilPvt::md5ParserUuid(text,text))
            uuid=QUuid::fromString(text);

        if(uuid.isNull())
            return false;

        uuidSet=uuid;
        return true;
    }
}

bool HashUtil::isHex(const QVariant &v)
{
    bool __isHex=false;
    for(auto &c:v.toString()){
        if (!isxdigit(c.toLatin1()))
            return false;
        __isHex=true;
    }
    return __isHex;
}

bool HashUtil::isBase64(const QVariant &v)
{
    auto result = QByteArray::fromBase64Encoding(v.toByteArray());
    if (result.decodingStatus == QByteArray::Base64DecodingStatus::Ok)
        return true;
    return false;
}

QByteArray HashUtil::toMd5(const QVariant &v)
{
    return HashUtilPvt::toMd5(v);
}

QByteArray HashUtil::toHex(const QVariant &v)
{
    return HashUtilPvt::toByteArray(v).toHex();
}

QUuid HashUtil::toUuid(const QVariant &v)
{
    return HashUtilPvt::md5toUuid(HashUtilPvt::toByteArray(v));
}

QString HashUtil::toUuidSimple(const QVariant &v)
{
    auto __return=HashUtil::toUuid(v).toString();
    return __return.toLower().replace(QStringLiteral("{"), QString{}).replace(QStringLiteral("}"), QString{});
}

QUuid HashUtil::toMd5Uuid(const QVariant &v)
{
    auto md5=HashUtil::toMd5(v);
    return HashUtil::toUuid(md5);
}

QUuid HashUtil::toUuidCompuser(const QVariant &value)
{
    QStringList compuserList;
    QVariantList compuserValues;

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
    auto uuid=HashUtil::toMd5Uuid(join);
    return uuid;
}

}
