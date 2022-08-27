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

typedef QVector<QString> QStringVector;


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

//!
//! \brief The RequestMethod enum
//!
enum RequestMethod {Head=1, Get=2, Post=4, Put=8, Delete=16, Options=32, Patch=64, Trace=128, MaxMethod=Trace};

//!
//! \brief The QStm::Protocol enum
//!
enum Protocol {TcpSocket=1, UdpSocket=2, WebSocket=4, Mqtt=8, Amqp=16, Http=32, Https=64, rpcProtocolMin=TcpSocket, rpcProtocolMax=Https};

namespace Private {
const QHash<int, QString> &___ProtocolName();
const QHash<QString, int> &___ProtocolType();
const QHash<QString, QStm::RequestMethod> &___RequestMethodType();
const QHash<int, QString> &___RequestMethodName();
}

//!
//! \brief RequestMethodName
//!
static const auto &RequestMethodName=QStm::Private::___RequestMethodName();

//!
//! \brief RequestMethodNameList
//!
static const auto &RequestMethodNameList=RequestMethodName.values();

//!
//! \brief RequestMethodType
//!
static const auto &RequestMethodType=QStm::Private::___RequestMethodType();

//!
//! \brief QStm::ProtocolName
//!
static const auto &ProtocolName=QStm::Private::___ProtocolName();

//!
//! \brief QStm::ProtocolType
//!
static const auto &ProtocolType=QStm::Private::___ProtocolType();

}










