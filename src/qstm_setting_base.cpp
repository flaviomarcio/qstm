#include "./qstm_setting_base.h"
#include "./qstm_macro.h"
#include "./qstm_meta_enum.h"
#include "./qstm_util_variant.h"
#include "./qstm_util_date.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QSettings>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <math.h>

namespace QStm {


static const auto __comma=",";
static const auto defaultLimit = "1y";
static const auto defaultInterval = "1m";

static const auto __scope="scope";
static const auto __identification="identification";
static const auto __name="name";
static const auto __enabled="enabled";
static const auto __activityLimit="activityLimit";
static const auto __activityInterval="activityInterval";
static const auto __activityIntervalInitial="activityIntervalInitial";
static const auto __activityThread="activityThread";
static const auto __memoryLimit="memoryLimit";
static const auto __connection="connection";
static const auto __configurations="configurations";
static const auto __t100ms="100ms";

static const auto __headers="headers";
static const auto __parameters="parameters";
static const auto __body="body";
static const auto __method="method";
static const auto __protocol="protocol";
static const auto __driverName="driverName";
static const auto __hostName="hostName";
static const auto __route="route";
static const auto __routeList="routeList";
static const auto __path="path";
static const auto __userName="userName";
static const auto __password="password";
static const auto __port="port";
static const auto __cacheDir="cacheDir";
static const auto __cacheInterval="cacheInterval";
static const auto __cacheCleanup="cacheCleanup";
static const auto __cacheCleanupInterval="cacheCleanupInterval";


static bool toBool(const QVariant &v)
{
    switch (v.typeId()) {
    case QMetaType::Bool:
        return v.toBool();
        break;
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::QChar:
    case QMetaType::QBitArray:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::ULong:
    case QMetaType::ULongLong:
    {
        QString s;
        switch (v.typeId()) {
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::Long:
            case QMetaType::LongLong:
            case QMetaType::ULong:
            case QMetaType::ULongLong:
                s=QString::number(v.toLongLong());
                break;
        default:
            s=v.toString().toLower();
        }
        static const QVector<QString> envsBool={"true","t","1"};
        return envsBool.contains(s);
    }
    default:
        return false;
    }

}


class SettingBasePvt:public QObject{
public:

    QObject *parent=nullptr;
    QStringList scope;
    QString identification;
    QString name;
    QVariant enabled=false;
    QVariant activityLimit=defaultLimit;
    QVariant activityInterval=defaultInterval;
    QVariant activityIntervalInitial=__t100ms;
    QVariant activityThread=1;
    QVariant memoryLimit;
    QStm::Envs envs;
    QVariant configurations;
    QVariant connection;

    QVariant headers;
    QVariant parameters;
    QVariant body;
    QVariant method;
    QVariant protocol;
    QVariant driverName;
    QVariant hostName;
    QVariant route;
    QVariant routeList;
    QVariant path;
    QVariant userName;
    QVariant password;
    QVariant port=0;
    QVariant cacheDir;
    QVariant cacheInterval=0;
    QVariant cacheCleanup=false;
    QVariant cacheCleanupInterval=0;

    explicit SettingBasePvt(QObject *parent=nullptr):QObject{parent}, parent{parent}
    {
    }

