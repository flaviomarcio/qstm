#include "./p_qstm_base_setting.h"
#include "../qstm_startup.h"

namespace QStm {

namespace Private {

typedef QStandardPaths StdPaths;

Q_GLOBAL_STATIC(QVariantHash, static_dirs)
Q_GLOBAL_STATIC(QVariantHash, static_variables)

static void make_static_variables()
{
    auto &d = *static_dirs;

#ifndef Q_OS_IOS
    QProcess process;
    auto lst = process.environment();
    for (auto &v : lst) {
        auto s = v.split(qsl("="));
        auto env = s.first().trimmed();
        auto value = s.last().trimmed();
        d[env] = value;
    }
#endif
    d[qsl("$DesktopLocation")] = StdPaths::writableLocation(StdPaths::DesktopLocation);
    d[qsl("$DocumentsLocation")] = StdPaths::writableLocation(StdPaths::DocumentsLocation);
    d[qsl("$FontsLocation")] = StdPaths::writableLocation(StdPaths::FontsLocation);
    d[qsl("$ApplicationsLocation")] = StdPaths::writableLocation(StdPaths::ApplicationsLocation);
    d[qsl("$MusicLocation")] = StdPaths::writableLocation(StdPaths::MusicLocation);
    d[qsl("$MoviesLocation")] = StdPaths::writableLocation(StdPaths::MoviesLocation);
    d[qsl("$PicturesLocation")] = StdPaths::writableLocation(StdPaths::PicturesLocation);
    d[qsl("$TempLocation")] = StdPaths::writableLocation(StdPaths::TempLocation);
    d[qsl("$HomeLocation")] = StdPaths::writableLocation(StdPaths::HomeLocation);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    d[qsl("$DataLocation")] = StdPaths::writableLocation(StdPaths::DataLocation);
#endif
    d[qsl("$CacheLocation")] = StdPaths::writableLocation(StdPaths::CacheLocation);
    d[qsl("$GenericDataLocation")] = StdPaths::writableLocation(StdPaths::GenericDataLocation);
    d[qsl("$RuntimeLocation")] = StdPaths::writableLocation(StdPaths::RuntimeLocation);
    d[qsl("$ConfigLocation")] = StdPaths::writableLocation(StdPaths::ConfigLocation);
    d[qsl("$DownloadLocation")] = StdPaths::writableLocation(StdPaths::DownloadLocation);
    d[qsl("$GenericCacheLocation")] = StdPaths::writableLocation(StdPaths::GenericCacheLocation);
    d[qsl("$GenericConfigLocation")] = StdPaths::writableLocation(StdPaths::GenericConfigLocation);
    d[qsl("$AppDataLocation")] = StdPaths::writableLocation(StdPaths::AppDataLocation);
    d[qsl("$AppConfigLocation")] = StdPaths::writableLocation(StdPaths::AppConfigLocation);
    d[qsl("$AppLocalDataLocation")] = StdPaths::writableLocation(StdPaths::AppLocalDataLocation);
    d[qsl("$HOME")] = StdPaths::writableLocation(StdPaths::HomeLocation);
    d[qsl("$TEMPDIR")] = StdPaths::writableLocation(StdPaths::TempLocation);
    d[qsl("$APPDIR")] = StdPaths::writableLocation(StdPaths::ApplicationsLocation);
    d[qsl("$APPNAME")] = qApp->applicationName();

    *static_variables=d;
}

void init()
{
    make_static_variables();
}

Q_COREAPP_STARTUP_FUNCTION(init);

class SettingBaseTemplatePrv: public QObject
{
public:
    QObject*parent=nullptr;
    QString identification;
    QVariantHash variables;
    QString name;
    bool enabled=false;
    QVariant activityLimit=defaultLimit;
    QVariant activityInterval=defaultInterval;
    int activityThread=0;
    QVariant memoryLimit=0;

    explicit SettingBaseTemplatePrv(QObject*parent=nullptr):QObject{parent}
    {
        this->parent=parent;
    }

    virtual ~SettingBaseTemplatePrv()
    {
    }

