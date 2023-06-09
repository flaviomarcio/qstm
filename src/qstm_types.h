#pragma once

//#include "./qstm_global.h"
//#include "./qstm_types_const.h"
//#include "./qstm_macro.h"
//#include "./qstm_const.h"
#include "./qstm_types.h"
#include <QObject>
#include <QVector>
#include <QVariant>
#include <QVariantHash>
#include <QString>
#include <QMetaProperty>
#include <QMutex>

//typedef QVector<QString> QStringVector;

namespace QStm {

#define Q_STM_DECLARE_MESSAGE_TYPE public:\
enum MessageType { \
    None = 1, \
    Information = 2, \
    Warning = 4, \
    Validation = 8, \
    Critical = 16, \
    Unauthorized = 32, \
    Notfound = 64, \
    BadRequest = 128, \
    UnsupportedMediaType = 256, \
    NoContent = 512, \
    NotImplemented = 1024, \
}; \
Q_ENUM(MessageType)

}