    explicit SettingBasePvt(const QVariantHash &vSetting, QObject *parent):QObject{parent}, parent{parent}
    {
        if(vSetting.isEmpty())
            return;
        Q_DECLARE_VU;
        this->scope=vu.toStringList(vSetting.value(__scope));
        this->identification=vSetting.value(__identification).toString().trimmed();
        this->name=vSetting.value(__name).toString().trimmed();
        this->enabled=vSetting.value(__enabled);
        this->activityLimit=vSetting.value(__activityLimit);
        this->activityInterval=vSetting.value(__activityInterval);
        this->activityIntervalInitial=vSetting.value(__activityIntervalInitial);
        this->activityThread=vSetting.value(__activityThread);
        this->memoryLimit=vSetting.value(__memoryLimit);
        this->connection=vSetting.value(__connection);
        this->configurations=vSetting.value(__configurations);
        this->headers=vSetting.value(__headers);
        this->parameters=vSetting.value(__parameters);
        this->body=vSetting.value(__body);
        this->method=vSetting.value(__method);
        this->protocol=vSetting.value(__protocol);
        this->driverName=vSetting.value(__driverName);
        this->hostName=vSetting.value(__hostName);
        this->route=vSetting.value(__route);
        this->routeList=vSetting.value(__routeList).toList();
        this->path=vSetting.value(__path);
        this->userName=vSetting.value(__userName);
        this->password=vSetting.value(__password);
        this->port=vSetting.value(__port).toInt();
        this->cacheDir=vSetting.value(__cacheDir);
        this->cacheInterval=vSetting.value(__cacheInterval);
        this->cacheCleanup=vSetting.value(__cacheCleanup);
        this->cacheCleanupInterval=vSetting.value(__cacheCleanupInterval);
    }


    static const QVariant getAlpha(const QVariant &v)
    {
        static auto num=QStringLiteral("0123456789,.");
        QString r;
        auto ss=v.toString();
        for(auto &v:ss){
            if(!num.contains(v))
                r+=v;
        }
        return r;
    }

    static const QVariant getNumber(const QVariant &v)
    {
        static auto num=QStringLiteral("0123456789,.");
        QString r,ss;

        switch (v.typeId()) {
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Int:
        case QMetaType::UInt:
            ss=QString::number(v.toLongLong(),'f',0);
            break;
        case QMetaType::Double:
            ss=QString::number(v.toDouble(),'f',11);
            break;
        default:
            if(v.toLongLong()>0){
                ss=QString::number(v.toLongLong(),'f',0);
                break;
            }
            ss=v.toString();
        }

        for(auto &c:ss){
            if(num.contains(c))
                r+=c;
        }
        return r;
    }

