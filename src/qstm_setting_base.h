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

#define __Q_STM_OBJECT_CONFIG(CLASS, BASECLASS)\
public:\
CLASS &operator=(const CLASS *object)\
{\
        this->fromObject(object);\
        return *this;\
}\
CLASS &operator=(const CLASS &object)\
{\
        this->fromObject(&object);\
        return *this;\
}\
CLASS &operator=(const QVariant &object)\
{\
    this->fromVar(object);\
    return *this;\
}\
static CLASS *from(const QVariant &v, QObject *parent)\
{\
    auto item = new CLASS{parent};\
    (*item)=v;\
    if(!item->isValid()){\
        delete item;\
        return nullptr;\
    }\
    return item;\
}

#define Q_STM_OBJECT_CONFIG(CLASS) __Q_STM_OBJECT_CONFIG(CLASS, SettingBase)

//!
//! \brief The SettingBase class
//!
//! basic class to configuration
class Q_STM_EXPORT SettingBase:public QStm::Object
{
    Q_OBJECT

    __Q_STM_OBJECT_CONFIG(SettingBase, QStm::Object)

    Q_PROPERTY(QVariant scope READ scope WRITE setScope RESET resetScope NOTIFY scopeChanged)
    Q_PROPERTY(QVariant identification READ identification WRITE setIdentification RESET resetIdentification NOTIFY identificationChanged)
    Q_PROPERTY(QVariant variables READ variables WRITE setVariables RESET resetVariables NOTIFY variablesChanged)
    Q_PROPERTY(QVariant configurations READ configurations WRITE setConfigurations RESET resetConfigurations NOTIFY variablesChanged)
    Q_PROPERTY(QVariant enabled READ enabled WRITE setEnabled RESET resetEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QVariant name READ name WRITE setName RESET resetName NOTIFY nameChanged)
    Q_PROPERTY(QVariant memoryLimit READ memoryLimit WRITE setMemoryLimit RESET resetMemoryLimit NOTIFY memoryLimitChanged)
    Q_PROPERTY(QVariant activityLimit READ activityLimit WRITE setActivityLimit RESET resetActivityLimit NOTIFY activityLimitChanged)
    Q_PROPERTY(QVariant activityInterval READ activityInterval WRITE setActivityInterval RESET resetActivityInterval NOTIFY activityIntervalChanged)
    Q_PROPERTY(QVariant activityIntervalInitial READ activityIntervalInitial WRITE setActivityIntervalInitial RESET resetActivityIntervalInitial NOTIFY activityIntervalInitialChanged)
    Q_PROPERTY(QVariant activityThread READ activityThread WRITE setActivityThread RESET resetActivityThread NOTIFY activityThreadChanged)
    Q_PROPERTY(QVariant connection READ connection WRITE setConnection RESET resetConnection NOTIFY memoryLimitChanged)
    Q_PROPERTY(QVariant headers READ headers WRITE setHeaders RESET resetHeaders NOTIFY headersChanged)
    Q_PROPERTY(QVariant protocol READ protocol WRITE setProtocol RESET resetProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QVariant method READ method WRITE setMethod RESET resetMethod NOTIFY methodChanged)
    Q_PROPERTY(QVariant driverName READ driverName WRITE setDriverName RESET resetDriverName NOTIFY driverNameChanged)
    Q_PROPERTY(QVariant hostName READ hostName WRITE setHostName RESET resetHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QVariant userName READ userName WRITE setUserName RESET resetUserName NOTIFY userNameChanged)
    Q_PROPERTY(QVariant password READ password WRITE setPassword RESET resetPassword NOTIFY passwordChanged)
    Q_PROPERTY(QVariant port READ port WRITE setPort RESET resetPort NOTIFY portChanged)
    Q_PROPERTY(QVariant route READ route WRITE setRoute RESET resetRoute NOTIFY routeChanged)
    Q_PROPERTY(QVariant path READ path WRITE setPath RESET resetPath NOTIFY pathChanged)
    Q_PROPERTY(QVariant parameters READ parameters WRITE setParameters RESET resetParameters NOTIFY parametersChanged)
    Q_PROPERTY(QVariant body READ body WRITE setBody RESET resetBody NOTIFY bodyChanged)
    Q_PROPERTY(QVariant cacheInterval READ cacheInterval WRITE setCacheInterval RESET resetCacheInterval NOTIFY cacheIntervalChanged)
    Q_PROPERTY(QVariant cacheCleanup READ cacheCleanup WRITE setCacheCleanup RESET resetCacheCleanup NOTIFY cacheCleanupChanged)
    Q_PROPERTY(QVariant cacheCleanupInterval READ cacheCleanupInterval WRITE setCacheCleanupInterval RESET resetCacheCleanupInterval NOTIFY cacheCleanupIntervalChanged)
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
    SettingBase &resetName();

