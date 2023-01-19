#include "qstm_envs.h"
#include "qstm_util_variant.h"
#include "qstm_util_date.h"
#include "qstm_macro.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

namespace QStm {

Q_GLOBAL_STATIC(QVariantHash, static_DirEnvs)
Q_GLOBAL_STATIC(QVariantHash, static_SystemEnvs)
Q_GLOBAL_STATIC_WITH_ARGS(QString, __argVar,("${%1}"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, __argVarSetting,("%1=%2"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, __argText,("%1"))

static const auto __envUndefined="${?}";
static const auto __envRegExt="\\${+([a-zA-Z0-9._-]+)}";
static const auto __splitEnvs="|";
static const auto __splitEnv="=";
static const auto __splitArray=",";

static void make_static_variables()
{
    QVariantHash envsDir={
         {"DesktopLocation", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)}
        ,{"DocumentsLocation", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)}
        ,{"FontsLocation", QStandardPaths::writableLocation(QStandardPaths::FontsLocation)}
        ,{"ApplicationsLocation", QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)}
        ,{"MusicLocation", QStandardPaths::writableLocation(QStandardPaths::MusicLocation)}
        ,{"MoviesLocation", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)}
        ,{"PicturesLocation", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)}
        ,{"TempLocation", QStandardPaths::writableLocation(QStandardPaths::TempLocation)}
        ,{"HomeLocation", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)}
        ,{"CacheLocation", QStandardPaths::writableLocation(QStandardPaths::CacheLocation)}
        ,{"GenericDataLocation", QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)}
        ,{"RuntimeLocation", QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation)}
        ,{"ConfigLocation", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)}
        ,{"DownloadLocation", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)}
        ,{"GenericCacheLocation", QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)}
        ,{"GenericConfigLocation", QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)}
        ,{"AppDataLocation", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)}
        ,{"AppConfigLocation", QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)}
        ,{"AppLocalDataLocation", QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)}
        ,{"home", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)}
        ,{"tempDir", QStandardPaths::writableLocation(QStandardPaths::TempLocation)}
        ,{"appDir", QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)}
        ,{"appFileName", qApp->applicationFilePath()}
        ,{"appName", qApp->applicationName()}
    };

    *static_DirEnvs=envsDir;
    *static_SystemEnvs=envsDir;
    QVariantHash systemEnvironment;
#ifdef Q_OS_IOS
    Q_UNUSED(systemEnvironment)
#else
    QProcess process;
    auto lst = process.systemEnvironment();
    for (auto &v : lst) {
        auto s = v.split(__splitEnv);
        auto env = s.first().trimmed();
        auto value = s.last().trimmed();
        systemEnvironment.insert(env, value);
    }
#endif

    if(!envsDir.isEmpty()){
        QHashIterator<QString, QVariant> i(envsDir);
        while (i.hasNext()) {
            i.next();
            auto k1=__argVar->arg(i.key()).trimmed().toLower();
            *static_DirEnvs->insert(k1,i.value());
        }
    }

    if(!systemEnvironment.isEmpty()){
        QHashIterator<QString, QVariant> i(systemEnvironment);
        while (i.hasNext()) {
            i.next();
            auto k1=__argVar->arg(i.key()).trimmed().toLower();
            *static_SystemEnvs->insert(k1,i.value());
        }
    }
}

void init()
{
    make_static_variables();
}

Q_COREAPP_STARTUP_FUNCTION(init)


