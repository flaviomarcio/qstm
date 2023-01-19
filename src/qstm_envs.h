#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantHash>
#include "./qstm_global.h"

namespace QStm {
class EnvsPvt;
//!
//! \brief The Envs class
//!
class Q_STM_EXPORT Envs : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant customEnvs READ customEnvs WRITE customEnvs RESET resetCustomEnvs NOTIFY customEnvsChanged)
    Q_PROPERTY(bool envalidEnvsClean READ envalidEnvsClean WRITE envalidEnvsClean RESET resetEnvalidEnvsClean NOTIFY envalidEnvsCleanChanged)
    Q_PROPERTY(QVariant invalidEnvsValue READ invalidEnvsValue WRITE invalidEnvsValue RESET resetInvalidEnvsValue NOTIFY invalidEnvsValueChanged)
    Q_PROPERTY(bool ignoreSystemEnvs READ ignoreSystemEnvs WRITE ignoreSystemEnvs RESET resetIgnoreSystemEnvs NOTIFY ignoreSystemEnvsChanged)
    Q_PROPERTY(bool ignoreDirEnvs READ ignoreDirEnvs WRITE ignoreDirEnvs RESET resetIgnoreDirEnvs NOTIFY ignoreDirEnvsChanged)
    Q_PROPERTY(bool ignoreCustomEnvs READ ignoreCustomEnvs WRITE ignoreCustomEnvs RESET resetIgnoreCustomEnvs NOTIFY ignoreCustomEnvsChanged)
public:
    explicit Envs(QObject *parent = nullptr);
    explicit Envs(const QVariant &customEnvs, QObject *parent = nullptr);


    //!
    //! \brief parser
    //! \param v
    //! \return
    //!
    QVariant parser(const QVariant &values) const;
    static const QVariant parser(const QVariant &values, const QVariant &envs);

    //!
    //! \brief systemEnvs
    //! \return
    //!
    static const QVariantHash &systemEnvs();

    //!
    //! \brief customEnvs
    //! \return
    //!
    QVariantHash &customEnvs()const;
    Envs &customEnvs(const QVariant &newCustomEnvs);
    Envs &resetCustomEnvs();

    //!
    //! \brief customEnv
    //! \param newCustomEnv
    //! \return
    //!
    Envs &customEnv(const QVariant &newCustomEnv);
    Envs &customEnv(const QString &key, const QVariant &value);

    //!
    //! \brief envalidEnvsClean
    //! \return
    //!
    bool envalidEnvsClean() const;
    Envs &envalidEnvsClean(bool newEnvalidEnvsClean);
    Envs &resetEnvalidEnvsClean();

    //!
    //! \brief invalidEnvsValue
    //! \return
    //!
    QVariant &invalidEnvsValue() const;
    Envs &invalidEnvsValue(const QVariant &newInvalidEnvsValue);
    Envs &resetInvalidEnvsValue();

    //!
    //! \brief ignoreSystemEnvs
    //! \return
    //!
    bool ignoreSystemEnvs() const;
    Envs &ignoreSystemEnvs(bool newIgnoreSystemEnvs);
    Envs &resetIgnoreSystemEnvs();

    //!
    //! \brief ignoreDirEnvs
    //! \return
    //!
    bool ignoreDirEnvs() const;
    Envs &ignoreDirEnvs(bool newIgnoreDirEnvs);
    Envs &resetIgnoreDirEnvs();

    //!
    //! \brief ignoreCustomEnvs
    //! \return
    //!
    bool ignoreCustomEnvs() const;
    Envs &ignoreCustomEnvs(bool newIgnoreCustomEnvs);
    Envs &resetIgnoreCustomEnvs();

private:
    EnvsPvt *p=nullptr;
signals:
    void customEnvsChanged();
    void envalidEnvsCleanChanged();
    void invalidEnvsValueChanged();

    void ignoreSystemEnvsChanged();
    void ignoreDirEnvsChanged();
    void ignoreCustomEnvsChanged();
};

} // namespace QStm

