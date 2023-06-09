#include "qstm_setting_file.h"
#include <QVariantHash>
#include <QJsonDocument>
#include <QFile>

namespace QStm {

class SettingFilePvt:public QObject
{
public:
    QStringList setting;
    QStringList envs;

    explicit SettingFilePvt(SettingFile *parent):QObject{parent}
    {
    }

//    static QVariantHash unionVariant(const QVariant &newSetting)
//    {
//        if(!newSetting.isValid())
//            return {};

//        auto vSettings=newSetting;
//        switch (vSettings.typeId()) {
//        case QMetaType::QVariantList:
//        case QMetaType::QStringList:
//        {
//            auto vList=vSettings.toList();
//            for(auto &v: vList){
//                QFile file(v.toByteArray());
//                if(!file.exists())
//                    continue;
//                if(!file.open(file.ReadOnly))
//                    continue;
//                auto bytes=file.readAll();
//                file.close();
//                auto jVar=QJsonDocument::fromJson(bytes).toVariant();

//                switch (jVar.typeId()) {
//                case QMetaType::QVariantList:
//                case QMetaType::QStringList:
//                case QMetaType::QVariantHash:
//                case QMetaType::QVariantMap:
//                case QMetaType::QVariantPair:
//                    v=jVar;
//                    break;
//                default:
//                    continue;
//                }
//            }
//            vSettings=vList;
//            break;
//        }
//        default:
//            break;
//        }

//        QVariantHash vSettingsHash;
//        switch (vSettings.typeId()) {
//        case QMetaType::QVariantHash:
//        case QMetaType::QVariantMap:
//            vSettingsHash=vSettings.toHash();
//            break;
//        case QMetaType::QVariantList:
//        {
//            auto vList=vSettings.toList();
//            for(auto &v:vList){
//                auto vHash=v.toHash();
//                QHashIterator<QString, QVariant> i(vHash);
//                while(i.hasNext()){
//                    i.next();
//                    auto &vSettingValue=vSettingsHash[i.key()];
//                    if(!vSettingValue.isValid()){
//                        vSettingValue=i.value();
//                        continue;
//                    }
//                    switch (vSettingValue.typeId()) {
//                    case QMetaType::QVariantHash:
//                    case QMetaType::QVariantMap:
//                    case QMetaType::QVariantPair:
//                    {
//                        auto vDst=vSettingValue.toHash();
//                        auto vSrc=i.value().toHash();
//                        QHashIterator<QString, QVariant> i(vSrc);
//                        while(i.hasNext()){
//                            i.next();
//                            vDst.insert(i.key(), i.value());
//                        }
//                        vSettingValue=vDst;
//                        break;
//                    }
//                    case QMetaType::QVariantList:
//                    case QMetaType::QStringList:
//                    {
//                        auto vDst=vSettingValue.toList();
//                        auto vSrc=i.value().toList();
//                        for(auto &v: vSrc)
//                            vDst.append(v);
//                        vSettingValue=vDst;
//                        break;
//                    }
//                    default:
//                        break;
//                    }
//                }
//            }
//            break;
//        }
//        default:
//            break;
//        }
//        return vSettingsHash;
//    }
};

SettingFile::SettingFile(QObject *parent)
    : QObject{parent}
{
    this->p=new SettingFilePvt{this};
}

SettingFile::SettingFile(const QVariant &setting, const QVariant &envs)
{
    this->p=new SettingFilePvt{this};
    this->p->setting=setting.toStringList();
    this->p->envs=envs.toStringList();
}

SettingFile &SettingFile::clear()
{
    this->p->setting.clear();
    this->p->envs.clear();
    return *this;
}

QStringList &SettingFile::setting() const
{
    return p->setting;
}

SettingFile &SettingFile::setting(const QVariant &newSetting)
{
    auto &varList=p->setting;
    auto &newValues=newSetting;
    QStringList strList;
    switch (newValues.typeId()) {
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
        strList=newValues.toStringList();
        break;
    default:
        auto fileName=newValues.toString().trimmed();
        if(!fileName.isEmpty())
            strList.append(fileName);
        break;
    }

    varList.clear();
    for(auto &fileName : strList){
        if(!QFile::exists(fileName))
            continue;
        varList.append(fileName);
    }
    emit settingChanged();
    return *this;
}

SettingFile &SettingFile::resetSetting()
{
    return setting({});
}

QStringList &SettingFile::envs() const
{
    return p->envs;
}

SettingFile &SettingFile::envs(const QVariant &newEnvs)
{
    auto &varList=p->envs;
    auto &newValues=newEnvs;
    QStringList strList;
    switch (newValues.typeId()) {
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
        strList=newValues.toStringList();
        break;
    default:
        auto fileName=newValues.toString().trimmed();
        if(!fileName.isEmpty())
            strList.append(fileName);
        break;
    }

    varList.clear();
    for(auto &fileName : strList){
        if(!QFile::exists(fileName))
            continue;
        varList.append(fileName);
    }
    emit envsChanged();
    return *this;
}

SettingFile &SettingFile::resetEnvs()
{
    return envs({});
}

} // namespace QApr
