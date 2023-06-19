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
static const auto __t100ms="100ms";

static const auto __url="url";
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


class SettingBasePvt:public QObject{
public:

    QObject *parent=nullptr;
    QStringList scope, scopeParser;
    QString identification, identificationParser;
    QString name, nameParser;
    bool enabled=false;
    QVariant activityLimit=defaultLimit;
    QVariant activityInterval=defaultInterval;
    QVariant activityIntervalInitial=__t100ms;
    int activityThread=0;
    QVariant memoryLimit=0;
    QStm::Envs envs;
    QVariantHash connection;

    QVariant url;
    QVariantHash headers, headersParser;
    QVariantHash parameters, parametersParser;
    QVariant body, bodyParser;
    QString method, methodParser;
    QVariant protocol, protocolParser;
    QString driverName, driverNameParser;
    QString hostName, hostNameParser;
    QVariant route;
    QVariantList routeList;
    QString path, pathParser;
    QString userName, userNameParser;
    QString password, passwordParser;
    int port=-1;
    QString cacheDir;
    int cacheInterval=0;
    bool cacheCleanup=false;
    int cacheCleanupInterval=0;

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
        this->enabled=vSetting.value(__enabled).toBool();
        this->activityLimit=vSetting.value(__activityLimit);
        this->activityInterval=vSetting.value(__activityInterval);
        this->activityIntervalInitial=vSetting.value(__activityIntervalInitial);
        this->activityThread=vSetting.value(__activityThread).toInt();
        this->memoryLimit=vSetting.value(__memoryLimit);
        this->connection=vSetting.value(__connection).toHash();


        this->url=vSetting.value(__url).toString();
        this->headers=vSetting.value(__headers).toHash();
        this->parameters=vSetting.value(__parameters).toHash();
        this->body=vSetting.value(__body);
        this->method=vSetting.value(__method).toString();
        this->protocol=vSetting.value(__protocol);
        this->driverName=vSetting.value(__driverName).toString();
        this->hostName=vSetting.value(__hostName).toString();
        this->route=vSetting.value(__route);
        this->routeList=vSetting.value(__routeList).toList();
        this->path=vSetting.value(__path).toString();
        this->userName=vSetting.value(__userName).toString();
        this->password=vSetting.value(__password).toString();
        this->port=vSetting.value(__port).toInt();
        this->cacheDir=vSetting.value(__cacheDir).toString();
        this->cacheInterval=vSetting.value(__cacheInterval).toInt();
        this->cacheCleanup=vSetting.value(__cacheCleanup).toBool();
        this->cacheCleanupInterval=vSetting.value(__cacheCleanupInterval).toInt();
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
    for (int row = 0; row < this->metaObject()->propertyCount(); ++row) {
        auto property=this->metaObject()->property(row);
        if(QByteArray{property.name()}==QT_STRINGIFY2(objectName))
            continue;

        switch (property.typeId()) {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Double:
            property.write(this,0);
            break;
        case QMetaType::QDate:
            property.write(this,QDate());
            break;
        case QMetaType::QTime:
            property.write(this,QTime());
            break;
        case QMetaType::QDateTime:
            property.write(this,QDateTime());
            break;
        case QMetaType::QVariantHash:
            property.write(this,QVariantHash());
            break;
        case QMetaType::QVariantMap:
            property.write(this,QVariant());
            break;
        case QMetaType::QVariantList:
            property.write(this,QVariantList());
            break;
        case QMetaType::QStringList:
            property.write(this,QStringList());
            break;
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QChar:
        case QMetaType::QBitArray:
            property.write(this,{});
            break;
        default:
            property.write(this, {});
        }
    }
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

QString &SettingBase::name() const
{
    p->nameParser=p->envs.parser(p->name).toString();
    return p->name;
}

QStringList &SettingBase::scope() const
{
    p->scopeParser=p->envs.parser(p->scope).toStringList();
    return p->scopeParser;
}

QString &SettingBase::identification() const
{
    p->identificationParser=p->envs.parser(p->identification).toString();
    return p->identificationParser;
}

const QVariantHash &SettingBase::variables() const
{
    return p->envs.customEnvs();
}

bool SettingBase::enabled() const
{
    return p->enabled;
}

qlonglong SettingBase::activityLimit() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityLimit);
    return du.parseInterval(v, defaultInterval).toLongLong();
}

