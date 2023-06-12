#include "./p_qstm_base_setting.h"
#include "../qstm_macro.h"
#include "../qstm_util_variant.h"
#include "../qstm_util_date.h"
#include "../qstm_envs.h"

namespace QStm {

namespace Private {

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

class SettingBaseTemplatePrv: public QObject
{
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

    explicit SettingBaseTemplatePrv(const QVariantHash &vSetting, QObject *parent):QObject{parent}, parent{parent}
    {
        Q_DECLARE_VU;
        if(!vSetting.isEmpty()){
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
        }
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

    static QVariant getInterval(const QVariant &v, const QVariant &defaultV={})
    {
        Q_DECLARE_DU;
        return du.parseInterval(v,defaultV);
    }

    QVariant getMemoryBytes(const QVariant &v, const QVariant &defaultV={})
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


SettingBaseTemplate::SettingBaseTemplate(QObject *parent):QStm::Object{parent}, p{new SettingBaseTemplatePrv{{},this}}
{
}

SettingBaseTemplate::SettingBaseTemplate(const QVariantHash &vSetting, QObject *parent):QStm::Object{parent}, p{new SettingBaseTemplatePrv{vSetting, this}}
{

}

Envs &SettingBaseTemplate::envs() const
{
    return p->envs;
}

QVariant SettingBaseTemplate::envs(const QString &envName) const
{
    return p->envs.value(envName);
}

void SettingBaseTemplate::clear()
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
            property.write(this,QVariant());
        }
    }
}

void SettingBaseTemplate::print() const
{
    Q_DECLARE_VU;
    auto vHash=toHash();
    if(vHash.isEmpty())
        return;

    sInfo()<<QT_STRINGIFY(settings);
    QHashIterator<QString, QVariant> i(vHash);
    while (i.hasNext()){
        i.next();
        sInfo()<<QStringLiteral("      %1:%2").arg(i.key(), vu.toByteArray(i.value()));
    }
    sInfo()<<QStringLiteral(" ");
}

const QVariant SettingBaseTemplate::parseAlpha(const QVariant &v)
{
    return SettingBaseTemplatePrv::getAlpha(v);
}

const QVariant SettingBaseTemplate::parseNumber(const QVariant &v)
{
    return SettingBaseTemplatePrv::getNumber(v);
}

const QVariant SettingBaseTemplate::parseInterval(const QVariant &v)
{
    return SettingBaseTemplatePrv::getInterval(v, QVariant());
}

const QVariant SettingBaseTemplate::parseInterval(const QVariant &v, const QVariant &interval)
{
    return SettingBaseTemplatePrv::getInterval(v, interval);
}

bool SettingBaseTemplate::macth(const QString &name)
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

bool SettingBaseTemplate::isValid() const
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

QString &SettingBaseTemplate::name() const
{
    p->nameParser=p->envs.parser(p->name).toString();
    return p->name;
}

QStringList &SettingBaseTemplate::scope() const
{
    p->scopeParser=p->envs.parser(p->scope).toStringList();
    return p->scopeParser;
}

QString &SettingBaseTemplate::identification() const
{
    p->identificationParser=p->envs.parser(p->identification).toString();
    return p->identificationParser;
}

const QVariantHash &SettingBaseTemplate::variables() const
{
    return p->envs.customEnvs();
}

bool SettingBaseTemplate::enabled() const
{
    return p->enabled;
}

qlonglong SettingBaseTemplate::activityLimit() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityLimit);
    return du.parseInterval(v, defaultInterval).toLongLong();
}

qlonglong SettingBaseTemplate::activityInterval() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityInterval);
    return du.parseInterval(v, defaultInterval).toLongLong();
}

qlonglong SettingBaseTemplate::activityIntervalInitial() const
{
    Q_DECLARE_DU;
    auto v=this->envs().parser(p->activityIntervalInitial);
    return du.parseInterval(v, defaultInterval).toLongLong();
}

int SettingBaseTemplate::activityThread() const
{
    return (p->activityThread>0)
               ?p->activityThread
               :QThread::idealThreadCount();
}

qlonglong SettingBaseTemplate::memoryLimit() const
{
    auto v=this->envs().parser(p->memoryLimit);
    return p->getMemoryBytes(v, 0).toLongLong();
}

QVariantHash &SettingBaseTemplate::connection() const
{
    return p->connection;
}

void SettingBaseTemplate::setScope(const QVariant &value)
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
}

void SettingBaseTemplate::setIdentification(const QString &value)
{
    p->identification=value;
}

void SettingBaseTemplate::setName(const QString &value)
{
    if(p->name==value)
        return;
    p->name = value.trimmed();
}

void SettingBaseTemplate::setVariables(const QVariantHash &value)
{
    p->envs.customEnvs(value);
}

void SettingBaseTemplate::setEnabled(const bool &value)
{
    if(p->enabled==value)
        return;
    p->enabled=value;
}

void SettingBaseTemplate::setActivityLimit(const QVariant &value)
{
    if(p->activityLimit==value)
        return;
    p->activityLimit=value;
}

void SettingBaseTemplate::setActivityInterval(const QVariant &value)
{
    if(p->activityInterval==value)
        return;
    p->activityInterval=value;
}

void SettingBaseTemplate::setActivityIntervalInitial(const QVariant &value)
{
    if(p->activityInterval==value)
        return;
    p->activityIntervalInitial=value;
}

void SettingBaseTemplate::setActivityThread(const QVariant &value)
{
    if(p->activityThread==value)
        return;
    p->activityThread=value.toInt();
}

void SettingBaseTemplate::setMemoryLimit(const QVariant &value)
{
    if(p->memoryLimit==value)
        return;
    p->memoryLimit=value;
}

void SettingBaseTemplate::setConnection(const QVariant &value)
{
    if(p->connection==value)
        return;
    p->connection=value.toHash();
}

} // namespace Private
} // namespace QStm
