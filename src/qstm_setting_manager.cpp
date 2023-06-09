#include "./qstm_setting_manager.h"
#include "./private/p_qstm_setting_manager.h"
#include "./qstm_setting_base.h"
#include <QCoreApplication>

namespace QStm {


SettingManager::SettingManager(QObject *parent) : QObject{nullptr}
{
    this->p = new SettingManagerPvt{this};
    if (parent && parent->thread() == this->thread())
        this->setParent(parent);

    p->parentParent = parent;
    p->load(p->parentParent);
}

SettingManager::SettingManager(const QStringList &settingFileName, QObject *parent)
{
    this->p = new SettingManagerPvt{this};
    if (parent != nullptr) {
        if (parent->thread() == this->thread())
            this->setParent(parent);
    }

    p->load(settingFileName);
}

SettingManager::SettingManager(const QString &settingFileName, QObject *parent) : QObject{parent}
{
    this->p = new SettingManagerPvt{this};
    if (parent != nullptr) {
        if (parent->thread() == this->thread())
            this->setParent(parent);
    }

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

SettingBase &SettingManager::setting()
{
    return this->setting({});
}

SettingBase &SettingManager::setting(const QString &value)
{
    return p->settingGetCheck(value.toUtf8());
}

SettingBase *SettingManager::settingClone(const QString &settingName, QObject *parent)
{
    auto setting=p->settings.value(p->settingNameAdjust(settingName));
    if (!setting || !setting->isValid())
        return nullptr;
    auto newSetting = p->settingCreate(parent);
    newSetting->fromHash(setting->toHash());
    return newSetting;
}

QObject *SettingManager::settingCreate(QObject *parent)
{
    Q_UNUSED(parent)
    qFatal("not implemented");
    return nullptr;
}

bool SettingManager::load(const QVariant &settings)
{
    return p->v_load(settings);
}

bool SettingManager::load(const SettingManager &manager)
{
    return p->load(manager.toHash());
}

bool SettingManager::load(QObject *settingsObject)
{
    return p->load(settingsObject);
}

QVariant SettingManager::settingsFileName() const
{
    return p->settingsFileName;
}

SettingManager &SettingManager::operator<<(SettingBase &value)
{
    return this->insert(value);
}

QVariantHash &SettingManager::settingBody() const
{
    return p->settingBody;
}

const QVariantHash SettingManager::settingBody(const QString &settingName)
{
    const auto setting=p->settings.value(p->settingNameAdjust(settingName));
    if(!setting)
        return {};
    return setting->toHash();
}

QVariantHash SettingManager::arguments() const
{
    return p->arguments;
}

void SettingManager::setArguments(const QVariantHash &value)
{
    p->arguments=p->envs.parser(value).toHash();
}

QVariantHash SettingManager::variables() const
{
    return p->envs.customEnvs();
}

void SettingManager::setVariables(const QVariantHash &value)
{
    p->envs.customEnvs(value);
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
