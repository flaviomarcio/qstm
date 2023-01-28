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

static const auto __argVarStartWith="${";
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
    QVariantHash argumentEnvs;
    QVariantHash customEnvs;
    bool invalidEnvsClean;
    QVariant invalidEnvsValue;
    bool ignoreSystemEnvs=false;
    bool ignoreArgumentEnvs=false;
    bool ignoreDirEnvs=false;
    bool ignoreCustomEnvs=false;
    bool clearUnfoundEnvs=false;
    Envs *parent=nullptr;
    explicit EnvsPvt(Envs *parent):QObject{parent}
    {
        this->parent=parent;
    }

public:
    static QVariant readEnvFile(QFile &envs)
    {
        envs.close();
        if(!envs.open(QFile::Unbuffered | QFile::ReadOnly)){
            sWarning()<<envs.errorString();
            return {};
        }
        Q_DECLARE_VU;
        static const QStringList phrasesReplace={{"#!/bin/bash"}, {"export "}};
        auto vBody=vu.toVariant(envs.readAll());
        envs.close();
        switch (vBody.typeId()) {
        case QMetaType::QStringList:
        case QMetaType::QVariantList:
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            break;
        case QMetaType::QString:
        case QMetaType::QByteArray:
        {
            auto bytes=vBody.toString();
            if(bytes.contains('\n'))
                vBody=bytes.split('\n');
            break;
        }
        default:
            break;
        }

        switch (vBody.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            return vBody.toHash();
        case QMetaType::QStringList:
        case QMetaType::QVariantList:
        {
            QVariantHash vHash;

            auto envs=vBody.toList();
            vBody.clear();
            for(auto&env:envs){
                QString line=env.toString();

                if(line.isEmpty())
                    continue;

                if(line.right(1)==';')
                    line=line.left(line.length()-1);

                for(auto&phrase: phrasesReplace)
                    line=line.replace(phrase,"").trimmed();

                if(line.isEmpty())
                    continue;

                if(!line.contains(__splitEnv))
                    vHash.insert(line, {});
                else{
                    auto lst=line.split(__splitEnv);
                    auto key=lst.takeFirst();
                    auto value=lst.join(__splitEnv).trimmed();
                    vHash.insert(key, value.isEmpty()?QVariant{}:value);
                }
            }
            return vHash;
        }
        default:
            {
                QVariantHash vHash;
                auto lst=vBody.toString().split(__splitEnv);
                vBody.clear();
                if(lst.size()>1)
                    vHash.insert(lst.takeFirst(), lst.join(__splitEnv));
                else
                    vHash.insert(lst.takeFirst(), {});
                return vHash;
            }
        }

        return {};
    }

private:

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

    static const QVariantHash toEnvHash(const QVariant &envs)
    {
        QVariantHash __return;

        Q_DECLARE_VU;

        QVariantList lst;
        auto value=vu.toVariant(envs);
        switch (value.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            return value.toHash();
            break;
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            lst=value.toList();
            break;
        default:
            lst=QVariant{value.toString().split('\n')}.toList();
            break;
        }
        for (auto &v : lst) {
            auto s = v.toString().trimmed().split(__splitEnv);
            auto env = s.first().trimmed();
            auto value = s.size()==1?"":s.last().trimmed();
            __return.insert(env, value);
        }

        return __return;

    }
    static const QString parserToStr(const QVariant &value)
    {
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

    static const QStringList getEnvKeys(const QVariant &values)
    {
        QStringList __return;
        Q_DECLARE_VU;
        auto bytes=vu.toByteArray(values);
        QRegularExpression reA(__envRegExt);
        QRegularExpressionMatchIterator i = reA.globalMatch(bytes);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            if (match.hasMatch()){
                auto key=match.captured(0).trimmed().toLower();
                if(!__return.contains(key))
                __return.append(key);
            }
        }
#ifdef QT_DEBUG
        __return.sort();
#endif
        return __return;
    }

    static const QString parser(const QVariant &values, const QVariantHash &envs)
    {
        Q_DECLARE_VU;
        QString textParser=parserDeclaredEnvs(vu.toByteArray(values));
        static QRegularExpression re(__envRegExt);

        QHashIterator<QString, QVariant> i(envs);
        while (i.hasNext()) {
            i.next();
            const auto&key=i.key();
            auto value = parserToStr(i.value());
            textParser = textParser.replace(key, value, Qt::CaseSensitive);
            if(!re.match(textParser).hasMatch())
                break;
        }

        return textParser;
    }

    static void addEnv(EnvsPvt*envPvt, QVariantHash&envs, const QString &envKey, const QVariant &envValue)
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
        if(re.match(value).hasMatch())
            value=fullParser(envPvt, value).toString();


        v=vu.toVariant(v);
        envs.insert(key, v);
    }

    void addCustomEnv(const QString &envKey, const QVariant &envValue)
    {
        addEnv(this, customEnvs, envKey, envValue);
    }


