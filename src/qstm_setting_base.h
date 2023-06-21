#pragma once

#include "./qstm_global.h"
#include "./qstm_object.h"
#include "./qstm_envs.h"
#include <QVariant>
#include <QDateTime>
#include <QVariantHash>
#include <QVariantList>
#include <QStringList>

namespace QStm {
class SettingBasePvt;
//!
//! \brief The SettingBase class
//!
//! basic class to configuration
class Q_STM_EXPORT SettingBase:public QStm::Object
{
    Q_OBJECT

    Q_PROPERTY(QVariant scope READ scope WRITE setScope NOTIFY scopeChanged)
    Q_PROPERTY(QVariant identification READ identification WRITE setIdentification NOTIFY identificationChanged)
    Q_PROPERTY(QVariant variables READ variables WRITE setVariables NOTIFY variablesChanged)
    Q_PROPERTY(QVariant enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QVariant name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant memoryLimit READ memoryLimit WRITE setMemoryLimit NOTIFY memoryLimitChanged)
    Q_PROPERTY(QVariant activityLimit READ activityLimit WRITE setActivityLimit NOTIFY activityLimitChanged)
    Q_PROPERTY(QVariant activityInterval READ activityInterval WRITE setActivityInterval NOTIFY activityIntervalChanged)
    Q_PROPERTY(QVariant activityIntervalInitial READ activityIntervalInitial WRITE setActivityIntervalInitial NOTIFY activityIntervalInitialChanged)
    Q_PROPERTY(QVariant activityThread READ activityThread WRITE setActivityThread NOTIFY activityThreadChanged)
    Q_PROPERTY(QVariant connection READ connection WRITE setConnection NOTIFY memoryLimitChanged)
    Q_PROPERTY(QVariant headers READ headers WRITE setHeaders NOTIFY headersChanged)
    Q_PROPERTY(QVariant protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QVariant method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QVariant driverName READ driverName WRITE setDriverName NOTIFY driverNameChanged)
    Q_PROPERTY(QVariant hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QVariant userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QVariant password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QVariant port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QVariant route READ route WRITE setRoute NOTIFY routeChanged)
    Q_PROPERTY(QVariant path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QVariant parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(QVariant body READ body WRITE setBody NOTIFY bodyChanged)
    Q_PROPERTY(QVariant cacheInterval READ cacheInterval WRITE setCacheInterval NOTIFY cacheIntervalChanged)
    Q_PROPERTY(QVariant cacheCleanup READ cacheCleanup WRITE setCacheCleanup NOTIFY cacheCleanupChanged)
    Q_PROPERTY(QVariant cacheCleanupInterval READ cacheCleanupInterval WRITE setCacheCleanupInterval NOTIFY cacheCleanupIntervalChanged)
public:
    enum Method {
        HEAD = 1,
        GET = 2,
        POST = 4,
        PUT = 8,
        DELETE = 16,
        OPTIONS = 32,
        PATCH = 64,
        TRACE = 128,
    };

    Q_ENUM(Method)

    enum Protocol {TcpSocket=1, UdpSocket=2, WebSocket=4, Mqtt=8, Amqp=16, Http=32};
    Q_ENUM(Protocol)
    //!
    //! \brief SettingBase
    //! \param parent
    //!
    Q_INVOKABLE explicit SettingBase(QObject *parent=nullptr);
    explicit SettingBase(const QVariantHash &vSetting, QObject *parent=nullptr);

public:

    //!
    //! \brief envs
    //! \return
    //!
    QStm::Envs &envs()const;

    //!
    //! \brief envs
    //! \param envName
    //! \return
    //!
    QVariant envs(const QString &envName)const;

    //!
    //! \brief clear
    //!
    SettingBase &clear();

    //!
    //! \brief print
    //!
    const SettingBase &print();

    //!
    //! \brief parseInterval
    //! \param v
    //! \return
    //!
    static const QVariant parseInterval(const QVariant &v);

    //!
    //! \brief parseInterval
    //! \param v
    //! \param interval
    //! \return
    //!
    static const QVariant parseInterval(const QVariant &v, const QVariant &defaultValue);

    //!
    //! \brief macth
    //! \param name
    //! \return
    //!
    bool macth(const QString &name);

    //!
    //! \brief isValid
    //! \return
    //!
    bool isValid() const;

    //!
    //! \brief url
    //! \return
    //!
    const QVariant url() const;

    //!
    //! \brief name
    //! \return
    //!
    const QString name() const;
    SettingBase &setName(const QVariant &value);
    SettingBase &name(const QVariant &value){ return this->setName(value);}

    //!
    //! \brief scope
    //! \return
    //!
    const QStringList scope() const;
    SettingBase &setScope(const QVariant &value);
    SettingBase &scope(const QVariant &value){ return this->setScope(value);}

    //!
    //! \brief identification
    //! \return
    //!
    const QString identification() const;
    SettingBase &setIdentification(const QVariant &value);
    SettingBase &identification(const QVariant &value){ return this->setIdentification(value);}

    //!
    //! \brief variables
    //! \return
    //!
    const QVariantHash variables() const;
    SettingBase &setVariables(const QVariant &value);
    SettingBase &variables(const QVariant &value){ return this->setVariables(value);}

    //!
    //! \brief enabled
    //! \return
    //!
    bool enabled() const;
    SettingBase &setEnabled(const QVariant &value);
    SettingBase &enabled(const QVariant &value){ return this->setEnabled(value);}

    //!
    //! \brief activityLimit
    //! \return
    //!
    qlonglong activityLimit() const;
    SettingBase &setActivityLimit(const QVariant &value);
    SettingBase &activityLimit(const QVariant &value){ return this->setActivityLimit(value);}

    //!
    //! \brief activityInterval
    //! \return
    //!
    qlonglong activityInterval() const;
    SettingBase &setActivityInterval(const QVariant &value);
    SettingBase &activityInterval(const QVariant &value){ return this->setActivityInterval(value);}

    //!
    //! \brief activityIntervalInitial
    //! \return
    //!
    qlonglong activityIntervalInitial() const;
    SettingBase &setActivityIntervalInitial(const QVariant &value);
    SettingBase &activityIntervalInitial(const QVariant &value){ return this->setActivityIntervalInitial(value);}

    //!
    //! \brief activityThread
    //! \return
    //!
    int activityThread() const;
    SettingBase &setActivityThread(const QVariant &value);
    SettingBase &activityThread(const QVariant &value){ return this->setActivityThread(value);}

    //!
    //! \brief memoryLimit
    //! \return
    //!
    qlonglong memoryLimit() const;
    SettingBase &setMemoryLimit(const QVariant &value);
    SettingBase &memoryLimit(const QVariant &value){ return this->setMemoryLimit(value);}

    //!
    //! \brief connections
    //! \return
    //!
    const QVariantHash connection() const;
    SettingBase &setConnection(const QVariant &value);
    SettingBase &connection(const QVariant &value){ return this->setConnection(value);}

    //!
    //! \brief headers
    //! \return
    //!
    const QVariantHash headers() const;
    SettingBase &setHeaders(const QVariant &value);
    SettingBase &headers(const QVariant &value){ return this->setHeaders(value);}

    //!
    //! \brief protocol
    //! \return
    //!
    const QString protocol() const;
    SettingBase &setProtocol(const QVariant &value);
    SettingBase &protocol(const QVariant &value){ return this->setProtocol(value);}

    //!
    //! \brief method
    //! \return
    //!
    const QString method() const;
    SettingBase &setMethod(const QVariant &value);
    SettingBase &method(const QVariant &value){ return this->setMethod(value);}

    //!
    //! \brief driverName
    //! \return
    //!
    const QString driverName() const;
    SettingBase &setDriverName(const QVariant &value);
    SettingBase &driverName(const QVariant &value){ return this->setDriverName(value);}

    //!
    //! \brief hostName
    //! \return
    //!
    const QString hostName() const;
    SettingBase &setHostName(const QVariant &value);
    SettingBase &hostName(const QVariant &value){ return this->setHostName(value);}

    //!
    //! \brief userName
    //! \return
    //!
    const QString userName() const;
    SettingBase &setUserName(const QVariant &value);
    SettingBase &userName(const QVariant &value){ return this->setUserName(value);}

    //!
    //! \brief password
    //! \return
    //!
    const QString password() const;
    SettingBase &setPassword(const QVariant &value);
    SettingBase &password(const QVariant &value){ return this->setPassword(value);}

    //!
    //! \brief port
    //! \return
    //!
    int port() const;
    SettingBase &setPort(const QVariant &value);
    SettingBase &port(const QVariant &value){ return this->setPort(value);}

    //!
    //! \brief route
    //! \return
    //!
    QVariant route() const;
    SettingBase &setRoute(const QVariant &value);
    SettingBase &route(const QVariant &value){ return this->setRoute(value);}

    //!
    //! \brief routeList
    //! \return
    //!
    const QVariantList routeList() const;

    //!
    //! \brief path
    //! \return
    //!
    const QString path() const;
    SettingBase &setPath(const QVariant &value);
    SettingBase &path(const QVariant &value){ return this->setPath(value);}

    //!
    //! \brief parameters
    //! \return
    //!
    const QVariantHash parameters() const;
    SettingBase &setParameters(const QVariant &value);
    SettingBase &parameters(const QVariant &value){ return this->setParameters(value);}

    //!
    //! \brief body
    //! \return
    //!
    const QVariant body() const;
    SettingBase &setBody(const QVariant &value);
    SettingBase &body(const QVariant &value){ return this->setBody(value);}

    //!
    //! \brief cacheInterval
    //! \return
    //!
    int cacheInterval() const;
    SettingBase &setCacheInterval(const QVariant &value);
    SettingBase &cacheInterval(const QVariant &value){ return this->setCacheInterval(value);}

    //!
    //! \brief cacheCleanup
    //! \return
    //!
    bool cacheCleanup() const;
    SettingBase &setCacheCleanup(const QVariant &value);
    SettingBase &cacheCleanup(const QVariant &value){ return this->setCacheCleanup(value);}

    //!
    //! \brief cacheCleanupInterval
    //! \return
    //!
    int cacheCleanupInterval() const;
    SettingBase &setCacheCleanupInterval(const QVariant &value);
    SettingBase &cacheCleanupInterval(const QVariant &value){ return this->setCacheCleanupInterval(value);}

signals:
    void scopeChanged();
    void identificationChanged();
    void nameChanged();
    void variablesChanged();
    void enabledChanged();
    void activityLimitChanged();
    void activityIntervalChanged();
    void activityIntervalInitialChanged();
    void activityThreadChanged();
    void memoryLimitChanged();
    void connectionChanged();

    void serviceChanged();
    void headersChanged();
    void protocolChanged();
    void methodChanged();
    void driverNameChanged();
    void hostNameChanged();
    void userNameChanged();
    void passwordChanged();
    void portChanged();
    void routeChanged();
    void pathChanged();
    void parametersChanged();
    void bodyChanged();
    void cacheIntervalChanged();
    void cacheCleanupChanged();
    void cacheCleanupIntervalChanged();
private:
    SettingBasePvt *p=nullptr;
};


}