    static QVariant staticReplaceString(const QVariantHash&static_variables, const QVariant &v)
    {
        QString value;
        auto typeId=qTypeId(v);
        switch (typeId) {
        case QMetaType_QVariantMap:
        case QMetaType_QVariantHash:
        case QMetaType_QVariantList:
        case QMetaType_QStringList:
            value=QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
            break;
        default:
            value=v.toByteArray();
        }

        if(!value.contains(qsl("$")))
            return v;

        QHashIterator<QString, QVariant> i(static_variables);
        while (i.hasNext()) {
            i.next();
            auto key=i.key();
            const auto k=QString(qsl("$")+key)
                    .replace(qsl("$$"),qsl("$"))
                    .replace(qsl("${%1}").arg(key), qsl("$%1").arg(key));
            const auto v=i.value().toString().trimmed();
            value=value.replace(k,v);
        }

        switch (typeId) {
        case QMetaType_QVariantMap:
        case QMetaType_QVariantHash:
        case QMetaType_QVariantList:
        case QMetaType_QStringList:
            return QJsonDocument::fromJson(value.toUtf8()).toVariant();
        default:
            return value;
        }

    }

    static const QVariant replaceEnvStatic(const QVariant &v)
    {
        auto value=v;
        value=staticReplaceString(*static_variables, value);
        return value;
    }

    QVariant replaceEnv(const QVariant &v)const
    {
        auto value=v;
        value=staticReplaceString(*static_variables, value);
        value=staticReplaceString(variables, value);
        return value;
    }

