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

    Q_PROPERTY(QStringList scope READ scope WRITE setScope NOTIFY scopeChanged)
    Q_PROPERTY(QString identification READ identification WRITE setIdentification NOTIFY identificationChanged)
    Q_PROPERTY(QVariantHash variables READ variables WRITE setVariables NOTIFY variablesChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant memoryLimit READ memoryLimit WRITE setMemoryLimit NOTIFY memoryLimitChanged)
    Q_PROPERTY(QVariant activityLimit READ activityLimit WRITE setActivityLimit NOTIFY activityLimitChanged)
    Q_PROPERTY(QVariant activityInterval READ activityInterval WRITE setActivityInterval NOTIFY activityIntervalChanged)
    Q_PROPERTY(QVariant activityIntervalInitial READ activityIntervalInitial WRITE setActivityIntervalInitial NOTIFY activityIntervalInitialChanged)
    Q_PROPERTY(QVariant activityThread READ activityThread WRITE setActivityThread NOTIFY activityThreadChanged)
    Q_PROPERTY(QVariant connection READ connection WRITE setConnection NOTIFY memoryLimitChanged)

    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(QVariantHash headers READ headers WRITE setHeaders NOTIFY headersChanged)
    Q_PROPERTY(QVariant protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QString driverName READ driverName WRITE setDriverName NOTIFY driverNameChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QVariant route READ route WRITE setRoute NOTIFY routeChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QVariantHash parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(QVariant body READ body WRITE setBody NOTIFY bodyChanged)
    Q_PROPERTY(int cacheInterval READ cacheInterval WRITE setCacheInterval NOTIFY cacheIntervalChanged)
    Q_PROPERTY(int cacheCleanup READ cacheCleanup WRITE setCacheCleanup NOTIFY cacheCleanupChanged)
    Q_PROPERTY(int cacheCleanupInterval READ cacheCleanupInterval WRITE setCacheCleanupInterval NOTIFY cacheCleanupIntervalChanged)
public:
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
    virtual const SettingBase &print();

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
    virtual bool macth(const QString &name);

    //!
    //! \brief isValid
    //! \return
    //!
    virtual bool isValid() const;

    //!
    //! \brief name
    //! \return
    //!
    virtual QString &name() const;

    //!
    //! \brief scope
    //! \return
    //!
    virtual QStringList &scope() const;

    //!
    //! \brief identification
    //! \return
    //!
    virtual QString &identification() const;

    //!
    //! \brief variables
    //! \return
    //!
    virtual const QVariantHash &variables() const;

    //!
    //! \brief enabled
    //! \return
    //!
    virtual bool enabled() const;
    //!
    //! \brief activityLimit
    //! \return
    //!
    virtual qlonglong activityLimit() const;

    //!
    //! \brief activityInterval
    //! \return
    //!
    virtual qlonglong activityInterval() const;

    //!
    //! \brief activityIntervalInitial
    //! \return
    //!
    virtual qlonglong activityIntervalInitial() const;

    //!
    //! \brief activityThread
    //! \return
    //!
    virtual int activityThread() const;

    //!
    //! \brief memoryLimit
    //! \return
    //!
    virtual qlonglong memoryLimit() const;

    //!
    //! \brief connections
    //! \return
    //!
    virtual QVariantHash &connection() const;

    //!
    //! \brief setScope
    //! \param value
    //!
    SettingBase &setScope(const QVariant &value);

    //!
    //! \brief setIdentification
    //! \param value
    //!
    SettingBase &setIdentification(const QString &value);

    //!
    //! \brief setName
    //! \param value
    //!
    SettingBase &setName(const QString &value);

    //!
    //! \brief setVariables
    //! \param value
    //!
    SettingBase &setVariables(const QVariantHash &value);

    //!
    //! \brief setEnabled
    //! \param value
    //!
    SettingBase &setEnabled(const bool &value);

    //!
    //! \brief setActivityLimit
    //! \param value
    //!
    SettingBase &setActivityLimit(const QVariant &value);

    //!
    //! \brief setActivityInterval
    //! \param value
    //!
    SettingBase &setActivityInterval(const QVariant &value);

    //!
    //! \brief setActivityIntervalInitial
    //! \param value
    //!
    SettingBase &setActivityIntervalInitial(const QVariant &value);

    //!
    //! \brief setActivityThread
    //! \param value
    //!
    SettingBase &setActivityThread(const QVariant &value);

    //!
    //! \brief setMemoryLimit
    //! \param value
    //!
    SettingBase &setMemoryLimit(const QVariant &value);

    //!
    //! \brief setConnections
    //! \param value
    //!
    SettingBase &setConnection(const QVariant &value);

    //!
    //! \brief url
    //! \return
    //!
    virtual QVariant &url() const;

    //!
    //! \brief service
    //! \return
    //!
    virtual QString &service() const;

    //!
    //! \brief setService
    //! \param value
    //!
    virtual SettingBase &setService(const QString &value);

    //!
    //! \brief headers
    //! \return
    //!
    virtual QVariantHash &headers() const;
    virtual SettingBase &setHeaders(const QVariantHash &value);

    //!
    //! \brief protocol
    //! \return
    //!
    virtual QVariant &protocol() const;
    virtual SettingBase &setProtocol(const QVariant &value);

    //!
    //! \brief method
    //! \return
    //!
    virtual QString method() const;
    virtual SettingBase &setMethod(const QString &value);

    //!
    //! \brief driverName
    //! \return
    //!
    virtual QString &driverName() const;
    virtual SettingBase &setDriverName(const QString &value);

    //!
    //! \brief hostName
    //! \return
    //!
    virtual QString &hostName() const;
    virtual SettingBase &setHostName(const QString &value);

    //!
    //! \brief userName
    //! \return
    //!
    virtual QString &userName() const;
    virtual SettingBase &setUserName(const QString &value);

    //!
    //! \brief password
    //! \return
    //!
    virtual QString &password() const;
    virtual SettingBase &setPassword(const QString &value);

    //!
    //! \brief port
    //! \return
    //!
    virtual int port() const;
    virtual SettingBase &setPort(int value);

    //!
    //! \brief route
    //! \return
    //!
    virtual QVariant route() const;
    virtual SettingBase &setRoute(const QVariant &value);

    //!
    //! \brief routeList
    //! \return
    //!
    virtual QVariantList &routeList() const;
    virtual QString &path() const;
    virtual SettingBase &setPath(const QString &value);

    //!
    //! \brief parameters
    //! \return
    //!
    virtual QVariantHash &parameters() const;
    virtual SettingBase &setParameters(const QVariantHash &value);

    //!
    //! \brief body
    //! \return
    //!
    virtual QVariant &body() const;
    virtual SettingBase &setBody(const QVariant &value);

    //!
    //! \brief cacheInterval
    //! \return
    //!
    virtual int cacheInterval() const;
    virtual SettingBase &setCacheInterval(const QVariant &value);

    //!
    //! \brief cacheCleanup
    //! \return
    //!
    virtual bool cacheCleanup() const;
    virtual SettingBase &setCacheCleanup(const QVariant &value);

    //!
    //! \brief cacheCleanupInterval
    //! \return
    //!
    virtual int cacheCleanupInterval() const;
    virtual SettingBase &setCacheCleanupInterval(const QVariant &value);

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
