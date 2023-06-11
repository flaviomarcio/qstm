#include "./p_qstm_base_setting.h"
#include "../qstm_macro.h"
#include "../qstm_util_variant.h"
#include "../qstm_util_date.h"
#include "../qstm_util_meta_object.h"
#include "../qstm_envs.h"

namespace QStm {

namespace Private {

class SettingBaseTemplatePrv: public QObject
{
public:
    QObject *parent=nullptr;
    QVariant scope, scopeParser;
    QString identification, identificationParser;
    QString name, nameParser;
    bool enabled=false;
    QVariant activityLimit=defaultLimit;
    QVariant activityInterval=defaultInterval;
    QVariant activityIntervalInitial=defaultInterval;
    int activityThread=0;
    QVariant memoryLimit=0;
    QStm::Envs variables;
    QVariantHash configs;

    explicit SettingBaseTemplatePrv(QObject *parent=nullptr):QObject{parent}
    {
        this->parent=parent;
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


SettingBaseTemplate::SettingBaseTemplate(QObject *parent):QStm::Object{parent}
{
    this->p = new SettingBaseTemplatePrv{this};
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


QVariant SettingBaseTemplate::variable(const QString &v) const
{
    return p->variables.parser(v);
}

QVariant SettingBaseTemplate::config(const QString &v) const
{
    return this->configs().value(v.trimmed().toLower());
}

QVariant SettingBaseTemplate::parseVariables(const QVariant &v) const
{
    return p->variables.parser(v);
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

QVariantMap SettingBaseTemplate::toMap() const
{
    if(!this->isValid())
        return {};

    QVariantMap __return;
    for (int row = 0; row < this->metaObject()->propertyCount(); ++row) {
        auto property=this->metaObject()->property(row);
        if(QByteArray{property.name()}==QT_STRINGIFY2(objectName))
            continue;

        const auto key=property.name();
        const auto value = property.read(this);
        if(!value.isNull())
            __return.insert(key, value);
    }
    return __return;

}

QVariantHash SettingBaseTemplate::toHash() const
{
    if(!this->isValid())
        return {};

    QVariantHash __return;
    for (int row = 0; row < this->metaObject()->propertyCount(); ++row) {
        auto property=this->metaObject()->property(row);
        if(QByteArray{property.name()}==QT_STRINGIFY2(objectName))
            continue;

        const auto key=property.name();
        const auto value = property.read(this);
        if(!value.isNull())
            __return.insert(key, value);
    }
    return __return;

}

bool SettingBaseTemplate::fromHash(const QVariantHash &v)
{
    this->clear();
    Q_DECLARE_VU;
    QVariantHash vHash;
    QHashIterator<QString, QVariant> i(v);
    while (i.hasNext()){
        i.next();
        vHash.insert(i.key().toLower(), i.value());
    }

    auto metaObject = this->metaObject();
    QStm::MetaObjectUtil metaObjectUtil(*metaObject);
    auto propertyList=metaObjectUtil.toPropertyList();
    bool __return=!v.isEmpty();
    for(auto &property:propertyList){
        if(QByteArray{property.name()}==QT_STRINGIFY2(objectName))
            continue;

        auto key = QString::fromUtf8(property.name()).toLower();

        auto value = vHash.value(key);
        if(value.isNull() || !value.isValid())
            continue;

        if(property.write(this,value))
            continue;

        switch (property.typeId()) {
        case QMetaType::Int:
        case QMetaType::UInt:
            property.write(this,value.toInt());
            break;
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            property.write(this,value.toLongLong());
            break;
        case QMetaType::Double:
            property.write(this,value.toDouble());
            break;
        case QMetaType::QVariantHash:
            property.write(this, vu.toHash(value));
            break;
        case QMetaType::QVariantMap:
            property.write(this, vu.toMap(value));
            break;
        case QMetaType::QVariantList:
            property.write(this, vu.toList(value));
            break;
        case QMetaType::QStringList:
            property.write(this, vu.toStringList(value));
            break;
        default:
            __return=false;
        }
    }
    return __return;
}

bool SettingBaseTemplate::fromMap(const QVariantMap &v)
{
    return this->fromHash(QVariant(v).toHash());
}

bool SettingBaseTemplate::fromVariant(const QVariant &v)
{
    Q_DECLARE_VU;
    auto vv=vu.toVariantObject(v);
    switch (vv.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return this->fromHash(vv.toHash());
    case QMetaType::QVariantList:{
        for(auto &h:vv.toList())
            if(!this->fromHash(h.toHash()))
                return false;
        return true;
    }
    default:
        return true;
    }
}

bool SettingBaseTemplate::fromSetting(QObject *v)
{
    auto o=dynamic_cast<QStm::Object*>(v);
    if(!o)return {};
    return this->fromHash(o->toHash());
}

bool SettingBaseTemplate::mergeHash(const QVariantHash &v)
{
    Q_DECLARE_VU;
    QVariantHash vHash;
    QHashIterator<QString, QVariant> i(v);
    while (i.hasNext()){
        i.next();
        vHash.insert(i.key().toLower(), i.value());
    }

    bool __return=false;
    auto metaObject = this->metaObject();
    QStm::MetaObjectUtil metaObjectUtil(*metaObject);
    auto propertyList=metaObjectUtil.toPropertyList();
    for(auto &property:propertyList){
        auto key = QString::fromUtf8(property.name()).toLower();

        if(key==QT_STRINGIFY2(objectName))
            continue;

        auto value = vHash.value(key);
        if(value.isNull() || !value.isValid())
            continue;

        if(property.write(this, value)){
            __return=true;
            continue;
        }

        switch (property.typeId()) {
        case QMetaType::Int:
        case QMetaType::UInt:
        {
            property.write(this,value.toInt());
            break;
        }
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        {
            if(property.write(this,value.toLongLong()))
                __return=true;
            break;
        }
        case QMetaType::Double:
        {
            if(property.write(this,value.toDouble()))
                __return=true;
            break;
        }
        case QMetaType::QVariantHash:
        {
            if(property.write(this, vu.toHash(value)))
                __return=true;
            break;
        }
        case QMetaType::QVariantMap:
        {
            if(property.write(this, vu.toMap()))
                __return=true;
            break;
        }
        case QMetaType::QVariantList:
        {
            if(property.write(this, vu.toList()))
                __return=true;
            break;
        }
        case QMetaType::QStringList:{
            if(property.write(this, vu.toStringList()))
                __return=true;
            break;
        }
        default:
            break;
        }
    }
    return __return;
}

bool SettingBaseTemplate::mergeMap(const QVariantMap &v)
{
    return this->mergeHash(QVariant(v).toHash());
}

bool SettingBaseTemplate::mergeVariant(const QVariant &v)
{
    Q_DECLARE_VU;
    auto vv=vu.toVariantObject(v);
    switch (vv.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return this->mergeHash(vv.toHash());
    case QMetaType::QVariantList:
    {
        for(auto &h:vv.toList())
            if(!this->mergeHash(h.toHash()))
                return false;
        break;
    }
    default:
        break;
    }
    return true;
}

QString &SettingBaseTemplate::name() const
{
    p->nameParser=this->parseVariables(p->name).toString();
    return p->name;
}

QVariant &SettingBaseTemplate::scope() const
{
    p->scopeParser=this->parseVariables(p->scope).toString();
    return p->scopeParser;
}

QString &SettingBaseTemplate::identification() const
{
    p->identificationParser=this->parseVariables(p->identification).toString();
    return p->identificationParser;
}

const QVariantHash &SettingBaseTemplate::variables() const
{
    return p->variables.customEnvs();
}

const QVariantHash &SettingBaseTemplate::configs() const
{
    if(!p->configs.isEmpty())
        p->configs=p->variables.parser(p->configs).toHash();
    return p->configs;
}

bool SettingBaseTemplate::enabled() const
{
    return p->enabled;
}

qlonglong SettingBaseTemplate::activityLimit() const
{
    return p->getInterval(p->activityLimit, defaultLimit).toLongLong();
}

qlonglong SettingBaseTemplate::activityInterval() const
{
    return p->getInterval(p->activityInterval, defaultInterval).toLongLong();
}

qlonglong SettingBaseTemplate::activityIntervalInitial() const
{
    return p->getInterval(p->activityIntervalInitial, defaultInterval).toLongLong();
}

qlonglong SettingBaseTemplate::activityInitial() const
{
    return p->getInterval(p->activityIntervalInitial, defaultInterval).toLongLong();
}

int SettingBaseTemplate::activityThread() const
{
    return p->activityThread>0?p->activityThread:QThread::idealThreadCount();
}

qlonglong SettingBaseTemplate::memoryLimit() const
{
    return p->getMemoryBytes(p->memoryLimit, 0).toLongLong();
}

void SettingBaseTemplate::setScope(const QVariant &value)
{
    if(p->scope==value)
        return;
    p->scope=value;
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
    p->variables.customEnvs(value);
}

void SettingBaseTemplate::setConfigs(const QVariant &value)
{
    Q_DECLARE_VU;
    p->configs.clear();
    auto vHash=vu.toHash(value);
    QHashIterator<QString, QVariant> i(vHash);
    while(i.hasNext()){
        i.next();
        p->configs.insert(i.key().trimmed().toLower(), i.value());
    }
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

} // namespace Private
} // namespace QStm
