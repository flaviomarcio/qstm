#include "./p_qstm_setting_manager.h"
#include "../qstm_util_variant.h"
#include "../qstm_macro.h"

static const auto __hostName="hostName";
static const auto __port="port";
static const auto __name="name";

QStm::SettingManagerPvt::SettingManagerPvt(SettingManager *parent)
    : QObject{parent},
      settingsDefault{parent},
      envs{parent}
{
    this->parent=parent;
}

bool QStm::SettingManagerPvt::isLoaded()
{
    QHashIterator<QString, SettingBase*> i(this->settings);
    while (i.hasNext()) {
        i.next();
        if(i.key().trimmed().isEmpty())
            continue;

        auto &v=i.value();
        if(!v->isValid())
            continue;

        return true;
    }
    return false;
}

bool QStm::SettingManagerPvt::isEmpty()
{
    QHashIterator<QString, SettingBase*> i(this->settings);
    while (i.hasNext()) {
        i.next();
        auto &v=i.value();
        if(!v->isValid())
            continue;
        return false;
    }
    return true;
}

void QStm::SettingManagerPvt::clear()
{
    auto _detail=this->settings.values();
    this->settings.clear();
    qDeleteAll(_detail);
}

QVariantHash QStm::SettingManagerPvt::toHash()
{
    QVariantHash vHash, vServices;
    auto vList=QList<SettingBase*>{&this->settingsDefault};
    vList=vList+this->settings.values();
    for(auto &v:vList)
        vServices.insert(v->name(), v->toHash());
    vHash.insert(__variables, this->envs.customEnvs());
    vHash.insert(__services, vServices);
    return vHash;
}

QByteArray QStm::SettingManagerPvt::settingNameAdjust(const QString &settingName)
{
    return settingName.trimmed().toUtf8();
}

QStm::SettingBase &QStm::SettingManagerPvt::settingGetCheck(const QByteArray &settingName)
{
    if(QThread::currentThread()!=this->thread())
        qFatal();
    auto name=this->settingNameAdjust(settingName);
    auto ___return=settings.value(name);
    if(___return==nullptr){
        ___return=this->settingCreate(this);
        settings.insert(name, ___return);
    }
    return *___return;
}

QStm::SettingBase *QStm::SettingManagerPvt::settingCreate(QObject *parent)
{
    auto object=this->parent->settingCreate(parent);
    if(object!=nullptr){
        auto setting=dynamic_cast<SettingBase*>(object);
        if(setting==nullptr)
            delete object;
        return setting;
    }
    return nullptr;
}

QStm::SettingManager &QStm::SettingManagerPvt::insert(const QVariantHash &value)
{
    QVariantHash vValue=value;
    if(vValue.isEmpty())
        return *this->parent;
    auto name=vValue.value(QStringLiteral("name")).toByteArray().trimmed();
    if(name.isEmpty())
        return *this->parent;

    auto setting=this->settings.value(name);
    if(setting!=nullptr)
        setting->deleteLater();

    static auto l=QStringList{QT_STRINGIFY2(activityLimit),QT_STRINGIFY2(activityInterval)};
    for(auto &property:l){
        auto v=vValue.value(property);
        if(v.isValid() && v.toLongLong()<=0){
            v=SettingBase::parseInterval(v);
            vValue[property]=v;
        }
    }
    setting=this->settingCreate(this);
    setting->fromHash(vValue);
    setting->setName(name);
    this->settings.insert(setting->name(), setting);
    return *this->parent;
}

bool QStm::SettingManagerPvt::v_load(const QVariant &v)
{
    switch (v.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return this->load(v.toHash());
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return this->load(v.toStringList());
    default:
        return this->load(v.toString());
    }
}

bool QStm::SettingManagerPvt::load(QObject *settingsObject)
{
    if(settingsObject==nullptr)
        return false;

    auto metaObject=settingsObject->metaObject();
    for(int methodIndex = 0; methodIndex < metaObject->methodCount(); ++methodIndex) {
        auto metaMethod = metaObject->method(methodIndex);
        if(metaMethod.parameterCount()>0)
            continue;

        auto methodName=QString(metaMethod.name()).toLower().trimmed();
        static auto staticNames=QStringList{QStringLiteral("resourcesettings")};
        if(!staticNames.contains(methodName))
            continue;

        QVariant invokeReturn;
        auto argReturn=Q_RETURN_ARG(QVariant, invokeReturn);
        if(!metaMethod.invoke(settingsObject, argReturn))
            continue;
        return this->v_load(invokeReturn);
    }
    return false;
}