    static QVariant getMemoryBytes(const QVariant &v, const QVariant &defaultV={})
    {
        if(v.isNull() || !v.isValid() || v.toLongLong()<0)
            return defaultV;

        switch (v.typeId()) {
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Double:
            return v;
        default:
            if(v.toLongLong()>0)
                return v.toLongLong();
        }

        qlonglong scale=1;
        static qlonglong KB=1024;
        static qlonglong K=1024;

        auto a=getAlpha(v).toString().toLower();
        if(a==QStringLiteral("kb"))
            scale=1;
        else if(a==QStringLiteral("mb"))
            scale=2;
        else if(a==QStringLiteral("gb"))
            scale=3;
        else if(a==QStringLiteral("tb"))
            scale=4;
        else if(a==QStringLiteral("pb"))
            scale=5;
        else if(a==QStringLiteral("eb"))
            scale=6;
        else if(a==QStringLiteral("zb"))
            scale=7;
        else if(a==QStringLiteral("yb"))
            scale=8;
        else
            scale=1;//ms

        auto iN=getNumber(v);
        auto i=iN.toLongLong();
        i*=KB*pow(K, scale);
        if(i<=0)
            i=getMemoryBytes(defaultV).toLongLong();
        return i;
    }


};


SettingBase::SettingBase(QObject *parent):QStm::Object{parent}, p{new SettingBasePvt{this}}
{
}

SettingBase::SettingBase(const QVariantHash &vSetting, QObject *parent):QStm::Object{parent}, p{new SettingBasePvt{vSetting, this}}
{
}

Envs &SettingBase::envs() const
{
    return p->envs;
}

QVariant SettingBase::envs(const QString &envName) const
{
    return p->envs.value(envName);
}

SettingBase &SettingBase::clear()
{
    static const auto __objectName="objectName";
    for (int row = 0; row < this->metaObject()->propertyCount(); ++row) {
        auto property=this->metaObject()->property(row);

        if(!property.isResettable())
            continue;

        if(QByteArray{property.name()}==__objectName)
            continue;

        property.reset(this);
    }

    p->protocol={};
    p->activityLimit={};
    p->activityInterval={};
    p->activityIntervalInitial={};
    p->activityThread=0;
    p->memoryLimit={};
    p->envs.clear();

    return *this;
}

const SettingBase &SettingBase::print()
{
    Q_DECLARE_VU;
    auto vHash=toHash();
    if(vHash.isEmpty())
        return *this;

    sInfo()<<QT_STRINGIFY(settings);
    QHashIterator<QString, QVariant> i(vHash);
    while (i.hasNext()){
        i.next();
        sInfo()<<QStringLiteral("      %1:%2").arg(i.key(), vu.toByteArray(i.value()));
    }
    sInfo()<<QStringLiteral(" ");
    return *this;
}

//const QVariant SettingBase::parseNumber(const QVariant &v)
//{
//    Q_DECLARE_VU;
//    vu.toAlphaNumber()
//    return SettingBasePrv::getNumber(v);
//}

const QVariant SettingBase::parseInterval(const QVariant &v)
{
    Q_DECLARE_DU;
    return du.parseInterval(v,{});
}

const QVariant SettingBase::parseInterval(const QVariant &v, const QVariant &defaultValue)
{
    Q_DECLARE_DU;
    return du.parseInterval(v, defaultValue);
}

bool SettingBase::macth(const QString &name)
{
    auto i_name=this->name();
    auto startWith=i_name.contains(QByteArrayLiteral("*"));

    if(startWith)
        i_name=i_name.split(QByteArrayLiteral("*")).first();

    if(startWith && name.startsWith(i_name))
        return true;

    if((name==i_name))
        return true;

    return false;
}

bool SettingBase::isValid() const
{
    for (int row = 0; row < this->metaObject()->propertyCount(); ++row) {
        auto property=this->metaObject()->property(row);
        if(QByteArray{property.name()}==QT_STRINGIFY2(objectName))
            continue;

        auto vGet = property.read(this);
        if(!vGet.isValid())
            continue;

        auto t=vGet.typeId();
        switch (t) {
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QChar:
        case QMetaType::QBitArray:
        {
            if(vGet.toString().trimmed().isEmpty())
                continue;
            break;
        }
        case QMetaType::Double:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        {
            if(vGet.toLongLong()<=0)
                continue;
            break;
        }
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        {
            if(vGet.toHash().isEmpty())
                continue;
            break;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            if(vGet.toList().isEmpty())
                continue;
            break;
        }
        default:
            break;
        }
        return true;
    }
    return false;
}

const QVariant SettingBase::url()const
{
    QVariantList vList;
    auto vRouteList=this->routeList();
    if(vRouteList.isEmpty() && !this->route().toString().isEmpty())
        vRouteList.append(this->route());

    auto sport=(this->port()==80 || this->port()<=0)?"":QStringLiteral(":%1").arg(this->port());

    for(auto &v:vRouteList){
        auto route=v.toString().trimmed();
        auto url=QStringLiteral("%1%2/%3").arg(this->hostName(),sport,route);
        while(url.contains(QStringLiteral("//")))
            url=url.replace(QStringLiteral("//"), QStringLiteral("/"));

        vList.append(QStringLiteral("%1://%2").arg(this->protocol(),url));
    }
    return (vList.size()==1)?vList.first():vList;
}

const QString SettingBase::name() const
{
    return p->envs.parser(p->name).toString();
}

SettingBase &SettingBase::setName(const QVariant &value)
{
    if(p->name==value)
        return *this;
    Q_DECLARE_VU;
    p->name = vu.toStr(value).trimmed();
    emit nameChanged();
    return *this;
}

SettingBase &SettingBase::resetName()
{
    return this->setName({});
}

const QStringList SettingBase::scope() const
{
    return p->envs.parser(p->scope).toStringList();
}

SettingBase &SettingBase::setScope(const QVariant &value)
{
    if(value.isNull() || !value.isValid()){
        p->scope.clear();
        emit scopeChanged();
        return *this;
    }

    Q_DECLARE_VU;
    QVariantList scopeValue;
    switch (value.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantPair:
        scopeValue=vu.toList(value);
        break;
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::QChar:
    case QMetaType::QBitArray:
    {
        auto v=QJsonDocument::fromJson(value.toByteArray()).toVariant();
        switch (v.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            scopeValue=vu.toList(v);
            break;
        default:
            auto scope=value.toString().trimmed();

            if (!scope.isEmpty()){
                if(scope.contains(__comma))
                    scopeValue=vu.toList(scope.split(__comma));
                else
                    scopeValue=vu.toList(v);
            }
            break;
        }
        break;
    }
    default:
        scopeValue=vu.toList(value);
        break;
    }

    QStringList scope;
    for(auto &v: scopeValue){
        auto s=vu.toByteArray(v).trimmed();
        if(s.isEmpty())
            continue;
        scope.append(s);
    }
    p->scope=scope;
    emit scopeChanged();
    return *this;
}

SettingBase &SettingBase::resetScope()
{
    return this->setScope({});
}

const QString SettingBase::identification() const
{
    return p->envs.parser(p->identification).toString();
}

SettingBase &SettingBase::setIdentification(const QVariant &value)
{
    Q_DECLARE_VU;
    p->identification=vu.toStr(value);
    emit identificationChanged();
    return *this;
}

SettingBase &SettingBase::resetIdentification()
{
    return this->setIdentification({});
}

const QVariantHash SettingBase::variables() const
{
    return p->envs.customEnvs();
}

SettingBase &SettingBase::setVariables(const QVariant &value)
{
    p->envs.customEnvs(value);
    emit variablesChanged();
    return *this;
}

SettingBase &SettingBase::resetVariables()
{
    return this->setVariables({});
}

const QVariantHash SettingBase::configurations() const
{
    auto env=p->envs.parser(p->configurations);
    Q_DECLARE_VU;
    return vu.toHash(env);
}

SettingBase &SettingBase::setConfigurations(const QVariant &value)
{
    if(p->configurations==value)
        return *this;
    p->configurations=value.toHash();
    emit configurationsChanged();
    return *this;
}

SettingBase &SettingBase::resetConfigurations()
{
    return this->setConfigurations({});
}

bool SettingBase::enabled() const
{
    return toBool(p->envs.parser(p->enabled));
}

SettingBase &SettingBase::setEnabled(const QVariant &value)
{
    if(p->enabled==value)
        return *this;
    p->enabled=value;
    emit enabledChanged();
    return *this;
}

SettingBase &SettingBase::resetEnabled()
{
    return this->setEnabled({});
}

qlonglong SettingBase::activityLimit() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityLimit);
    return du.parseInterval(v).toLongLong();
}