class EnvsPvt: public QObject{
public:
    QVariantHash dirEnvs=*static_DirEnvs;
    QVariantHash systemEnvs=*static_SystemEnvs;
    QVariantHash customEnvs;
    bool envalidEnvsClean;
    QVariant invalidEnvsValue;
    bool ignoreSystemEnvs=false;
    bool ignoreDirEnvs=false;
    bool ignoreCustomEnvs=false;
    bool clearUnfoundEnvs=false;
    Envs *parent=nullptr;
    explicit EnvsPvt(Envs *parent):QObject{parent}
    {
        this->parent=parent;
    }
private:
    static const QString parserToStr(const QVariant &value){
        Q_DECLARE_VU;
        Q_DECLARE_DU;

        auto toValue=[&vu, &du](const QVariant &valueIn){

            QVariant value;
            if(vu.toLongLong(valueIn)!=0)
                value=vu.toLongLong(valueIn);
            else if(vu.toDouble(valueIn)!=0)
                value=vu.toLongLong(valueIn);
            else if(!du.toDateTime(valueIn).isNull() && du.toDateTime(valueIn).isValid())
                value=du.toDateTime(valueIn);
            else if(!du.toDate(valueIn).isNull() && du.toDate(valueIn).isValid())
                value=du.toDate(valueIn);
            else if(!du.toTime(valueIn).isNull() && du.toTime(valueIn).isValid())
                value=du.toTime(valueIn);
            else
                value=valueIn;


            switch (value.typeId()) {
            case QMetaType::UnknownType:
                return QString{};
            case QMetaType::Bool:
                return value.toString();
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::Long:
            case QMetaType::ULong:
            case QMetaType::LongLong:
            case QMetaType::ULongLong:
            case QMetaType::Double:
            case QMetaType::Float:
                return vu.toStr(value);
            default:
                auto text=vu.toStr(value);
                text=text.replace("\"","");
                return __argText->arg(text);
            }
        };

        switch (value.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            QStringList out;
            auto vList=value.toList();
            for(auto&v:vList)
                out.append(vu.toStr(v));
            return toValue(out.join(','));
        }
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            QStringList out;
            auto vHash=value.toHash();
            QHashIterator<QString, QVariant> i(vHash);
            while (i.hasNext()) {
                i.next();
                out.append(__argVarSetting->arg(i.key(), toValue(i.value())));
            }
            return toValue(out.join(','));
        }
        default:
            return vu.toStr(value);
        }

    }

    static const QString parserDeclaredEnvs(const QString &textParser){
        auto input=textParser.trimmed();
        if(input.isEmpty())
            return {};
        QRegularExpression reA(__envRegExt);
        QRegularExpressionMatchIterator i = reA.globalMatch(input);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            if (match.hasMatch()) {
                auto captured=match.captured(0);
                input.replace(captured,captured.toLower());
            }
        }
        return input;
    }

    static const QString parser(const QVariant &values, const QVariantHash &envs)
    {
        Q_DECLARE_VU;
        QString textParser=parserDeclaredEnvs(vu.toByteArray(values));

        QHashIterator<QString, QVariant> i(envs);
        while (i.hasNext()) {
            i.next();
            const auto&key=i.key();
            auto value = parserToStr(i.value());
            textParser = textParser.replace(key, value, Qt::CaseSensitive);

            static QRegularExpression re(__envRegExt);
            if(!re.match(textParser).hasMatch())
                break;
        }

        return textParser;
    }

    void addCustomEnv(const QString &envKey, const QVariant &envValue)
    {
        Q_DECLARE_VU;
        static QRegularExpression re(__envRegExt);
        QString envValueText=parserDeclaredEnvs(vu.toStr(envValue)).trimmed();
        QString key=((re.match(envKey).hasMatch())?envKey:__argVar->arg(envKey.trimmed())).toLower();
        QString value=envValueText;

        if(value.contains(key)){
            value=value.replace(key,__envUndefined);
            sWarning()<<QString("invalid values: value[%1] contains key[%2]").arg(envValueText, key);
            sWarning()<<QString("value modified to: [%1]").arg(value);
        }
        QVariant v=value.isEmpty()?QVariant{}:value;
        v=vu.toVariant(v);
        this->customEnvs.insert(key, v);
    }

    static QString clearEnvs(const QString &textParser)
    {
        static QRegularExpression re(__envRegExt);
        auto text=textParser;
        auto match = re.match(text);
        if (!match.hasMatch())
            return textParser;
        for(auto&key:match.capturedTexts())
            text=text.replace(key, "");
        return text;
    }

