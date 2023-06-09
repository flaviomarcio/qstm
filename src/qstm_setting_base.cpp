#include "./qstm_setting_base.h"
#include "./qstm_meta_enum.h"
//#include "./qstm_types.h"
#include <QStandardPaths>
#include <QProcess>
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

class SettingBasePvt:public QObject{
public:
    QObject *parent=nullptr;
    QVariantHash headers;
    QVariantHash parameters;
    QVariant body;
    QString method;
    QVariant protocol;
    QString driverName;
    QString hostName;
    QVariant route;
    QString path;
    QString userName;
    QString password;
    int port=-1;
    int cacheInterval=0;
    bool cacheCleanup=false;
    int cacheCleanupInterval=0;

    explicit SettingBasePvt(QObject *parent):QObject{parent}
    {
        this->parent=parent;
    }
};


SettingBase::SettingBase(QObject *parent):SettingBaseTemplate<SettingBase>{parent}
{
    this->p = new SettingBasePvt(this);
}

QVariant SettingBase::url()const
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
    QVariant __return=(vList.size()==1)?vList.first():vList;
    return __return;
}

QString SettingBase::service() const
{
    return this->identification();
}

SettingBase &SettingBase::setService(const QString &value)
{
    this->setIdentification(value);
    return *this;
}

QVariantHash SettingBase::headers() const
{
    return p->headers;
}

SettingBase &SettingBase::setHeaders(const QVariantHash &value)
{
    p->headers = value;
    return *this;
}

QVariant SettingBase::protocol() const
{
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
    return p->method;
}

SettingBase &SettingBase::setMethod(const QString &value)
{
    p->method=value;
    return *this;
}

QString SettingBase::driverName() const
{
    return p->driverName;
}

SettingBase &SettingBase::setDriverName(const QString &value)
{
    p->driverName=value;
    return *this;
}

QString SettingBase::hostName() const
{
    return this->parseVariables(p->hostName).toString();
}

SettingBase &SettingBase::setHostName(const QString &value)
{
    p->hostName = value.trimmed();
    return *this;
}

QString SettingBase::userName() const
{
    return this->parseVariables(p->userName).toString();
}

SettingBase &SettingBase::setUserName(const QString &value)
{
    p->userName = value.trimmed();
    return *this;
}

QString SettingBase::password() const
{
    return p->password;
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
    return this->parseVariables(p->route).toString();
}

QVariantList SettingBase::routeList() const
{
    const auto &vRoute=p->route;
    QVariantList vRouteList=vRoute.toList();
    if(vRouteList.isEmpty()){
        auto route=vRoute.toString().trimmed();
        if(!route.isEmpty())
            vRouteList.append(route);
    }
    for(auto &v:vRouteList)
        v=this->parseVariables(v.toString().trimmed());
    return vRouteList;
}

SettingBase &SettingBase::setRoute(const QVariant &value)
{
    p->route = value;
    return *this;
}

QString SettingBase::path() const
{
    return this->parseVariables(p->path).toString();
}

SettingBase &SettingBase::setPath(const QString &value)
{
    p->path = value.trimmed();
    return *this;
}

QVariantHash SettingBase::parameters() const
{
    return p->parameters;
}

SettingBase &SettingBase::setParameters(const QVariantHash &value)
{
    p->parameters=value;
    return *this;
}

QVariant SettingBase::body() const
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