SettingBase &SettingBase::setActivityLimit(const QVariant &value)
{
    if(p->activityLimit==value)
        return *this;
    p->activityLimit=value;
    emit activityLimitChanged();
    return *this;
}

SettingBase &SettingBase::resetActivityLimit()
{
    return this->setActivityLimit({});
}

qlonglong SettingBase::activityInterval() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityInterval);
    return du.parseInterval(v).toLongLong();
}

SettingBase &SettingBase::setActivityInterval(const QVariant &value)
{
    if(p->activityInterval==value)
        return *this;
    p->activityInterval=value;
    emit activityIntervalChanged();
    return *this;
}

SettingBase &SettingBase::resetActivityInterval()
{
    return this->setActivityInterval({});
}

SettingBase &SettingBase::setActivityIntervalInitial(const QVariant &value)
{
    if(p->activityInterval==value)
        return *this;
    p->activityIntervalInitial=value;
    emit activityIntervalInitialChanged();
    return *this;
}

SettingBase &SettingBase::resetActivityIntervalInitial()
{
    return this->setActivityIntervalInitial({});
}

qlonglong SettingBase::activityIntervalInitial() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityIntervalInitial);
    return du.parseInterval(v).toLongLong();
}

int SettingBase::activityThread() const
{
    return p->envs.parser(p->activityThread).toInt();
}