qlonglong SettingBase::activityInterval() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityInterval);
    return du.parseInterval(v, defaultInterval).toLongLong();
}

qlonglong SettingBase::activityIntervalInitial() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityIntervalInitial);
    return du.parseInterval(v, defaultInterval).toLongLong();
}

int SettingBase::activityThread() const
{
    return (p->activityThread>0)
               ?p->activityThread
               :QThread::idealThreadCount();
}

qlonglong SettingBase::memoryLimit() const
{
    auto v=this->envs().parser(p->memoryLimit);
    return p->getMemoryBytes(v, 0).toLongLong();
}

QVariantHash &SettingBase::connection() const
{
    return p->connection;
}

SettingBase &SettingBase::setScope(const QVariant &value)
{
    Q_DECLARE_VU;
    QStringList scope;
    auto vList=value.toList();
    for(auto &v: vList){
        auto s=vu.toByteArray(v).trimmed();
        if(s.isEmpty())
            continue;
        scope.append(s);
    }
    p->scope=scope;
    emit scopeChanged();
    return *this;
}

SettingBase &SettingBase::setIdentification(const QString &value)
{
    p->identification=value;
    emit identificationChanged();
    return *this;
}

SettingBase &SettingBase::setName(const QString &value)
{
    if(p->name==value)
        return *this;
    p->name = value.trimmed();
    emit nameChanged();
    return *this;
}

SettingBase &SettingBase::setVariables(const QVariantHash &value)
{
    p->envs.customEnvs(value);
    emit variablesChanged();
    return *this;
}

SettingBase &SettingBase::setEnabled(const bool &value)
{
    if(p->enabled==value)
        return *this;
    p->enabled=value;
    emit enabledChanged();
    return *this;
}

SettingBase &SettingBase::setActivityLimit(const QVariant &value)
{
    if(p->activityLimit==value)
        return *this;
    p->activityLimit=value;
    emit activityLimitChanged();
    return *this;
}

SettingBase &SettingBase::setActivityInterval(const QVariant &value)
{
    if(p->activityInterval==value)
        return *this;
    p->activityInterval=value;
    emit activityIntervalChanged();
    return *this;
}

SettingBase &SettingBase::setActivityIntervalInitial(const QVariant &value)
{
    if(p->activityInterval==value)
        return *this;
    p->activityIntervalInitial=value;
    emit activityIntervalInitialChanged();
    return *this;
}

SettingBase &SettingBase::setActivityThread(const QVariant &value)
{
    if(p->activityThread==value)
        return *this;
    p->activityThread=value.toInt();
    emit activityThreadChanged();
    return *this;
}

SettingBase &SettingBase::setMemoryLimit(const QVariant &value)
{
    if(p->memoryLimit==value)
        return *this;
    p->memoryLimit=value;
    emit memoryLimitChanged();
    return *this;
}

SettingBase &SettingBase::setConnection(const QVariant &value)
{
    if(p->connection==value)
        return *this;
    p->connection=value.toHash();
    emit connectionChanged();
    return *this;
}

QVariant &SettingBase::url()const
{
    QVariantList vList;
    auto vRouteList=this->routeList();
    if(vRouteList.isEmpty() && !this->route().toString().isEmpty())
        vRouteList.append(this->route());

    for(auto &v:vRouteList){
        auto route=v.toString().trimmed();
        auto url=QStringLiteral("%1:%2/%3").arg(this->hostName()).arg(this->port()).arg(route);
        while(url.contains(QStringLiteral("//")))
            url=url.replace(QStringLiteral("//"), QStringLiteral("/"));


        switch (protocol().typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto record=this->protocol().toList();
            for(const auto &v:record){
                MetaEnum<Protocol> eProtocol=v;
                vList.append(QStringLiteral("%1://%2").arg(eProtocol.name(),url));
            }
            break;
        }
        default:
            const auto protocol=this->protocol().toString();
            vList.append(QStringLiteral("%1://%2").arg(protocol,url));
        }
    }
    p->url=(vList.size()==1)?vList.first():vList;
    return p->url;
}

QString &SettingBase::service() const
{
    return this->identification();
}

