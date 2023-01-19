#ifndef Q_STM_EnvsFunctional_H
#define Q_STM_EnvsFunctional_H

#include "./qstm_test_unit.h"
#include "../src/qstm_envs.h"
#include "../src/qstm_util_variant.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantList>
#include <QVariantHash>

namespace QStm {
class Q_STM_EnvsFunctional : public SDKGoogleTestUnit {
public:
    Envs envs;
    static QByteArray toEnv(const QVariantHash &envs)
    {
        QVariantHash __return;
        QHashIterator<QString, QVariant> i(envs);
        while (i.hasNext()) {
            i.next();
            auto key=QStringLiteral("${%1}").arg(i.key()).trimmed().toLower();
            if(i.key().startsWith("${"))
                __return.insert(i.key(), i.value());
            else
                __return.insert(key, i.value());
        }
        return QJsonDocument::fromVariant(__return).toJson();
    }

    static QByteArray toEnv(const QVariantList &envs)
    {
        QVariantList __return;
        Q_DECLARE_VU;
        for(auto&v:envs){
            auto env=vu.toVariant(v);
            switch (env.typeId()) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantPair:
            {
                __return.append(toEnv(env.toHash()));
                break;
            }
            default:
                __return.append(v);
                break;
            }
        }
        return QJsonDocument::fromVariant(__return).toJson();
    }
};

TEST_F(Q_STM_EnvsFunctional, checkLoadEnvsArrayArrayEnvName)
{
    QString envsIn="envA=valueA|envB=valueB|envC|envD=1,2,3,4";
    QVariantHash envsOut={
        {"envA","valueA"},
        {"envB","valueB"},
        {"envC",{}},
        {"envD",QVariantList{{"1","2","3","4"}}}
    };

    auto envsOutBytes=toEnv(envsOut);
    auto envsInBytes=toEnv(envs.customEnvs(envsIn).customEnvs());

    EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
}

TEST_F(Q_STM_EnvsFunctional, checkLoadEnvsArrayArrayObjectText)
{
    QString envsIn="envA=valueA|envB=valueB|envC|envD=1,2,3,4";
    QVariantHash envsOut={
        {"envA","valueA"},
        {"envB","valueB"},
        {"envC",{}},
        {"envD",QVariantList{{"1","2","3","4"}}}
    };

    auto envsOutBytes=toEnv(envsOut);
    auto envsInBytes=toEnv(envs.customEnvs(envsIn).customEnvs());

    EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
}

TEST_F(Q_STM_EnvsFunctional, checkLoadEnvsObjectJson)
{
    auto envsIn=QVariantHash{
        {"envA","valueA"},
        {"envB","valueB"},
        {"envC",{}},
        {"envD",QVariantList{{"1","2","3","4"}}}
    };
    auto envsOutBytes=toEnv(envsIn);
    auto envsInBytes=toEnv(envs.customEnvs(envsIn).customEnvs());

    EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
}

TEST_F(Q_STM_EnvsFunctional, checkLoadEnvsArrayJsonObject)
{
    auto envsIn=QVariantList{
        QVariantHash{{"envA","valueA"}},
        QVariantHash{{"envB","valueB"}},
        QVariantHash{{"envC",{}}},
        QVariantHash{{"envD",QVariantList{{"1","2","3","4"}}}}
    };
    auto envsOut=QVariantHash{
        {"envA","valueA"},
        {"envB","valueB"},
        {"envC",{}},
        {"envD",QVariantList{{"1","2","3","4"}}}
    };

    auto envsOutBytes=toEnv(envsOut);
    auto envsInBytes=toEnv(envs.customEnvs(envsIn).customEnvs());

    EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
}

TEST_F(Q_STM_EnvsFunctional, checkLoadEnvsErrorSetKey)
{
    auto envsIn=QVariantList{
        QVariantHash{{"${envA}","valueA"}},
        QVariantHash{{"envB","valueB"}},
        QVariantHash{{"envC",{}}},
        QVariantHash{{"${envD}",QVariantList{{"1","2","3","4"}}}}
    };
    auto envsOut=QVariantHash{
        {"envA","valueA"},
        {"envB","valueB"},
        {"envC",{}},
        {"envD",QVariantList{{"1","2","3","4"}}}
    };

    auto envsOutBytes=toEnv(envsOut);
    auto envsInBytes=toEnv(envs.customEnvs(envsIn).customEnvs());

    EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
}

TEST_F(Q_STM_EnvsFunctional, checkLoadEnvsErrorSetValues)
{
    auto envsIn=QVariantList{
        QVariantHash{{"${envA}","valueA ${envA}"}},
        QVariantHash{{"${envB}","valueB ${envB}"}},
        QVariantHash{{"${envC}",{}}},
        QVariantHash{{"${envD}",QVariantList{{"1","2","3","4"}}}}
    };
    auto envsOut=QVariantHash{
        {"envA","valueA"},
        {"envB","valueB"},
        {"envC",{}},
        {"envD",QVariantList{{"1","2","3","4"}}}
    };

    auto envsOutBytes=toEnv(envsOut);
    auto envsInBytes=toEnv(envs.customEnvs(envsIn).customEnvs());

    EXPECT_NE(envsOutBytes, envsInBytes) <<"Invalid load env data";
}

TEST_F(Q_STM_EnvsFunctional, replaceVarText)
{
    const auto envsIn=QVariantHash{
        {"envA","valueA"}
    };

    {
        auto parserTextIn="item1=envA,item1=${envA}";
        auto envValue=envsIn.value("envA").toString();
        auto envsOutBytes=QString("item1=envA,item1=%1").arg(envValue);

        auto envsInBytes=envs
                .customEnvs(envsIn)
                .parser(parserTextIn)
                .toString();

        EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
    }
}

TEST_F(Q_STM_EnvsFunctional, replaceVarArray)
{
    const auto envsIn=QVariantHash{
        {"envA",QVariantList{{"1","2","3","4"}}}
    };

    {
        auto parserTextIn="item1=envA,item1=${envA}";
        auto envValue=envsIn.value("envA").toString();
        auto envsOutBytes=QString("item1=envA,item1=1,2,3,4").arg(envValue);

        auto envsInBytes=envs
                .customEnvs(envsIn)
                .parser(parserTextIn)
                .toString();

        EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
    }
}

TEST_F(Q_STM_EnvsFunctional, replaceVarObject)
{
    const auto envsIn=QVariantHash{
        {"envA",QVariantHash{{"a","aaa"},{"b","bbb"}}}
    };

    {
        auto parserTextIn="item1=envA,item1:${envA}";
        auto envValue=envsIn.value("envA").toString();
        auto envsOutBytes=QString("item1=envA,item1:a=aaa,b=bbb").arg(envValue);

        auto envsInBytes=envs
                .customEnvs(envsIn)
                .parser(parserTextIn)
                .toString();

        EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
    }
}

TEST_F(Q_STM_EnvsFunctional, replaceSystemEnvs)
{
    const auto envsIn=QVariantHash{
    {"user",QByteArray{getenv("USER")}}
    };

    {
        auto parserTextIn="user=${user}";
        auto envsOutBytes=QString("user=%1").arg(QByteArray{getenv("USER")});

        auto envsInBytes=envs
                .customEnvs(envsIn)
                .parser(parserTextIn)
                .toString();

        EXPECT_EQ(envsOutBytes, envsInBytes) <<"Invalid load env data";
    }
}

}


#endif