SettingBase &SettingBase::setActivityThread(const QVariant &value)
{
    if(p->activityThread==value)
        return *this;
    p->activityThread=value;
    emit activityThreadChanged();
    return *this;
}

SettingBase &SettingBase::resetActivityThread()
{
    return this->setActivityThread({});
}

qlonglong SettingBase::memoryLimit() const
{
    auto v=this->envs().parser(p->memoryLimit);
    return p->getMemoryBytes(v, 0).toLongLong();
}

SettingBase &SettingBase::setMemoryLimit(const QVariant &value)
{
    if(p->memoryLimit==value)
        return *this;
    p->memoryLimit=value;
    emit memoryLimitChanged();
    return *this;
}

SettingBase &SettingBase::resetMemoryLimit()
{
    return this->setMemoryLimit({});
}

const QVariantHash SettingBase::connection() const
{
    auto env=p->envs.parser(p->connection);
    Q_DECLARE_VU;
    return vu.toHash(env);
}

SettingBase &SettingBase::setConnection(const QVariant &value)
{
    if(p->connection==value)
        return *this;
    p->connection=value.toHash();
    emit connectionChanged();
    return *this;
}

SettingBase &SettingBase::resetConnection()
{
    return this->setConnection({});
}

const QVariantHash SettingBase::headers() const
{
    return this->envs().parser(p->headers).toHash();
}

SettingBase &SettingBase::setHeaders(const QVariant &value)
{
    Q_DECLARE_VU;
    p->headers = vu.toHash(value);
    emit headersChanged();
    return *this;
}

SettingBase &SettingBase::resetHeaders()
{
    return this->setHeaders({});
}

const QString SettingBase::protocol() const
{
    auto env=p->envs.parser(p->protocol).toString().trimmed();
    if(env.isEmpty())
        return {};
    MetaEnum<Protocol> e=this->envs().parser(env);
    if(!e.isValid())
        return {};
    return e.name();
}

SettingBase &SettingBase::setProtocol(const QVariant &value)
{
    Q_DECLARE_VU;
    p->protocol=vu.toStr(value);
    emit protocolChanged();
    return *this;
}

SettingBase &SettingBase::resetProtocol()
{
    return this->setProtocol({});
}

const QString SettingBase::method() const
{
    auto env=p->envs.parser(p->method).toString().trimmed();
    if(env.isEmpty())
        return {};
    MetaEnum<Method> e=this->envs().parser(p->method);
    if(!e.isValid())
        return {};
    return e.name();
}

SettingBase &SettingBase::setMethod(const QVariant &value)
{
    Q_DECLARE_VU;
    p->method=vu.toStr(value);
    emit methodChanged();
    return *this;
}

SettingBase &SettingBase::resetMethod()
{
    return this->setMethod({});
}

const QString SettingBase::driverName() const
{
    return this->envs().parser(p->driverName).toString();
}

SettingBase &SettingBase::setDriverName(const QVariant &value)
{
    Q_DECLARE_VU;
    p->driverName=vu.toStr(value);
    emit driverNameChanged();
    return *this;
}

SettingBase &SettingBase::resetDriverName()
{
    return this->setDriverName({});
}

const QString SettingBase::hostName() const
{
    return this->envs().parser(p->hostName).toString();
}

SettingBase &SettingBase::setHostName(const QVariant &value)
{
    Q_DECLARE_VU;
    p->hostName = vu.toStr(value).trimmed();
    emit hostNameChanged();
    return *this;
}

SettingBase &SettingBase::resetHostName()
{
    return this->setHostName({});
}

const QString SettingBase::userName() const
{
    return this->envs().parser(p->userName).toString();
}