    //!
    //! \brief scope
    //! \return
    //!
    const QStringList scope() const;
    SettingBase &setScope(const QVariant &value);
    SettingBase &scope(const QVariant &value){ return this->setScope(value);}
    SettingBase &resetScope();

    //!
    //! \brief identification
    //! \return
    //!
    const QString identification() const;
    SettingBase &setIdentification(const QVariant &value);
    SettingBase &identification(const QVariant &value){ return this->setIdentification(value);}
    SettingBase &resetIdentification();

    //!
    //! \brief variables
    //! \return
    //!
    const QVariantHash variables() const;
    SettingBase &setVariables(const QVariant &value);
    SettingBase &variables(const QVariant &value){ return this->setVariables(value);}
    SettingBase &resetVariables();

    //!
    //! \brief variables
    //! \return
    //!
    const QVariantHash configurations() const;
    SettingBase &setConfigurations(const QVariant &value);
    SettingBase &configurations(const QVariant &value){ return this->setConfigurations(value);}
    SettingBase &resetConfigurations();

    //!
    //! \brief enabled
    //! \return
    //!
    bool enabled() const;
    SettingBase &setEnabled(const QVariant &value);
    SettingBase &enabled(const QVariant &value){ return this->setEnabled(value);}
    SettingBase &resetEnabled();

    //!
    //! \brief activityLimit
    //! \return
    //!
    qlonglong activityLimit() const;
    SettingBase &setActivityLimit(const QVariant &value);
    SettingBase &activityLimit(const QVariant &value){ return this->setActivityLimit(value);}
    SettingBase &resetActivityLimit();

    //!
    //! \brief activityInterval
    //! \return
    //!
    qlonglong activityInterval() const;
    SettingBase &setActivityInterval(const QVariant &value);
    SettingBase &activityInterval(const QVariant &value){ return this->setActivityInterval(value);}
    SettingBase &resetActivityInterval();

    //!
    //! \brief activityIntervalInitial
    //! \return
    //!
    qlonglong activityIntervalInitial() const;
    SettingBase &setActivityIntervalInitial(const QVariant &value);
    SettingBase &activityIntervalInitial(const QVariant &value){ return this->setActivityIntervalInitial(value);}
    SettingBase &resetActivityIntervalInitial();

    //!
    //! \brief activityThread
    //! \return
    //!
    int activityThread() const;
    SettingBase &setActivityThread(const QVariant &value);
    SettingBase &activityThread(const QVariant &value){ return this->setActivityThread(value);}
    SettingBase &resetActivityThread();

    //!
    //! \brief memoryLimit
    //! \return
    //!
    qlonglong memoryLimit() const;
    SettingBase &setMemoryLimit(const QVariant &value);
    SettingBase &memoryLimit(const QVariant &value){ return this->setMemoryLimit(value);}
    SettingBase &resetMemoryLimit();

    //!
    //! \brief connections
    //! \return
    //!
    const QVariantHash connection() const;
    SettingBase &setConnection(const QVariant &value);
    SettingBase &connection(const QVariant &value){ return this->setConnection(value);}
    SettingBase &resetConnection();

