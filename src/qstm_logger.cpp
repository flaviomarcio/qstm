#include "qstm_logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <QUuid>
#include <QUrl>
#include <QDateTime>
#include <QVariantMap>
#include <QVariantHash>
#include <QVariantList>
#include <QStringList>
#include <QStringList>
#include <QJsonDocument>

namespace QStm {

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
    case QMetaType::QVariantHash:
    case QMetaType::QVariantList:
    case QMetaType::QVariantMap:
    case QMetaType::QStringList:
    case QMetaType::QVariantPair:
        return QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
    default:
        return v.toByteArray();
    }
}

//https://learn.microsoft.com/pt-br/cpp/c-runtime-library/reference/va-arg-va-copy-va-end-va-start?view=msvc-170
//https://en.cppreference.com/w/c/variadic/va_start

#define parseArgs \
va_list vl;\
int i;\
va_start( vl, format );\
for( i = 0; format[i] != '\0'; ++i ) {\
    auto arg=va_arg( vl, void* );\
    qInfo()<<toByteArray(QVariant::fromValue(arg));\
}\
va_end( vl )


//void parseArgs(const QString &format, ... )
//{
//    va_list vl;
//    int i;

//    //  szTypes is the last argument specified; you must access
//    //  all others using the variable-argument macros.
//    va_start( vl, format );
//    // Step through the list.
//    for( i = 0; format[i] != '\0'; ++i ) {
//        auto arg=va_arg( vl, void* );
//        qInfo()<<toByteArray(QVariant::fromValue(arg));
//    }
//    va_end( vl );
//}


//void parseArgs( QString szTypes, ... )
//{
//    va_list vl;
//    int i;

//    //  szTypes is the last argument specified; you must access
//    //  all others using the variable-argument macros.
//    va_start( vl, szTypes );
//    // Step through the list.
//    for( i = 0; szTypes[i] != '\0'; ++i ) {
//        auto arg=va_arg( vl, void* );
//        qInfo()<<toByteArray(QVariant::fromValue(arg));
//    }
//    va_end( vl );
//}

Logger::Logger(const QByteArray &className)
{
    this->className=className;
}

Logger &Logger::trace(QString format, ...)
{
    parseArgs;
    return *this;
}

Logger &Logger::debug(QString format, ...)
{
    parseArgs;
    return *this;
}

Logger &Logger::info(QString format, ...)
{
    parseArgs;
    return *this;
}

Logger &Logger::warn(QString format, ...)
{
    parseArgs;
    return *this;
}

Logger &Logger::error(QString format, ...)
{
    parseArgs;
    return *this;
}

} // namespace QStm