bool QStm::SettingManagerPvt::load(const QStringList &settingsFileName)
{
    QVariantList vList;
    for(auto &fileName:settingsFileName){
        QFile file(fileName);
        if(fileName.isEmpty())
            continue;

        if(!file.exists()){
            sWarning()<<QStringLiteral("file not exists %1").arg(file.fileName());
            continue;
        }

        if(!file.open(QFile::ReadOnly)){
            sWarning()<<QStringLiteral("%1, %2").arg(file.fileName(), file.errorString());
            continue;
        }

        auto bytes=file.readAll();
        file.close();
        QJsonParseError*error=nullptr;
        auto doc=QJsonDocument::fromJson(bytes, error);
        if(error!=nullptr){
            sWarning()<<QStringLiteral("%1, %2").arg(file.fileName(), error->errorString());
            continue;
        }

        if(doc.object().isEmpty()){
            sWarning()<<QStringLiteral("object is empty, %1").arg(file.fileName());
            continue;

        }
        auto vHash=doc.object().toVariantHash();
        if(!vHash.isEmpty())
            vList.append(vHash);
    }
    Q_DECLARE_VU;
    auto vHash=vu.vMerge(vList).toHash();
    if(this->load(vHash))
        this->settingsFileName=settingsFileName;
    else
        this->settingsFileName.clear();
    return this->isLoaded();
}

bool QStm::SettingManagerPvt::load(const QString &fileName)
{
    QFile file(fileName);
    if(fileName.trimmed().isEmpty()){
        sWarning()<<QStringLiteral("not file settings");
        return false;
    }
    if(!file.exists()){
        sWarning()<<QStringLiteral("file not exists %1").arg(file.fileName());
        return false;
    }

    if(!file.open(QFile::ReadOnly)){
        sWarning()<<QStringLiteral("%1, %2").arg(file.fileName(), fileName);
        return false;
    }

    auto bytes=file.readAll();
    file.close();

    auto vHash=QJsonDocument::fromJson(bytes).object().toVariantHash();
    if(!vHash.contains(__services)){
        sWarning()<<QStringLiteral("tag services not exists, %1").arg(file.fileName());
        return false;
    }

    sWarning()<<QStringLiteral("loaded settings: %1").arg(file.fileName());
    return this->load(vHash);
}

bool QStm::SettingManagerPvt::load(const QVariantHash &settingsIn)
{
    this->envs.reset();
    auto settingsBody=this->envs.parser(settingsIn).toHash();
    this->envs.customEnvs(settingsBody.value(__variables));
    this->settingBody=settingsIn;

    {//variables


        auto rootDir=settingsBody.value(__rootDir).toString().trimmed();
        this->envs.customEnvs(__rootDir,rootDir.isEmpty()?QStringLiteral("${HOME}/${APPNAME}"):rootDir);

        QVariantHash arguments;
        auto varguments=settingsBody.value(__arguments);
        if(varguments.isNull() || !varguments.isValid())
            varguments=this->arguments;
        else{
            this->arguments.clear();

            switch (varguments.typeId()) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            {
                QHashIterator<QString, QVariant> i(varguments.toHash());
                while (i.hasNext()) {
                    i.next();
                    arguments.insert(i.key().toLower(), i.value());
                }
                break;
            }
            case QMetaType::QVariantList:
            case QMetaType::QStringList:
            {
                for(auto &v:varguments.toList()){
                    auto l=v.toString().split(QStringLiteral("="));
                    if(l.isEmpty())
                        continue;

                    if(l.size()==1){
                        auto key=l.first();
                        auto value=l.last();
                        arguments.insert(key,value);
                        continue;
                    }

                    auto key=l.first().toLower();
                    auto value=l.last();
                    arguments.insert(key,value);
                }
                break;
            }
            default:
                break;
            }
            this->parent->setArguments(arguments);
        }
    }

    auto serviceSettings=settingsBody.contains(__services)?settingsBody.value(__services).toHash():this->settingBody;
    serviceSettings=this->envs.parser(serviceSettings).toHash();
    auto defaultSetting=serviceSettings.value(__default).toHash();

    this->settingsDefault=defaultSetting;

    if(serviceSettings.contains(__hostName) && serviceSettings.contains(__port)){
        this->insert(serviceSettings);
        return this->isLoaded();
    }

    QHashIterator<QString, QVariant> i(serviceSettings);
    while (i.hasNext()) {
        i.next();
        auto value=i.value().toHash();
        value.insert(__name, i.key().trimmed());
        {
            QHashIterator<QString, QVariant> i(defaultSetting);
            while (i.hasNext()) {
                i.next();
                if(!value.contains(i.key()))
                    value.insert(i.key(), i.value());
            }
        }


        this->insert(value);

    }
    return this->isLoaded();
}
