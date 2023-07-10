#include "./qstm_setting_manager.h"
#include "./qstm_setting_base.h"
#include "./qstm_util_variant.h"
#include "./qstm_macro.h"
#include "./qstm_envs.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace QStm {

static const auto __comma=",";
static const auto __rootDir="rootdir";
static const auto __arguments="arguments";
static const auto __variables="variables";
static const auto __default="default";
static const auto __services="services";
static const auto __name="name";

class SettingManagerPvt: public QObject{
public:
    QVariantHash arguments, settingBody;
    Envs envs;
    QHash<QString, SettingBase*> settings;
    SettingManager*parent=nullptr;
    QObject *parentParent=nullptr;

    explicit SettingManagerPvt(SettingManager*parent)
        : QObject{parent},
        envs{parent}
    {
        this->parent=parent;
    }

    bool isLoaded()
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

    bool isEmpty()
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

    void clear()
    {
        auto _detail=this->settings.values();
        this->settings.clear();
        qDeleteAll(_detail);
    }

    QVariantHash toHash()
    {
        QVariantHash vHash, vServices;
        auto vList=this->settings.values();
        for(auto &v:vList)
            vServices.insert(v->name(), v->toHash());
        vHash.insert(__variables, this->envs.customEnvs());
        vHash.insert(__services, vServices);
        return vHash;
    }