public:

    void putSystemEnvs(const QVariant &envs)
    {
        auto vHash=toEnvHash(envs);
        QHashIterator<QString, QVariant> i(vHash);
        while(i.hasNext()){
            i.next();
            auto key=i.key().trimmed().toUtf8();
            auto value=i.value().toByteArray().trimmed();
            qputenv(key, value);
        }
    }

    void reset()
    {
        this->dirEnvs=*static_DirEnvs;
        this->systemEnvs=*static_SystemEnvs;
        this->customEnvs.clear();
    }

    void clear()
    {
        this->dirEnvs.clear();
        this->systemEnvs.clear();
        this->customEnvs.clear();
        this->invalidEnvsClean=false;
        this->invalidEnvsValue={};
        this->ignoreSystemEnvs=false;
        this->ignoreDirEnvs=false;
        this->ignoreCustomEnvs=false;
        this->clearUnfoundEnvs=false;
    }

    bool contains(const QString &env)const{
        auto key=(*__argVar).arg(env).toLower();
        if(this->dirEnvs.contains(key))
            return true;
        if(this->systemEnvs.contains(key))
            return true;
        return false;
    }

    static const QVariant staticParser(const QVariant &values, const QVariant &envs, bool clearNullEnvs)
    {
        Q_DECLARE_VU;
        auto value=parserDeclaredEnvs(vu.toByteArray(values));

        value = parser(value, *static_SystemEnvs);

        value = parser(value, *static_DirEnvs);

        value = parser(value, vu.toHash(envs));

        if(clearNullEnvs)
            value=clearEnvs(value);

        return vu.toType(values.typeId(), value);
    }

    static const QVariant fullParser(EnvsPvt *envPvt, const QVariant &values)
    {
        Q_DECLARE_VU;
        auto value=parserDeclaredEnvs(vu.toByteArray(values));

        if(!envPvt->ignoreSystemEnvs)
            value = parser(value, envPvt->systemEnvs);

        if(!envPvt->ignoreCustomEnvs)
            value = parser(value, envPvt->argumentEnvs);

        if(!envPvt->ignoreDirEnvs)
            value = parser(value, envPvt->dirEnvs);

        if(!envPvt->ignoreCustomEnvs)
            value = parser(value, envPvt->customEnvs);


        if(envPvt->clearUnfoundEnvs)
            value=clearEnvs(value);

        return vu.toType(values.typeId(), value);
    }

    static QString getEnvValue(EnvsPvt*envPvt, const QString &env)
    {
        QString key;
        if(env.startsWith(__argVarStartWith))
            key=env;
        else
            key=(*__argVar).arg(env).toLower();

        auto value=envPvt->customEnvs.value(key).toString().trimmed();
        if(value.isEmpty())
            value=envPvt->dirEnvs.value(key).toString().trimmed();

        if(value.isEmpty())
            value=envPvt->systemEnvs.value(key).toString().trimmed();

        if(value.contains(key,Qt::CaseSensitive)){
            sWarning()<<tr("env[%1] contaisn it'self").arg(key);
            return {};
        }
        return value;
    }

    static void setEnvs(EnvsPvt*envPvt, QVariantHash &envs, const QVariant &customValues)
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
                addEnv(envPvt, envs, i.key(), i.value());
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
                    setEnvs(envPvt, envs, envItem);
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
                addEnv(envPvt, envs, key, value);
            }
            break;
        }
        default:
            auto text=values.toString().trimmed();
            if(!text.contains(__splitEnvs)){
                envPvt->addEnv(envPvt, envs, text, {});
            }
            else{
                auto vListText=text.split(__splitEnvs);
                if(!vListText.isEmpty())
                    envPvt->setEnvs(envPvt, envs, vListText);
            }
            break;
        }
    }

    static void putEnvs(EnvsPvt*envPvt, QVariantHash &envs, const QVariant &customValues)
    {
        QRegularExpression reA(__envRegExt);
        auto vHash=toEnvHash(customValues);
        {
            QHashIterator<QString, QVariant> i(vHash);
            while(i.hasNext()){
                i.next();
                auto key=i.key().trimmed().toUtf8();
                auto value=i.value().toByteArray().trimmed();
                if(!reA.match(value).hasMatch())
                    setEnvs(envPvt, envs, QVariantHash{{key, value}});
            }
        }
        {
            QHashIterator<QString, QVariant> i(vHash);
            while(i.hasNext()){
                i.next();
                auto key=i.key().trimmed().toUtf8();
                auto value=i.value().toString().trimmed();
                if(reA.match(value).hasMatch()){
                    auto envList=getEnvKeys(value);
                    for(auto&v:envList){
                        auto envVal=getEnvValue(envPvt, v);
                        value=value.replace(v,envVal,Qt::CaseInsensitive);
                    }
                    setEnvs(envPvt, envs, QVariantHash{{key, value}});
                }
            }
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
    p->setEnvs(p, p->customEnvs, customEnvs);
}

Envs &Envs::reset()
{
    p->reset();
    return *this;
}

Envs &Envs::clear()
{
    p->clear();
    return *this;
}

QVariant Envs::parser(const QVariant &values)const
{
    return p->fullParser(p,values);
}

const QVariant Envs::parser(const QVariant &values, const QVariant &envs)
{
    return EnvsPvt::staticParser(values, envs, false);
}

const QVariantHash &Envs::systemEnvs() const
{
    return *static_SystemEnvs;
}

Envs &Envs::systemEnvs(const QVariant &envs)
{
    p->putSystemEnvs(envs);
    return *this;
}

Envs &Envs::systemEnvs(const QString &env, const QVariant &value)
{
    p->putSystemEnvs(QVariantHash{{env,value}});
    return *this;
}

Envs &Envs::systemEnvs(QFile &envs)
{
    Q_DECLARE_VU;
    p->putSystemEnvs(p->readEnvFile(envs));
    return *this;
}

bool Envs::contains(const QString &env) const
{
    return p->contains(env);
}

QVariant Envs::value(const QString &env)
{
    return p->fullParser(p, __argVar->arg(env));
}

QVariantHash &Envs::argumentEnvs() const
{
    return p->argumentEnvs;
}

Envs &Envs::argumentEnvs(const QVariant &newCustomEnvs)
{
    Q_DECLARE_VU;
    p->argumentEnvs.clear();
    p->setEnvs(p, p->customEnvs, newCustomEnvs);
    emit customEnvsChanged();
    return *this;
}

Envs &Envs::argumentEnvs(const QString &env, const QVariant &value)
{
    Q_DECLARE_VU;
    p->setEnvs(p, p->argumentEnvs, QVariantHash{{env, value}});
    emit argumentEnvsChanged();
    return *this;
}

Envs &Envs::argumentEnvs(QFile &envs)
{
    p->putEnvs(p, p->argumentEnvs, p->readEnvFile(envs));
    return *this;
}

Envs &Envs::resetArgumentEnvs()
{
    return this->argumentEnvs({});
}

QVariantHash &Envs::customEnvs() const
{
    return p->customEnvs;
}

Envs &Envs::customEnvs(const QVariant &newCustomEnvs)
{
    Q_DECLARE_VU;
    p->customEnvs.clear();
    p->setEnvs(p, p->customEnvs, newCustomEnvs);
    emit customEnvsChanged();
    return *this;
}

Envs &Envs::customEnvs(const QString &env, const QVariant &value)
{
    Q_DECLARE_VU;
    p->setEnvs(p, p->customEnvs, QVariantHash{{env, value}});
    emit customEnvsChanged();
    return *this;
}

Envs &Envs::customEnvs(QFile &envs)
{
    p->putEnvs(p, p->customEnvs, p->readEnvFile(envs));
    return *this;
}

Envs &Envs::resetCustomEnvs()
{
    return this->customEnvs({});
}

Envs &Envs::customEnv(const QVariant &newCustomEnv)
{
    Q_DECLARE_VU;
    p->setEnvs(p, p->customEnvs, newCustomEnv);
    emit customEnvsChanged();
    return *this;
}

Envs &Envs::customEnv(const QString &key, const QVariant &value)
{
    Q_DECLARE_VU;
    p->setEnvs(p, p->customEnvs, QVariantHash{{key, value}});
    emit customEnvsChanged();
    return *this;
}

bool Envs::invalidEnvsClean() const
{
    return p->invalidEnvsClean;
}

Envs &Envs::invalidEnvsClean(bool newinvalidEnvsClean)
{
    if (p->invalidEnvsClean == newinvalidEnvsClean)
        return *this;
    p->invalidEnvsClean = newinvalidEnvsClean;
    emit invalidEnvsCleanChanged();
    return *this;
}

Envs &Envs::resetInvalidEnvsClean()
{
    return invalidEnvsClean({});
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

bool Envs::ignoreArgumentEnvs() const
{
    return p->ignoreArgumentEnvs;
}

Envs &Envs::ignoreArgumentEnvs(bool newIgnoreArgumentEnvs)
{
    if (p->ignoreArgumentEnvs == newIgnoreArgumentEnvs)
        return *this;
    p->ignoreArgumentEnvs = newIgnoreArgumentEnvs;
    emit ignoreArgumentEnvsChanged();
    return *this;
}

Envs &Envs::resetIgnoreArgumentEnvs()
{
    return ignoreArgumentEnvs({});
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
