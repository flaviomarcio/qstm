#pragma once

#include "../qstm_setting_manager.h"
#include "../qstm_envs.h"
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

static const auto __rootDir="rootdir";
static const auto __arguments="arguments";
static const auto __variables="variables";
static const auto __default="default";
static const auto __services="services";

class SettingManagerPvt: QObject{
public:
    QVariant settingsFileName;
    SettingBase settingsDefault;
    QVariantHash arguments, settingBody;
    Envs envs;
    QHash<QString, SettingBase*> settings;
    SettingManager*parent=nullptr;
    QObject *parentParent=nullptr;

    explicit SettingManagerPvt(SettingManager*parent);

    virtual ~SettingManagerPvt();

    bool isLoaded();

    bool isEmpty();

    void clear();

    QVariantHash toHash();

    QByteArray settingNameAdjust(const QString &settingName);

    SettingBase&settingGetCheck(const QByteArray &settingName);

    SettingBase*settingCreate(QObject *parent);

    SettingManager &insert(const QVariantHash &value);

    bool v_load(const QVariant &v);

    bool load(QObject *settingsObject);

    bool load(const QStringList &settingsFileName);

    bool load(const QString &fileName);

    bool load(const QVariantHash &settingsIn);

};

}
