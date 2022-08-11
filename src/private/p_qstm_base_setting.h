#pragma once

#include "../qstm_object.h"
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

#define defaultLimit QByteArrayLiteral("1y")
#define defaultInterval QByteArrayLiteral("1m")



#define QSTM_SETTING_OBJECT(ClassName)\
public:\
ClassName &operator=(ClassName &value)\
{\
    this->fromSetting(&value);\
    return *this;\
}\
ClassName &operator=(const QVariant &value)\
{\
    this->fromVariant(value);\
    return *this;\
}\
ClassName &operator=(const QVariantMap &value)\
{\
    this->fromMap(value);\
    return *this;\
}\
ClassName &operator=(const QVariantHash &value)\
{\
    this->fromHash(value);\
    return *this;\
}\
ClassName &operator+=(const QVariant &value){\
    this->mergeVariant(value);\
    return *this;\
}\
ClassName &operator+=(const QVariantMap &value)\
{\
    this->mergeVariant(value);\
    return *this;\
}\
ClassName &operator+=(const QVariantHash &value)\
{\
    this->mergeVariant(value);\
    return *this;\
}\
ClassName &operator<<(const QVariant &value)\
{\
    this->mergeVariant(value);\
    return *this;\
}

namespace Private{
class SettingBaseTemplatePrv;
//!
//! \brief The SettingBaseTemplate class
//!
class SettingBaseTemplate:public QStm::Object
{
    Q_OBJECT
    Q_PROPERTY(QString identification READ identification WRITE setIdentification NOTIFY identificationChanged)
    Q_PROPERTY(QVariantHash variables READ variables WRITE setVariables NOTIFY variablesChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant memoryLimit READ memoryLimit WRITE setMemoryLimit NOTIFY memoryLimitChanged)
    Q_PROPERTY(QVariant activityLimit READ activityLimit WRITE setActivityLimit NOTIFY activityLimitChanged)
    Q_PROPERTY(QVariant activityInterval READ activityInterval WRITE setActivityInterval NOTIFY activityIntervalChanged)
    Q_PROPERTY(QVariant activityThread READ activityThread WRITE setActivityThread NOTIFY activityThreadChanged)

public:

    //!
    //! \brief SettingBaseTemplate
    //! \param parent
    //!
    explicit SettingBaseTemplate(QObject *parent=nullptr);

    //!
    //! \brief clear
    //!
    void clear();

    //!
    //! \brief print
    //!
    virtual void print()const;

    //!
    //! \brief parseVariables
    //! \param v
    //! \return
    //!
    QVariant parseVariables(const QVariant &v) const;

    //!
    //! \brief staticParseVariables
    //! \param v
    //! \return
    //!
    static const QVariant staticParseVariables(const QVariant &v);

    //!
    //! \brief variable
    //! \param v
    //! \return
    //!
    QVariant variable(const QString &v) const;

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
    //! \brief staticVariables
    //! \return
    //!
    const QVariantHash &staticVariables();

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
    //! \brief toMap
    //! \return
    //!
    virtual QVariantMap toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash toHash() const;

    //!
    //! \brief fromHash
    //! \param v
    //! \return
    //!
    bool fromHash(const QVariantHash &v);

    //!
    //! \brief fromMap
    //! \param v
    //! \return
    //!
    bool fromMap(const QVariantMap &v);

    //!
    //! \brief fromVariant
    //! \param v
    //! \return
    //!
    bool fromVariant(const QVariant &v);

    //!
    //! \brief fromSetting
    //! \param v
    //! \return
    //!
    bool fromSetting(QObject *v);

    //!
    //! \brief mergeHash
    //! \param v
    //! \return
    //!
    virtual bool mergeHash(const QVariantHash &v);

    //!
    //! \brief mergeMap
    //! \param v
    //! \return
    //!
    virtual bool mergeMap(const QVariantMap &v);

    //!
    //! \brief mergeVariant
    //! \param v
    //! \return
    //!
    virtual bool mergeVariant(const QVariant &v);

    //!
    //! \brief name
    //! \return
    //!
    virtual QString name() const;

    //!
    //! \brief identification
    //! \return
    //!
    virtual QString identification() const;

    //!
    //! \brief variables
    //! \return
    //!
    virtual QVariantHash &variables() const;

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
    //! \brief activityThread
    //! \return
    //!
    virtual int activityThread() const;

    //!
    //! \brief memoryLimit
    //! \return
    //!
    virtual qlonglong memoryLimit() const;

    void setIdentification(const QString &value);

    void setName(const QString &value);

    void setVariables(const QVariantHash &value);

    void setEnabled(const bool &value);

    void setActivityLimit(const QVariant &value);

    void setActivityInterval(const QVariant &value);

    void setActivityThread(const QVariant &value);

    void setMemoryLimit(const QVariant &value);

    //!
    //! \brief setStaticVariables
    //! \param v
    //!
    void setStaticVariables(const QVariantHash &v);

private:
    SettingBaseTemplatePrv*p=nullptr;
signals:
    void nameChanged();
    void variablesChanged();
    void enabledChanged();
    void activityLimitChanged();
    void activityIntervalChanged();
    void activityThreadChanged();
    void memoryLimitChanged();
    void identificationChanged();

};

}

template <class T>
class SettingBaseTemplate:public Private::SettingBaseTemplate
{
public:

    explicit SettingBaseTemplate(QObject *parent=nullptr):Private::SettingBaseTemplate{parent}
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
};

}
