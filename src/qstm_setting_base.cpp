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
    QVariantHash headers, headersParser;
    QVariantHash parameters, parametersParser;
    QVariant body, bodyParser;
    QString method, methodParser;
    QVariant protocol, protocolParser;
    QString driverName, driverNameParser;
    QString hostName, hostNameParser;
    QVariant route;QVariantList routeList;
    QString path, pathParser;
    QString userName, userNameParser;
    QString password, passwordParser;
    int port=-1;
    QString cacheDir;
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

QVariantHash &SettingBase::headers() const
{
    p->headersParser=this->parseVariables(p->headers).toHash();
    return p->headersParser;
}

SettingBase &SettingBase::setHeaders(const QVariantHash &value)
{
    p->headers = value;
    return *this;
}

QVariant &SettingBase::protocol() const
{
    p->protocolParser=this->parseVariables(p->protocol).toString();
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
    p->methodParser=this->parseVariables(p->method).toString();
    return p->methodParser;
}

SettingBase &SettingBase::setMethod(const QString &value)
{
    p->method=value;
    return *this;
}

QString &SettingBase::driverName() const
{
    p->driverNameParser=this->parseVariables(p->driverName).toString();
    return p->driverNameParser;
}

SettingBase &SettingBase::setDriverName(const QString &value)
{
    p->driverName=value;
    return *this;
}

QString &SettingBase::hostName() const
{
    p->hostNameParser=this->parseVariables(p->hostName).toString();
    return p->hostNameParser;
}

SettingBase &SettingBase::setHostName(const QString &value)
{
    p->hostName = value.trimmed();
    return *this;
}

QString &SettingBase::userName() const
{
    p->userNameParser=this->parseVariables(p->userName).toString();
    return p->userNameParser;
}

SettingBase &SettingBase::setUserName(const QString &value)
{
    p->userName = value.trimmed();
    return *this;
}

QString &SettingBase::password() const
{
    p->passwordParser=this->parseVariables(p->route).toString();
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
    return this->parseVariables(p->route).toString();
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
        v=this->parseVariables(v.toString().trimmed());
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
    p->pathParser=this->parseVariables(p->path).toString();
    return p->pathParser;
}

SettingBase &SettingBase::setPath(const QString &value)
{
    p->path = value.trimmed();
    return *this;
}

QVariantHash &SettingBase::parameters() const
{
    p->parametersParser=this->parseVariables(p->parameters).toHash();
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

