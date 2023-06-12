#pragma once

#include "../qstm_object.h"
#include "../qstm_envs.h"
#include <math.h>
#include <QVariant>
#include <QVariantHash>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QDateTime>
#include <QVariantHash>
#include <QVariantList>
#include <QVariantHash>
#include <QStringList>
#include <QFile>
#include <QSettings>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonObject>


namespace QStm {

static const auto defaultLimit = "1y";
static const auto defaultInterval = "1m";

namespace Private{
class SettingBaseTemplatePrv;
//!
//! \brief The SettingBaseTemplate class
//!
class SettingBaseTemplate:public QStm::Object
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

public:

    //!
    //! \brief SettingBaseTemplate
    //! \param parent
    //!
    Q_INVOKABLE explicit SettingBaseTemplate(QObject *parent=nullptr);
    explicit SettingBaseTemplate(const QVariantHash &vSetting, QObject *parent=nullptr);

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
    void clear();

    //!
    //! \brief print
    //!
    virtual void print()const;

    //!
    //! \brief parseAlpha
    //! \param v
    //! \return
    //!
    static const QVariant parseAlpha(const QVariant &v);

    //!
    //! \brief parseNumber
    //! \param v
    //! \return
    //!
    static const QVariant parseNumber(const QVariant &v);

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
    static const QVariant parseInterval(const QVariant &v, const QVariant &interval);

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
    void setScope(const QVariant &value);

    //!
    //! \brief setIdentification
    //! \param value
    //!
    void setIdentification(const QString &value);

    //!
    //! \brief setName
    //! \param value
    //!
    void setName(const QString &value);

    //!
    //! \brief setVariables
    //! \param value
    //!
    void setVariables(const QVariantHash &value);

    //!
    //! \brief setEnabled
    //! \param value
    //!
    void setEnabled(const bool &value);

    //!
    //! \brief setActivityLimit
    //! \param value
    //!
    void setActivityLimit(const QVariant &value);

    //!
    //! \brief setActivityInterval
    //! \param value
    //!
    void setActivityInterval(const QVariant &value);

    //!
    //! \brief setActivityIntervalInitial
    //! \param value
    //!
    void setActivityIntervalInitial(const QVariant &value);

    //!
    //! \brief setActivityThread
    //! \param value
    //!
    void setActivityThread(const QVariant &value);

    //!
    //! \brief setMemoryLimit
    //! \param value
    //!
    void setMemoryLimit(const QVariant &value);

    //!
    //! \brief setConnections
    //! \param value
    //!
    void setConnection(const QVariant &value);

private:
    SettingBaseTemplatePrv*p=nullptr;
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

};

}

template <class T>
class SettingBaseTemplate:public Private::SettingBaseTemplate
{
public:

    explicit SettingBaseTemplate(QObject *parent=nullptr):Private::SettingBaseTemplate{parent}
    {
    }

    explicit SettingBaseTemplate(const QVariantHash &vSetting, QObject *parent=nullptr):Private::SettingBaseTemplate{vSetting, parent}
    {
    }

    //!
    //! \brief clear
    //!
    virtual T &clear()
    {
        Private::SettingBaseTemplate::clear();
        return *dynamic_cast<T*>(this);
    }

    virtual T &setScope(const QVariant &value)
    {
        Private::SettingBaseTemplate::setScope(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setIdentification(const QString &value)
    {
        Private::SettingBaseTemplate::setIdentification(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setName(const QString &value)
    {
        Private::SettingBaseTemplate::setName(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setVariables(const QVariantHash &value)
    {
        Private::SettingBaseTemplate::setVariables(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setEnabled(const bool &value)
    {
        Private::SettingBaseTemplate::setEnabled(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setActivityLimit(const QVariant &value)
    {
        Private::SettingBaseTemplate::setActivityLimit(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setActivityInterval(const QVariant &value)
    {
        Private::SettingBaseTemplate::setActivityInterval(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setActivityThread(const QVariant &value)
    {
        Private::SettingBaseTemplate::setActivityThread(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setMemoryLimit(const QVariant &value)
    {
        Private::SettingBaseTemplate::setMemoryLimit(value);
        return *dynamic_cast<T*>(this);
    }

    virtual T &setConnection(const QVariant &value)
    {
        Private::SettingBaseTemplate::setConnection(value);
        return *dynamic_cast<T*>(this);
    }
};

}
