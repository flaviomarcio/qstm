#pragma once

#include <QObject>
#include <QVariant>
#include "./qstm_global.h"

namespace QStm {
class SettingFilePvt;
//!
//! \brief The SettingFile class
//!
class Q_STM_EXPORT SettingFile : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QVariant setting READ setting WRITE setting RESET resetSetting NOTIFY settingChanged)
    Q_PROPERTY(QVariant envs READ envs WRITE envs RESET resetEnvs NOTIFY envsChanged)
public:

    //!
    //! \brief SettingFile
    //! \param parent
    //!
    Q_INVOKABLE explicit SettingFile(QObject *parent = nullptr);

    //!
    //! \brief SettingFile
    //! \param setting
    //! \param envs
    //!
    explicit SettingFile(const QVariant &setting, const QVariant &envs);

    //!
    //! \brief clear
    //! \return
    //!
    SettingFile &clear();

    //!
    //! \brief setting
    //! \return
    //!
    QStringList &setting() const;
    SettingFile &setting(const QVariant &newSetting);
    SettingFile &resetSetting();

    //!
    //! \brief envs
    //! \return
    //!
    QStringList &envs() const;
    SettingFile &envs(const QVariant &newEnvs);
    SettingFile &resetEnvs();

private:
    SettingFilePvt *p=nullptr;
signals:
    void settingChanged();
    void envsChanged();
};

} // namespace QApr

