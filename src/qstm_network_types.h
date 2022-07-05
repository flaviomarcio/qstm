#pragma once

#include "./qstm_global.h"

namespace QStm {
class Q_STM_EXPORT Network:public QObject
{
    Q_OBJECT

    Q_PROPERTY(int HTTP_PORT_DEFAULT MEMBER HTTP_PORT_DEFAULT CONSTANT)
    Q_PROPERTY(int HTTP_PORT_DEBUG MEMBER HTTP_PORT_DEBUG CONSTANT)
    Q_PROPERTY(int HTTP_PORT_TEST MEMBER HTTP_PORT_TEST CONSTANT)
    Q_PROPERTY(QString HTTP_PROTOCOL MEMBER HTTP_PROTOCOL CONSTANT)
    Q_PROPERTY(QString LOCALHOST MEMBER LOCALHOST CONSTANT)
    Q_PROPERTY(QString HEADER_CONTENT_TYPE MEMBER HEADER_CONTENT_TYPE CONSTANT)
    Q_PROPERTY(QString HEADER_AUTHORIZATION MEMBER HEADER_AUTHORIZATION CONSTANT)
    Q_PROPERTY(QString AUTHORIZATION_BASIC MEMBER AUTHORIZATION_BASIC CONSTANT)
    Q_PROPERTY(QString AUTHORIZATION_BEARER MEMBER AUTHORIZATION_BEARER CONSTANT)
    Q_PROPERTY(QString APPLICATION_JSON MEMBER APPLICATION_JSON CONSTANT)
    Q_PROPERTY(QString APPLICATION_XML MEMBER APPLICATION_XML CONSTANT)
    Q_PROPERTY(QString APPLICATION_TEXT_XML MEMBER APPLICATION_TEXT_XML CONSTANT)
    Q_PROPERTY(QString APPLICATION_X_WWW_FORT_URLENCODED MEMBER APPLICATION_X_WWW_FORT_URLENCODED CONSTANT)
public:

    explicit Network(QObject*parent=nullptr);

    const int HTTP_PORT_DEFAULT=80;
    const int HTTP_PORT_DEBUG=8080;
    const int HTTP_PORT_TEST=9999;
    const QString HTTP_PROTOCOL="http";
    const QString LOCALHOST="localhost";
    const QString HEADER_CONTENT_TYPE="Content-Type";
    const QString HEADER_AUTHORIZATION="Authorization";
    const QString AUTHORIZATION_BASIC="Basic";
    const QString AUTHORIZATION_BEARER="Bearer";

    const QString APPLICATION_JSON="application/json";
    const QString APPLICATION_XML="application/xml";
    const QString APPLICATION_TEXT_XML="text/xml";
    const QString APPLICATION_X_WWW_FORT_URLENCODED="application/x-www-form-urlencoded";

    const QByteArray METHOD_GET="GET";
    const QByteArray METHOD_POST="POST";
    const QByteArray METHOD_PUT="PUT";
    const QByteArray METHOD_DELETE="DELETE";
    const QByteArray METHOD_HEAD="HEAD";
    const QByteArray METHOD_OPTIONS="OPTIONS";
    const QByteArray METHOD_PATCH="PACTH";
    const QByteArray METHOD_TRACE="TRACE";

    enum SyncMode{
        Sync, aSync
    };

    Q_ENUM(SyncMode)


    enum ContentType{
        ctJson=0,
        ctCBor=1,
        ctXml=2,
        ctBinary=3
    };

    Q_ENUM(ContentType)

    enum Method{
        GET=0,
        POST=1,
        PUT=2,
        DELETE=3,
        HEAD=4,
        OPTIONS=5
    };

    Q_ENUM(Method)

    enum Mode{
        mdSync=0, mdASync=1
    };
    Q_ENUM(Mode)

    static const QHash<int,QString> &methodName();

    //!
    //! \brief urlParsert
    //! \param protocol
    //! \param host
    //! \param port
    //! \param basePath
    //! \param path
    //! \return
    //!
    static QString urlParsert(const QString &protocol, const QString &hostName, int port, const QString &basePath, const QString &path);

};
}

