#include "./qstm_meta_types.h"

namespace QStm{

Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilString, ({QMetaType::QString,QMetaType::QByteArray,QMetaType::QChar,QMetaType::QBitArray}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilMetaString, ({QMetaType::QUrl, QMetaType::QUuid, QMetaType::QString, QMetaType::QByteArray, QMetaType::QChar, QMetaType::QBitArray}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilNumeric, ({QMetaType::LongLong,QMetaType::Int,QMetaType::UInt,QMetaType::ULongLong,QMetaType::Double}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilDoubles, ({QMetaType::Double}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilIntegers, ({QMetaType::LongLong,QMetaType::Int,QMetaType::UInt,QMetaType::ULongLong,QMetaType::Double}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilClass, ({QMetaType::QUrl,QMetaType::QUuid,QMetaType::QVariantMap,QMetaType::QVariantHash,QMetaType::QVariantList,QMetaType::QStringList}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilObjects, ({QMetaType::QVariantMap,QMetaType::QVariantHash,QMetaType::QVariantList,QMetaType::QStringList}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilObjectsString, ({QMetaType::QString,QMetaType::QByteArray}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilDates, ({QMetaType::QDate,QMetaType::QDateTime,QMetaType::QTime}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilBool, ({QMetaType::Bool}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilVariantDictionary, ({QMetaType::QVariantHash, QMetaType::QVariantMap}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilVariantList, ({QMetaType::QVariantList, QMetaType::QStringList}));
Q_GLOBAL_STATIC_WITH_ARGS(QMetaTypeUtil, staticQMetaTypeUtilObjectMetaData, ({QMetaType::QVariantHash, QMetaType::QVariantMap, QMetaType::QVariantList, QMetaType::QStringList}));

QMetaTypeUtil::QMetaTypeUtil(const QVector<int> &list):QVector<int>{list}
{

}

//bool QMetaTypeUtil::qIsNumeric(const QVariant &v)
//{
//    switch (v.typeId()) {
//    case QMetaType::LongLong:
//    case QMetaType::Int:
//    case QMetaType::UInt:
//    case QMetaType::ULongLong:
//    case QMetaType::Double:
//        return true;
//    default:
//        if(v.toDouble()>0)
//            return true;
//        return false;
//    }
//}

const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilString(){ return *staticQMetaTypeUtilString; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilMetaString(){ return *staticQMetaTypeUtilMetaString; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilNumeric(){ return *staticQMetaTypeUtilNumeric; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilDoubles(){ return *staticQMetaTypeUtilDoubles; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilIntegers(){ return *staticQMetaTypeUtilIntegers; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilClass(){ return *staticQMetaTypeUtilClass; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilObjects(){ return *staticQMetaTypeUtilObjects; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilObjectsString(){ return *staticQMetaTypeUtilObjectsString; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilDates(){ return *staticQMetaTypeUtilDates; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilBool(){ return *staticQMetaTypeUtilBool; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilVariantDictionary(){ return *staticQMetaTypeUtilVariantDictionary; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilVariantList(){ return *staticQMetaTypeUtilVariantList; }
const QMetaTypeUtil &QMetaTypeUtil::qMetaTypeUtilObjectMetaData(){ return *staticQMetaTypeUtilObjectMetaData; }

}


//int qTypeId(const QVariant &v)
//{
//    return QStm::QMetaTypeUtil::v.typeId();
//}

//int qTypeId(const QMetaProperty &p)
//{
//    return QStm::QMetaTypeUtil::qTypeId(p);
//}

//bool qIsNumeric(const QVariant &v)
//{
//    return QStm::QMetaTypeUtil::qIsNumeric(v);
//}
