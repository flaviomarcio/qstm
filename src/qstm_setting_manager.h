#pragma once

#include "./qstm_global.h"
#include "./qstm_setting_base.h"
#include <QSettings>

#define Q_SETTING_MANAGER_REPLACE_METHODS(Manager, Setting) public: \
Q_INVOKABLE explicit Manager(QObject *parent = nullptr) : QStm::SettingManager(parent){} \
Q_INVOKABLE explicit Manager(const QStringList&settingFileName, QObject *parent = nullptr):QStm::SettingManager{settingFileName, parent}{}\
Q_INVOKABLE explicit Manager(const QString &settingFileName, QObject *parent = nullptr):QStm::SettingManager{settingFileName, parent} {}\
Setting &setting(){ auto setting=&QStm::SettingManager::setting(); return *dynamic_cast<Setting*>(setting); } \
Setting &setting(const QString &value){ auto setting=&QStm::SettingManager::setting(value); return *dynamic_cast<Setting*>(setting); } \
Setting *settingClone(const QString &value, QObject *parent=nullptr){ auto setting=QStm::SettingManager::settingClone(value, parent); return dynamic_cast<Setting*>(setting); } \
virtual QObject *settingCreate(QObject *parent=nullptr){ return new Setting{parent}; }


namespace QStm {
class SettingManagerPvt;

//!
//! \brief The SettingManager class
//!
class Q_STM_EXPORT SettingManager : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief SettingManager
    //! \param parent
    //!
    Q_INVOKABLE explicit SettingManager(QObject *parent = nullptr);

    //!
    //! \brief SettingManager
    //! \param settingFileName
    //! \param parent
    //!
    explicit SettingManager(const QStringList&settingFileName, QObject *parent = nullptr);

    //!
    //! \brief SettingManager
    //! \param settingFileName
    //! \param parent
    //!
    explicit SettingManager(const QString &settingFileName, QObject *parent = nullptr);

    //!
    //! \brief clear
    //! \return
    //!
    virtual SettingManager &clear();

    //!
    //! \brief insert
    //! \param value
    //! \return
    //!
    virtual SettingManager &insert(SettingBase &value);

    //!
    //! \brief insert
    //! \param value
    //! \return
    //!
    virtual SettingManager &insert(const QVariantHash &value);

    //!
    //! \brief setting
    //! \param value
    //! \return
    //!
    SettingBase *setting(const QString &value) const;

    //!
    //! \brief settingClone
    //! \param value
    //! \return
    //!
    SettingBase *settingClone(const QString &settingName, QObject *parent=nullptr);

    //!
    //! \brief settingCreate
    //! \param parent
    //! \return
    //!
    virtual QObject *settingCreate(QObject *parent=nullptr);

    //!
    //! \brief load
    //! \param fileNames
    //! \return
    //!
    virtual bool load(const QStringList &fileNames);

    //!
    //! \brief load
    //! \param fileName
    //! \return
    //!
    virtual bool load(const QString &fileName);

    //!
    //! \brief load
    //! \param vSettings
    //! \return
    //!
    virtual bool load(const QVariantHash &vSettings);

    //!
    //! \brief load
    //! \param manager
    //! \return
    //!
    virtual bool load(const SettingManager &manager);

    //!
    //! \brief operator <<
    //! \param value
    //! \return
    //!
    SettingManager &operator<<(SettingBase&value);

    //!
    //! \brief settingBody
    //! \return
    //! full settings source
    virtual QVariantHash &settingBody() const;

    //!
    //! \brief settingBody
    //! \param value
    //! \return
    //!
    virtual QVariantHash settingBody(const QString &settingName) const;

    //!
    //! \brief arguments
    //! \return
    //!arguments with settings
    virtual QVariantHash &arguments() const;

    //!
    //! \brief setArguments
    //! \param value
    //!
    virtual SettingManager &setArguments(const QVariantHash &value);

    //!
    //! \brief variables
    //! \return
    //!
    virtual QVariantHash &variables() const;

    //!
    //! \brief setVariables
    //! \param value
    //!
    virtual SettingManager &setVariables(const QVariantHash &value);

    //!
    //! \brief rootDir
    //! \return
    //!
    virtual QString rootDir() const;

    //!
    //! \brief setRootDir
    //! \param value
    //! \return
    //!
    virtual SettingManager &setRootDir(const QString &value);

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantHash toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash toHash() const;


private:
    SettingManagerPvt *p=nullptr;
};

}