public:

    void clear()
    {
        this->dirEnvs.clear();
        this->systemEnvs.clear();
        this->customEnvs.clear();
    }

    static const QVariant staticParser(const QVariant &values, const QVariant &envs)
    {
        Q_DECLARE_VU;
        auto value=parserDeclaredEnvs(vu.toByteArray(values));

        value = parser(value, *static_SystemEnvs);

        value = parser(value, *static_DirEnvs);

        value = parser(value, vu.toHash(envs));

        value=clearEnvs(value);

        return vu.toType(values.typeId(), value);
    }

    QVariant customParser(const QVariant &values)const
    {
        Q_DECLARE_VU;
        auto value=parserDeclaredEnvs(vu.toByteArray(values));

        if(!this->ignoreSystemEnvs)
            value = parser(value, systemEnvs);

        if(!this->ignoreDirEnvs)
            value = parser(value, dirEnvs);

        if(!this->ignoreCustomEnvs)
            value = parser(value, this->customEnvs);

        if(this->clearUnfoundEnvs)
            value=clearEnvs(value);

        return vu.toType(values.typeId(), value);
    }

    void setCustomEnvs(const QVariant &customValues)
    {
        Q_DECLARE_VU;
        auto values=vu.toVariant(customValues);
        switch (values.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            auto vHash=values.toHash();
            QHashIterator<QString, QVariant> i(vHash);
            while(i.hasNext()){
                i.next();
                this->addCustomEnv(i.key(), i.value());
            }
            break;
        }
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto vList=values.toList();
            for (auto &v : vList) {
                auto envItem=vu.toVariant(v);

                switch (envItem.typeId()) {
                case QMetaType::QVariantHash:
                case QMetaType::QVariantMap:
                case QMetaType::QVariantPair:
                    this->setCustomEnvs(envItem);
                    continue;
                default:
                    break;
                }


                auto listVar = envItem.toString().split(__splitEnv);
                if(listVar.isEmpty())
                    continue;

                auto key=listVar.takeFirst().trimmed();
                QVariant value;

                switch (listVar.size()) {
                case 0:
                    break;
                case 1:
                    value=vu.toVariant(listVar.last());
                    switch (value.typeId()) {
                    case QMetaType::QString:
                    case QMetaType::QByteArray:{
                        if(value.toString().contains(__splitArray))
                            value=value.toString().split(__splitArray);
                        break;
                    }
                    default:
                        break;
                    }
                    break;
                default:
                {
                    QVariantList vList;
                    for(auto&v:listVar){
                        auto item=vu.toVariant(v);
                        switch (item.typeId()) {
                        case QMetaType::QString:
                        case QMetaType::QByteArray:{
                            if(item.toString().contains(__splitArray))
                                item=item.toString().split(__splitArray);
                            break;
                        }
                        default:
                            break;
                        }
                        if(item.isValid() && !item.isNull())
                            vList.append(item);
                    }
                    value=(vList.size()==1)?vList.first():vList;
                    break;
                }
                }
                this->addCustomEnv(key, value);
            }
            break;
        }
        default:
            auto text=values.toString().trimmed();
            if(!text.contains(__splitEnvs)){
                this->addCustomEnv(text, {});
            }
            else{
                auto vListText=text.split(__splitEnvs);
                if(!vListText.isEmpty())
                    this->setCustomEnvs(vListText);
            }
            break;
        }
    }
};

Envs::Envs(QObject *parent)
    : QObject{parent}
{
    this->p=new EnvsPvt{this};
}

Envs::Envs(const QVariant &customEnvs, QObject *parent):QObject{parent}
{
    this->p=new EnvsPvt{this};
    p->setCustomEnvs(customEnvs);
}

Envs &Envs::clear()
{
    p->clear();
    return *this;
}

QVariant Envs::parser(const QVariant &values)const
{
    return p->customParser(values);
}

const QVariant Envs::parser(const QVariant &values, const QVariant &envs)
{
    return EnvsPvt::staticParser(values, envs);
}

QVariant Envs::value(const QString &env)
{
    return p->customParser(__argVar->arg(env));
}

QVariantHash &Envs::customEnvs() const
{
    return p->customEnvs;
}

