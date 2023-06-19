#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantHash>
#include <QFile>
#include "./qstm_global.h"

namespace QStm {
class EnvsPvt;
//!
//! \brief The Envs class
//!
class Q_STM_EXPORT Envs : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant argumentEnvs READ argumentEnvs WRITE argumentEnvs RESET resetArgumentEnvs NOTIFY argumentEnvsChanged)
    Q_PROPERTY(QVariant systemEnvs READ systemEnvs WRITE systemEnvs RESET resetSystemEnvs NOTIFY systemEnvsChanged)
    Q_PROPERTY(QVariant customEnvs READ customEnvs WRITE customEnvs RESET resetCustomEnvs NOTIFY customEnvsChanged)
    Q_PROPERTY(bool invalidEnvsClean READ invalidEnvsClean WRITE invalidEnvsClean RESET resetInvalidEnvsClean NOTIFY invalidEnvsCleanChanged)
    Q_PROPERTY(QVariant invalidEnvsValue READ invalidEnvsValue WRITE invalidEnvsValue RESET resetInvalidEnvsValue NOTIFY invalidEnvsValueChanged)
    Q_PROPERTY(bool ignoreSystemEnvs READ ignoreSystemEnvs WRITE ignoreSystemEnvs RESET resetIgnoreSystemEnvs NOTIFY ignoreSystemEnvsChanged)
    Q_PROPERTY(bool ignoreArgumentEnvs READ ignoreArgumentEnvs WRITE ignoreArgumentEnvs RESET resetIgnoreArgumentEnvs NOTIFY ignoreArgumentEnvsChanged)
    Q_PROPERTY(bool ignoreDirEnvs READ ignoreDirEnvs WRITE ignoreDirEnvs RESET resetIgnoreDirEnvs NOTIFY ignoreDirEnvsChanged)
    Q_PROPERTY(bool ignoreCustomEnvs READ ignoreCustomEnvs WRITE ignoreCustomEnvs RESET resetIgnoreCustomEnvs NOTIFY ignoreCustomEnvsChanged)
    Q_PROPERTY(bool clearUnfoundEnvs READ clearUnfoundEnvs WRITE clearUnfoundEnvs RESET resetClearUnfoundEnvs NOTIFY clearUnfoundEnvsChanged)
public:
    explicit Envs(QObject *parent = nullptr);
    explicit Envs(const QVariant &customEnvs, QObject *parent = nullptr);

    //!
    Envs &reset();

    //!
    //! \brief clear
    //! \return
    //!
    Envs &clear();

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
    const QVariantHash &systemEnvs()const;
    Envs &systemEnvs(const QVariant &envs);
    Envs &systemEnvs(const QString &env, const QVariant &value);
    Envs &systemEnvs(QFile &envs);
    Envs &resetSystemEnvs();

    //!
    //! \brief contains
    //! \param env
    //! \return
    //!
    bool contains(const QString &env)const;

    //!
    //! \brief value
    //! \param env
    //! \return
    //!
    QVariant value(const QString &env);

    //!
    //! \brief argumentEnvs
    //! \return
    //!
    QVariantHash &argumentEnvs()const;
    Envs &argumentEnvs(const QVariant &newCustomEnvs);
    Envs &argumentEnvs(const QString &env, const QVariant &value);
    Envs &argumentEnvs(QFile &envs);
    Envs &resetArgumentEnvs();

    //!
    //! \brief customEnvs
    //! \return
    //!
    QVariantHash &customEnvs()const;
    Envs &customEnvs(const QVariant &newCustomEnvs);
    Envs &customEnvs(const QString &env, const QVariant &value);
    Envs &customEnvs(QFile &envs);
    Envs &resetCustomEnvs();

    //!
    //! \brief customEnv
    //! \param newCustomEnv
    //! \return
    //!
    Envs &customEnv(const QVariant &newCustomEnv);
    Envs &customEnv(const QString &key, const QVariant &value);

    //!
    //! \brief invalidEnvsClean
    //! \return
    //!
    bool invalidEnvsClean() const;
    Envs &invalidEnvsClean(bool newinvalidEnvsClean);
    Envs &resetInvalidEnvsClean();

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
    //! \brief ignoreArgumentEnvs
    //! \return
    //!
    bool ignoreArgumentEnvs() const;
    Envs &ignoreArgumentEnvs(bool newIgnoreArgumentEnvs);
    Envs &resetIgnoreArgumentEnvs();

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

    //!
    //! \brief clearUnfoundEnvs
    //! \return
    //!
    bool clearUnfoundEnvs() const;
    Envs &clearUnfoundEnvs(bool newClearUnfoundEnvs);
    Envs &resetClearUnfoundEnvs();

private:
    EnvsPvt *p=nullptr;
signals:
    void systemEnvsChanged();
    void argumentEnvsChanged();
    void customEnvsChanged();
    void invalidEnvsCleanChanged();
    void invalidEnvsValueChanged();
    void ignoreArgumentEnvsChanged();
    void ignoreSystemEnvsChanged();
    void ignoreDirEnvsChanged();
    void ignoreCustomEnvsChanged();
    void clearUnfoundEnvsChanged();
};

} // namespace QStm