SettingBase &SettingBase::setService(const QString &value)
{
    this->setIdentification(value);
    return *this;
}

QVariantHash &SettingBase::headers() const
{
    p->headersParser=this->envs().parser(p->headers).toHash();
    return p->headersParser;
}

SettingBase &SettingBase::setHeaders(const QVariantHash &value)
{
    p->headers = value;
    return *this;
}

QVariant &SettingBase::protocol() const
{
    p->protocolParser=this->envs().parser(p->protocol).toString();
    return p->protocol;
}

SettingBase &SettingBase::setProtocol(const QVariant &value)
{
    MetaEnum<Protocol> eProtocol=value;
    p->protocol=eProtocol.name();
    return *this;
}

QString SettingBase::method() const
{
    p->methodParser=this->envs().parser(p->method).toString();
    return p->methodParser;
}

SettingBase &SettingBase::setMethod(const QString &value)
{
    p->method=value;
    return *this;
}

QString &SettingBase::driverName() const
{
    p->driverNameParser=this->envs().parser(p->driverName).toString();
    return p->driverNameParser;
}

SettingBase &SettingBase::setDriverName(const QString &value)
{
    p->driverName=value;
    return *this;
}

QString &SettingBase::hostName() const
{
    p->hostNameParser=this->envs().parser(p->hostName).toString();
    return p->hostNameParser;
}

SettingBase &SettingBase::setHostName(const QString &value)
{
    p->hostName = value.trimmed();
    return *this;
}

QString &SettingBase::userName() const
{
    p->userNameParser=this->envs().parser(p->userName).toString();
    return p->userNameParser;
}

SettingBase &SettingBase::setUserName(const QString &value)
{
    p->userName = value.trimmed();
    return *this;
}

QString &SettingBase::password() const
{
    p->passwordParser=this->envs().parser(p->route).toString();
    return p->passwordParser;
}

SettingBase &SettingBase::setPassword(const QString &value)
{
    p->password = value.trimmed();
    return *this;
}

int SettingBase::port() const
{
    return p->port;
}

SettingBase &SettingBase::setPort(int value)
{
    p->port = value;
    return *this;
}

QVariant SettingBase::route() const
{
    return this->envs().parser(p->route).toString();
}

QVariantList &SettingBase::routeList() const
{
    const auto &vRoute=p->route;
    QVariantList vRouteList=vRoute.toList();
    if(vRouteList.isEmpty()){
        auto route=vRoute.toString().trimmed();
        if(!route.isEmpty())
            vRouteList.append(route);
    }
    for(auto &v:vRouteList)
        v=this->envs().parser(v.toString().trimmed());
    p->routeList=vRouteList;
    return p->routeList;
}

SettingBase &SettingBase::setRoute(const QVariant &value)
{
    p->route = value;
    return *this;
}

QString &SettingBase::path() const
{
    p->pathParser=this->envs().parser(p->path).toString();
    return p->pathParser;
}

SettingBase &SettingBase::setPath(const QString &value)
{
    p->path = value.trimmed();
    return *this;
}

QVariantHash &SettingBase::parameters() const
{
    p->parametersParser=this->envs().parser(p->parameters).toHash();
    return p->parametersParser;
}

SettingBase &SettingBase::setParameters(const QVariantHash &value)
{
    p->parameters=value;
    return *this;
}

QVariant &SettingBase::body() const
{
    return p->body;
}

SettingBase &SettingBase::setBody(const QVariant &value)
{
    p->body=value;
    return *this;
}

int SettingBase::cacheInterval() const
{
    return this->parseInterval(p->cacheInterval, defaultInterval).toInt();
}

SettingBase &SettingBase::setCacheInterval(const QVariant &value)
{
    p->cacheInterval=value.toInt();
    return *this;
}

bool SettingBase::cacheCleanup() const
{
    return p->cacheCleanup;
}

SettingBase &SettingBase::setCacheCleanup(const QVariant &value)
{
    p->cacheCleanup=value.toBool();
    return *this;
}

int SettingBase::cacheCleanupInterval() const
{
    return this->parseInterval(p->cacheCleanupInterval).toInt();
}

SettingBase &SettingBase::setCacheCleanupInterval(const QVariant &value)
{
    p->cacheCleanupInterval=value.toInt();
    return *this;
}

}