    bool load(const QStringList &settingsFileName)
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
        return this->load(vHash);
    }

    bool load(const QString &fileName)
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

    bool load(const QVariantHash &settingsIn)
    {
        this->envs.reset();
        auto vSettingsBody=this->envs.parser(settingsIn).toHash();
        this->envs.customEnvs(vSettingsBody.value(__variables));
        this->settingBody=settingsIn;

        {//variables


            auto rootDir=vSettingsBody.value(__rootDir).toString().trimmed();
            this->envs.customEnvs(__rootDir,rootDir.isEmpty()?QStringLiteral("${HOME}/${APPNAME}"):rootDir);

            QVariantHash arguments;
            auto varguments=this->envs.parser(vSettingsBody.value(__arguments));
            if(varguments.isNull() || !varguments.isValid())
                varguments=this->arguments;
            else{
                this->arguments.clear();

                switch (varguments.typeId()) {
                case QMetaType::QString:
                case QMetaType::QByteArray:
                case QMetaType::QChar:
                case QMetaType::QBitArray:
                    varguments=varguments.toString().split(__comma);
                    break;
                default:
                    break;
                }

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

        {//services
            auto vSettingsServices=vSettingsBody.contains(__services)
                                         ?vSettingsBody.value(__services).toHash()
                                         :this->settingBody;

            {//service merge
                Q_DECLARE_VU;
                auto vSettingDefault=vSettingsServices.value(__default);
                auto vSettingsMerge=vSettingsServices;

                vSettingsServices={{__default, vSettingDefault}};
                Q_V_HASH_ITERATOR (vSettingsMerge){
                    i.next();
                    auto serviceName=i.key().trimmed().toLower();
                    if(serviceName==__default)
                        continue;

                    auto vSettingService=i.value().toHash();
                    {
                        vSettingService.insert(__name, serviceName);
                        vSettingService=vu.vMerge(vSettingDefault, vSettingService).toHash();
                        vSettingsServices.insert(serviceName, vSettingService);
                    }
                }
                vSettingsBody.insert(__services, vSettingsServices);
            }
            Q_V_HASH_ITERATOR (vSettingsServices){
                i.next();
                auto vSettingService=i.value().toHash();
                this->insert(vSettingService);
            }
        }

        this->settingBody=vSettingsBody;

        return this->isLoaded();
    }

    QStm::SettingBase *settingGetCheck(const QByteArray &settingName)
    {
        return settings.value(settingName.trimmed().toLower());
    }

    QStm::SettingBase *settingCreate(QObject *parent)
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
    QStm::SettingManager &insert(const QVariantHash &value)
    {
        QVariantHash vValue=value;
        if(vValue.isEmpty())
            return *this->parent;
        auto name=vValue.value(__name).toByteArray().trimmed();
        if(name.isEmpty())
            return *this->parent;

        auto setting=this->settings.value(name);
        if(setting)
            setting->deleteLater();

        static auto l=QStringList{QT_STRINGIFY2(activityLimit),QT_STRINGIFY2(activityInterval),QT_STRINGIFY2(activityIntervalInitial)};
        for(auto &property:l){
            auto v=vValue.value(property);
            if(v.isValid() && v.toLongLong()<=0){
                v=SettingBase::parseInterval(v);
                vValue.insert(property, v);
            }
        }
        setting=this->settingCreate(this);
        setting->fromHash(vValue);
        setting->setName(name);
        this->settings.insert(setting->name(), setting);
        return *this->parent;
    }
};


SettingManager::SettingManager(QObject *parent) : QObject{parent}, p{new SettingManagerPvt{this}}
{
}

SettingManager::SettingManager(const QStringList &settingFileName, QObject *parent):QObject{parent}, p{new SettingManagerPvt{this}}
{
    p->load(settingFileName);
}

SettingManager::SettingManager(const QString &settingFileName, QObject *parent) : QObject{parent}, p{new SettingManagerPvt{this}}
{
    p->load(settingFileName);
}

SettingManager &SettingManager::clear()
{
    p->clear();
    return *this;
}

SettingManager &SettingManager::insert(SettingBase &value)
{
    return p->insert(value.toHash());
}

SettingManager &SettingManager::insert(const QVariantHash &value)
{
    return p->insert(value);
}

SettingBase *SettingManager::setting(const QString &value)const
{
    auto setting=p->settingGetCheck(value.toUtf8());

    return setting;
}

SettingBase *SettingManager::settingClone(const QString &settingName, QObject *parent)
{
    auto setting=p->settings.value(settingName.trimmed().toLower());
    if (!setting || !setting->isValid())
        return nullptr;
    auto newSetting = p->settingCreate(parent);
    newSetting->fromHash(setting->toHash());
    return newSetting;
}

QObject *SettingManager::settingCreate(QObject *parent)
{
    return new QStm::SettingBase{parent};
}

bool SettingManager::load(const QStringList &fileNames)
{
    return p->load(fileNames);
}

bool SettingManager::load(const QString &fileName)
{
    return p->load(fileName);
}

bool SettingManager::load(const QVariantHash &vSettings)
{
    return p->load(vSettings);
}

bool SettingManager::load(const SettingManager &manager)
{
    return p->load(manager.toHash());
}

SettingManager &SettingManager::operator<<(SettingBase &value)
{
    return this->insert(value);
}

QVariantHash &SettingManager::settingBody() const
{
    return p->settingBody;
}

QVariantHash SettingManager::settingBody(const QString &settingName)const
{
    if(!p->settingBody.contains(__services))
        return {};

    auto settingServices=p->settingBody.value(__services).toHash();
    if(settingServices.isEmpty())
        return {};

    auto key=settingName.trimmed().toLower();
    if(!settingServices.contains(key))
        return {};

    auto vSetting=settingServices.value(key).toHash();
    if(vSetting.isEmpty())
        return {};

    vSetting=p->envs.parser(vSetting).toHash();

    return vSetting;
}

QVariantHash &SettingManager::arguments() const
{
    return p->arguments;
}

SettingManager &SettingManager::setArguments(const QVariantHash &value)
{
    p->arguments=p->envs.parser(value).toHash();
    return *this;
}

QVariantHash &SettingManager::variables() const
{
    return p->envs.customEnvs();
}

SettingManager &SettingManager::setVariables(const QVariantHash &value)
{
    p->envs.customEnvs(value);
    return *this;
}

QVariantHash SettingManager::toHash() const
{
    return p->toHash();
}

QString SettingManager::rootDir() const
{
    return p->envs.value(__rootDir).toString();
}

SettingManager &SettingManager::setRootDir(const QString &value)
{
    p->envs.customEnvs(__rootDir, value);
    return *this;
}

QVariantHash SettingManager::toMap() const
{
    return this->toHash();
}

} // namespace QStm