Envs &Envs::customEnvs(const QVariant &newCustomEnvs)
{
    Q_DECLARE_VU;
    p->customEnvs.clear();
    p->setCustomEnvs(newCustomEnvs);
    emit customEnvsChanged();
    return *this;
}

Envs &Envs::customEnvs(const QString &env, const QVariant &value)
{
    Q_DECLARE_VU;
    p->setCustomEnvs(QVariantHash{{env, value}});
    emit customEnvsChanged();
    return *this;
}

Envs &Envs::resetCustomEnvs()
{
    return this->customEnvs({});
}

Envs &Envs::customEnv(const QVariant &newCustomEnv)
{
    Q_DECLARE_VU;
    p->setCustomEnvs(newCustomEnv);
    emit customEnvsChanged();
    return *this;
}

Envs &Envs::customEnv(const QString &key, const QVariant &value)
{
    Q_DECLARE_VU;
    p->setCustomEnvs(QVariantHash{{key, value}});
    emit customEnvsChanged();
    return *this;
}

bool Envs::envalidEnvsClean() const
{
    return p->envalidEnvsClean;
}

Envs &Envs::envalidEnvsClean(bool newEnvalidEnvsClean)
{
    if (p->envalidEnvsClean == newEnvalidEnvsClean)
        return *this;
    p->envalidEnvsClean = newEnvalidEnvsClean;
    emit envalidEnvsCleanChanged();
    return *this;
}

Envs &Envs::resetEnvalidEnvsClean()
{
    return envalidEnvsClean({});
}

QVariant &Envs::invalidEnvsValue() const
{
    return p->invalidEnvsValue;
}

Envs &Envs::invalidEnvsValue(const QVariant &newInvalidEnvsValue)
{
    Q_DECLARE_VU;
    p->invalidEnvsValue=vu.toVariant(newInvalidEnvsValue);
    emit invalidEnvsValueChanged();
    return *this;
}

Envs &Envs::resetInvalidEnvsValue()
{
    return invalidEnvsValue({});
}

bool Envs::ignoreSystemEnvs() const
{
    return p->ignoreSystemEnvs;
}

Envs &Envs::ignoreSystemEnvs(bool newIgnoreSystemEnvs)
{
    if (p->ignoreSystemEnvs == newIgnoreSystemEnvs)
        return *this;
    p->ignoreSystemEnvs = newIgnoreSystemEnvs;
    emit ignoreSystemEnvsChanged();
    return *this;
}

Envs &Envs::resetIgnoreSystemEnvs()
{
    return ignoreSystemEnvs({});
}

bool Envs::ignoreDirEnvs() const
{
    return p->ignoreDirEnvs;
}

Envs &Envs::ignoreDirEnvs(bool newIgnoreDirEnvs)
{
    if (p->ignoreDirEnvs == newIgnoreDirEnvs)
        return *this;
    p->ignoreDirEnvs = newIgnoreDirEnvs;
    emit ignoreDirEnvsChanged();
    return *this;
}

Envs &Envs::resetIgnoreDirEnvs()
{
    return ignoreDirEnvs({});
}

bool Envs::ignoreCustomEnvs() const
{
    return p->ignoreCustomEnvs;
}

Envs &Envs::ignoreCustomEnvs(bool newIgnoreCustomEnvs)
{
    if (p->ignoreCustomEnvs == newIgnoreCustomEnvs)
        return *this;
    p->ignoreCustomEnvs = newIgnoreCustomEnvs;
    emit ignoreCustomEnvsChanged();
    return *this;
}

Envs &Envs::resetIgnoreCustomEnvs()
{
    return ignoreCustomEnvs({});
}

bool Envs::clearUnfoundEnvs() const
{
    return p->clearUnfoundEnvs;
}

Envs &Envs::clearUnfoundEnvs(bool newClearUnfoundEnvs)
{
    if (p->clearUnfoundEnvs == newClearUnfoundEnvs)
        return *this;
    p->clearUnfoundEnvs=newClearUnfoundEnvs;
    emit clearUnfoundEnvsChanged();
    return *this;
}

Envs &Envs::resetClearUnfoundEnvs()
{
    return this->clearUnfoundEnvs({});
}

} // namespace QStm