    static const QVariant getAlpha(const QVariant &v)
    {
        static auto num=qsl("0123456789,.");
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
        static auto num=qsl("0123456789,.");
        QString r,ss;

        switch (qTypeId(v)) {
        case QMetaType_LongLong:
        case QMetaType_ULongLong:
        case QMetaType_Int:
        case QMetaType_UInt:
            ss=QString::number(v.toLongLong(),'f',0);
            break;
        case QMetaType_Double:
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

        switch (qTypeId(v)) {
        case QMetaType_LongLong:
        case QMetaType_ULongLong:
        case QMetaType_Int:
        case QMetaType_UInt:
        case QMetaType_Double:
            return v;
        default:
            if(v.toLongLong()>0)
                return v.toLongLong();
        }

        qlonglong scale=1;
        static qlonglong KB=1024;
        static qlonglong K=1024;

        auto a=getAlpha(v).toString().toLower();
        if(a==qsl("kb"))
            scale=1;
        else if(a==qsl("mb"))
            scale=2;
        else if(a==qsl("gb"))
            scale=3;
        else if(a==qsl("tb"))
            scale=4;
        else if(a==qsl("pb"))
            scale=5;
        else if(a==qsl("eb"))
            scale=6;
        else if(a==qsl("zb"))
            scale=7;
        else if(a==qsl("yb"))
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

        switch (qTypeId(property)) {
        case QMetaType_Int:
        case QMetaType_UInt:
        case QMetaType_LongLong:
        case QMetaType_ULongLong:
        case QMetaType_Double:
            property.write(this,0);
            break;
        case QMetaType_QDate:
            property.write(this,QDate());
            break;
        case QMetaType_QTime:
            property.write(this,QTime());
            break;
        case QMetaType_QDateTime:
            property.write(this,QDateTime());
            break;
        case QMetaType_QVariantHash:
            property.write(this,QVariantHash());
            break;
        case QMetaType_QVariantMap:
            property.write(this,QVariant());
            break;
        case QMetaType_QVariantList:
            property.write(this,QVariantList());
            break;
        case QMetaType_QStringList:
            property.write(this,QStringList());
            break;
        case QMetaType_QString:
        case QMetaType_QByteArray:
        case QMetaType_QChar:
        case QMetaType_QBitArray:
            property.write(this,qbl_null);
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

    sInfo()<<qsl_fy(settings);
    QHashIterator<QString, QVariant> i(vHash);
    while (i.hasNext()){
        i.next();
        sInfo()<<qsl("      %1:%2").arg(i.key(), vu.toByteArray(i.value()));
    }
    sInfo()<<qsl_space;
}

QVariant SettingBaseTemplate::parseVariables(const QVariant &v) const
{
    auto &p = *reinterpret_cast<SettingBaseTemplatePrv*>(this->p);
    return p.replaceEnv(v);
}

const QVariant SettingBaseTemplate::staticParseVariables(const QVariant &v)
{
    auto value=SettingBaseTemplatePrv::replaceEnvStatic(v);
    return value;
}

QVariant SettingBaseTemplate::variable(const QString &v) const
{
    auto &p = *reinterpret_cast<SettingBaseTemplatePrv*>(this->p);
    auto var=p.variables.value(v);
    return var.isValid()?var:p.replaceEnv(var);
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

const QVariantHash &SettingBaseTemplate::staticVariables()
{
    return *static_variables;
}

bool SettingBaseTemplate::macth(const QString &name)
{
    auto i_name=this->name();
    auto startWith=i_name.contains(qbl("*"));

    if(startWith)
        i_name=i_name.split(qbl("*")).first();

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

        auto t=qTypeId(vGet);
        switch (t) {
        case QMetaType_QString:
        case QMetaType_QByteArray:
        case QMetaType_QChar:
        case QMetaType_QBitArray:
        {
            if(vGet.toString().trimmed().isEmpty())
                continue;
            break;
        }
        case QMetaType_Double:
        case QMetaType_Int:
        case QMetaType_UInt:
        case QMetaType_LongLong:
        case QMetaType_ULongLong:
        {
            if(vGet.toLongLong()<=0)
                continue;
            break;
        }
        case QMetaType_QVariantMap:
        case QMetaType_QVariantHash:
        {
            if(vGet.toHash().isEmpty())
                continue;
            break;
        }
        case QMetaType_QVariantList:
        case QMetaType_QStringList:
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

        switch (qTypeId(property)) {
        case QMetaType_Int:
        case QMetaType_UInt:
            property.write(this,value.toInt());
            break;
        case QMetaType_LongLong:
        case QMetaType_ULongLong:
            property.write(this,value.toLongLong());
            break;
        case QMetaType_Double:
            property.write(this,value.toDouble());
            break;
        case QMetaType_QVariantHash:
            property.write(this, vu.toHash(value));
            break;
        case QMetaType_QVariantMap:
            property.write(this, vu.toMap(value));
            break;
        case QMetaType_QVariantList:
            property.write(this, vu.toList(value));
            break;
        case QMetaType_QStringList:
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
    switch (qTypeId(vv)) {
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
        return this->fromHash(vv.toHash());
    case QMetaType_QVariantList:{
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

        switch (qTypeId(property)) {
        case QMetaType_Int:
        case QMetaType_UInt:
        {
            property.write(this,value.toInt());
            break;
        }
        case QMetaType_LongLong:
        case QMetaType_ULongLong:
        {
            if(property.write(this,value.toLongLong()))
                __return=true;
            break;
        }
        case QMetaType_Double:
        {
            if(property.write(this,value.toDouble()))
                __return=true;
            break;
        }
        case QMetaType_QVariantHash:
        {
            if(property.write(this, vu.toHash(value)))
                __return=true;
            break;
        }
        case QMetaType_QVariantMap:
        {
            if(property.write(this, vu.toMap()))
                __return=true;
            break;
        }
        case QMetaType_QVariantList:
        {
            if(property.write(this, vu.toList()))
                __return=true;
            break;
        }
        case QMetaType_QStringList:{
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
    switch (qTypeId(vv)) {
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
        return this->mergeHash(vv.toHash());
    case QMetaType_QVariantList:
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

QString SettingBaseTemplate::name() const
{
    return p->name;
}

QString SettingBaseTemplate::identification() const
{
    return p->identification;
}

QVariantHash &SettingBaseTemplate::variables() const
{
    return p->variables;
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

int SettingBaseTemplate::activityThread() const
{
    return p->activityThread>0?p->activityThread:QThread::idealThreadCount();
}

qlonglong SettingBaseTemplate::memoryLimit() const
{
    return p->getMemoryBytes(p->memoryLimit, 0).toLongLong();
}

void SettingBaseTemplate::setIdentification(const QString &value)
{
    p->identification=value;
}

void SettingBaseTemplate::setName(const QString &value)
{
    p->name = value.trimmed();
}

void SettingBaseTemplate::setVariables(const QVariantHash &value)
{
    p->variables=value;
}

void SettingBaseTemplate::setEnabled(const bool &value)
{
    p->enabled=value;
}

void SettingBaseTemplate::setActivityLimit(const QVariant &value)
{
    p->activityLimit=value;
}

void SettingBaseTemplate::setActivityInterval(const QVariant &value)
{
    p->activityInterval=value;
}

void SettingBaseTemplate::setActivityThread(const QVariant &value)
{
    p->activityThread=value.toInt();
}

void SettingBaseTemplate::setMemoryLimit(const QVariant &value)
{
    p->memoryLimit=value;
}

void SettingBaseTemplate::setStaticVariables(const QVariantHash &v)
{
    *static_variables=v;
}

} // namespace Private
} // namespace QStm