    //!
    //! \brief headers
    //! \return
    //!
    const QVariantHash headers() const;
    SettingBase &setHeaders(const QVariant &value);
    SettingBase &headers(const QVariant &value){ return this->setHeaders(value);}
    SettingBase &resetHeaders();

    //!
    //! \brief protocol
    //! \return
    //!
    const QString protocol() const;
    SettingBase &setProtocol(const QVariant &value);
    SettingBase &protocol(const QVariant &value){ return this->setProtocol(value);}
    SettingBase &resetProtocol();

    //!
    //! \brief method
    //! \return
    //!
    const QString method() const;
    SettingBase &setMethod(const QVariant &value);
    SettingBase &method(const QVariant &value){ return this->setMethod(value);}
    SettingBase &resetMethod();

    //!
    //! \brief driverName
    //! \return
    //!
    const QString driverName() const;
    SettingBase &setDriverName(const QVariant &value);
    SettingBase &driverName(const QVariant &value){ return this->setDriverName(value);}
    SettingBase &resetDriverName();

    //!
    //! \brief hostName
    //! \return
    //!
    const QString hostName() const;
    SettingBase &setHostName(const QVariant &value);
    SettingBase &hostName(const QVariant &value){ return this->setHostName(value);}
    SettingBase &resetHostName();

    //!
    //! \brief userName
    //! \return
    //!
    const QString userName() const;
    SettingBase &setUserName(const QVariant &value);
    SettingBase &userName(const QVariant &value){ return this->setUserName(value);}
    SettingBase &resetUserName();

    //!
    //! \brief password
    //! \return
    //!
    const QString password() const;
    SettingBase &setPassword(const QVariant &value);
    SettingBase &password(const QVariant &value){ return this->setPassword(value);}
    SettingBase &resetPassword();

    //!
    //! \brief port
    //! \return
    //!
    int port() const;
    SettingBase &setPort(const QVariant &value);
    SettingBase &port(const QVariant &value){ return this->setPort(value);}
    SettingBase &resetPort();

    //!
    //! \brief route
    //! \return
    //!
    QVariant route() const;
    SettingBase &setRoute(const QVariant &value);
    SettingBase &route(const QVariant &value){ return this->setRoute(value);}
    SettingBase &resetRoute();

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
    SettingBase &resetPath();

    //!
    //! \brief parameters
    //! \return
    //!
    const QVariantHash parameters() const;
    SettingBase &setParameters(const QVariant &value);
    SettingBase &parameters(const QVariant &value){ return this->setParameters(value);}
    SettingBase &resetParameters();

    //!
    //! \brief body
    //! \return
    //!
    const QVariant body() const;
    SettingBase &setBody(const QVariant &value);
    SettingBase &body(const QVariant &value){ return this->setBody(value);}
    SettingBase &resetBody();

    //!
    //! \brief cacheInterval
    //! \return
    //!
    int cacheInterval() const;
    SettingBase &setCacheInterval(const QVariant &value);
    SettingBase &cacheInterval(const QVariant &value){ return this->setCacheInterval(value);}
    SettingBase &resetCacheInterval();

    //!
    //! \brief cacheCleanup
    //! \return
    //!
    bool cacheCleanup() const;
    SettingBase &setCacheCleanup(const QVariant &value);
    SettingBase &cacheCleanup(const QVariant &value){ return this->setCacheCleanup(value);}
    SettingBase &resetCacheCleanup();

    //!
    //! \brief cacheCleanupInterval
    //! \return
    //!
    int cacheCleanupInterval() const;
    SettingBase &setCacheCleanupInterval(const QVariant &value);
    SettingBase &cacheCleanupInterval(const QVariant &value){ return this->setCacheCleanupInterval(value);}
    SettingBase &resetCacheCleanupInterval();

signals:
    void scopeChanged();
    void identificationChanged();
    void nameChanged();
    void variablesChanged();
    void configurationsChanged();
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