SettingBase &SettingBase::setUserName(const QVariant &value)
{
    Q_DECLARE_VU;
    p->userName = vu.toStr(value).trimmed();
    emit userNameChanged();
    return *this;
}

SettingBase &SettingBase::resetUserName()
{
    return this->setUserName({});
}

const QString SettingBase::password() const
{
    return this->envs().parser(p->password).toString();
}

SettingBase &SettingBase::setPassword(const QVariant &value)
{
    Q_DECLARE_VU;
    p->password = vu.toStr(value).trimmed();
    emit passwordChanged();
    return *this;
}

SettingBase &SettingBase::resetPassword()
{
    return this->setPassword({});
}

int SettingBase::port() const
{
    return p->envs.parser(p->port).toInt();
}

SettingBase &SettingBase::setPort(const QVariant &value)
{
    Q_DECLARE_VU;
    p->port = vu.toInt(value);
    emit portChanged();
    return *this;
}

SettingBase &SettingBase::resetPort()
{
    return this->setPort({});
}

QVariant SettingBase::route() const
{
    return this->envs().parser(p->route).toString();
}

const QVariantList SettingBase::routeList() const
{
    auto env=p->envs.parser(p->route);
    Q_DECLARE_VU;
    QVariantList vRouteList=vu.toList(env);
    if(vRouteList.isEmpty()){
        auto route=p->route.toString().trimmed();
        if(!route.isEmpty())
            vRouteList.append(route);
    }
    return vRouteList.isEmpty()?QVariantList{"/"}:vRouteList;
}

SettingBase &SettingBase::setRoute(const QVariant &value)
{
    p->route = value;
    emit routeChanged();
    return *this;
}

SettingBase &SettingBase::resetRoute()
{
    return this->setRoute({});
}

const QString SettingBase::path() const
{
    return this->envs().parser(p->path).toString();
}

SettingBase &SettingBase::setPath(const QVariant &value)
{
    Q_DECLARE_VU;
    p->path = vu.toStr(value).trimmed();
    emit pathChanged();
    return *this;
}

SettingBase &SettingBase::resetPath()
{
    return this->setPath({});
}

const QVariantHash SettingBase::parameters() const
{
    return this->envs().parser(p->parameters).toHash();
}

SettingBase &SettingBase::setParameters(const QVariant &value)
{
    Q_DECLARE_VU;
    p->parameters=vu.toHash(value);
    emit parametersChanged();
    return *this;
}

SettingBase &SettingBase::resetParameters()
{
    return this->setPath({});
}

const QVariant SettingBase::body() const
{
    return p->body;
}

SettingBase &SettingBase::setBody(const QVariant &value)
{
    p->body=value;
    emit bodyChanged();
    return *this;
}

SettingBase &SettingBase::resetBody()
{
    return this->setBody({});
}

int SettingBase::cacheInterval() const
{
    return this->parseInterval(p->cacheInterval).toInt();
}

SettingBase &SettingBase::setCacheInterval(const QVariant &value)
{
    p->cacheInterval=value;
    emit cacheIntervalChanged();
    return *this;
}

SettingBase &SettingBase::resetCacheInterval()
{
    return this->setCacheInterval({});
}

bool SettingBase::cacheCleanup() const
{
    return toBool(p->envs.parser(p->cacheCleanup));
}

SettingBase &SettingBase::setCacheCleanup(const QVariant &value)
{
    p->cacheCleanup=value;
    emit cacheCleanupChanged();
    return *this;
}

SettingBase &SettingBase::resetCacheCleanup()
{
    return this->setCacheCleanup({});
}

int SettingBase::cacheCleanupInterval() const
{
    return this->parseInterval(p->cacheCleanupInterval).toInt();
}

SettingBase &SettingBase::setCacheCleanupInterval(const QVariant &value)
{
    p->cacheCleanupInterval=value;
    emit cacheCleanupIntervalChanged();
    return *this;
}

SettingBase &SettingBase::resetCacheCleanupInterval()
{
    return this->setCacheCleanupInterval({});
}

}

