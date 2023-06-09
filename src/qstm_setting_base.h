#pragma once

#include "./qstm_global.h"
#include "./private/p_qstm_base_setting.h"

namespace QStm {
class SettingBasePvt;
//!
//! \brief The SettingBase class
//!
//! basic class to configuration
class Q_STM_EXPORT SettingBase:public QStm::SettingBaseTemplate<SettingBase>
{
    Q_OBJECT
    QSTM_SETTING_OBJECT(SettingBase)
public:
    enum Protocol {TcpSocket=1, UdpSocket=2, WebSocket=4, Mqtt=8, Amqp=16, Http=32};
    Q_ENUM(Protocol)
    //!
    //! \brief SettingBase
    //! \param parent
    //!
    Q_INVOKABLE explicit SettingBase(QObject *parent=nullptr);

public:
    //!
    //! \brief url
    //! \return
    //!
    virtual QVariant url() const;

    //!
    //! \brief service
    //! \return
    //!
    virtual QString service() const;

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
