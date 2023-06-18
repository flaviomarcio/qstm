#pragma once

#include "./qstm_test.h"
#include "../src/qstm_envs.h"
#include "../src/qstm_util_variant.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantList>
#include <QVariantHash>

namespace QStm {
class UT_Envs : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_Envs)
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

    Q_INVOKABLE void test_checkLoadEnvsArrayArrayEnvName()
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

        QCOMPARE_EQ(envsOutBytes, envsInBytes);
    }

    Q_INVOKABLE void test_checkLoadEnvsArrayArrayObjectText()
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

        QCOMPARE_EQ(envsOutBytes, envsInBytes);
    }

    Q_INVOKABLE void test_checkLoadEnvsObjectJson()
    {
        auto envsIn=QVariantHash{
            {"envA","valueA"},
            {"envB","valueB"},
            {"envC",{}},
            {"envD",QVariantList{{"1","2","3","4"}}}
        };
        auto envsOutBytes=toEnv(envsIn);
        auto envsInBytes=toEnv(envs.customEnvs(envsIn).customEnvs());

        QCOMPARE_EQ(envsOutBytes, envsInBytes);
    }

    Q_INVOKABLE void test_checkLoadEnvsArrayJsonObject()
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

        QCOMPARE_EQ(envsOutBytes, envsInBytes);
    }

    Q_INVOKABLE void test_checkLoadEnvsErrorSetKey()
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

        QCOMPARE_EQ(envsOutBytes, envsInBytes);
    }

    Q_INVOKABLE void test_checkLoadEnvsErrorSetValues()
    {
        auto envsIn=QVariantList{
            QVariantHash{{"${envA}","valueA"}},
            QVariantHash{{"${envB}","valueB"}},
            QVariantHash{{"${envC}",{}}},
            QVariantHash{{"${envD}",QVariantList{{"1","2","3","4"}}}}
        };
        auto outHash=envs.customEnvs(envsIn).customEnvs();

        for(auto&v:envsIn){
            auto inHash=v.toHash();
            QHashIterator<QString, QVariant> i(inHash);
            while(i.hasNext()){
                i.next();
                auto key=QString(i.key().toLower()).replace("$","").replace("{","").replace("}","");
                auto vA=i.value().toByteArray();
                auto vB=outHash.value(key).toByteArray();
                QCOMPARE_EQ(vA,vB);
            }
        }
        QCOMPARE_EQ(outHash.size(),envsIn.size());
    }

    Q_INVOKABLE void test_replaceVarText()
    {
        const auto envsIn=QVariantHash{
            {"envA","valueA"}
        };

        {
            auto parserTextIn="item1=envA,item1=${envA}";
            auto envsOutBytesA=QString("item1=envA,item1=valueA");
            auto envsOutBytesB=QString("item1=valueA,item1=envA");

            auto envsInBytes=envs
                                   .customEnvs(envsIn)
                                   .parser(parserTextIn)
                                   .toString();

            QVERIFY((envsInBytes==envsOutBytesA || envsInBytes==envsOutBytesB));
        }
    }

    Q_INVOKABLE void test_replaceVarArray()
    {
        const auto envsIn=QVariantHash{
            {"envA",QVariantList{{"1","2","3","4"}}}
        };

        {
            auto parserTextIn="item1=envA,item1=${envA}";
            auto envsOutBytesA=QString("item1=envA,item1=1,2,3,4");
            auto envsOutBytesB=QString("item1=1,2,3,4,item1=envA");

            auto envsInBytes=envs
                                   .customEnvs(envsIn)
                                   .parser(parserTextIn)
                                   .toString();

            QVERIFY((envsInBytes==envsOutBytesA || envsInBytes==envsOutBytesB));
        }
    }

    Q_INVOKABLE void test_replaceVarObject()
    {
        const auto envsIn=QVariantHash{{"envA",QVariantHash{{"a","aaa"},{"b","bbb"}}}};

        {
            auto parserTextIn="item1=envA,item1:${envA}";
            auto envsOutBytesA=QString("item1=envA,item1:a=aaa,b=bbb");
            auto envsOutBytesB=QString("item1=envA,item1:b=bbb,a=aaa");

            auto envsInBytes=envs
                                   .customEnvs(envsIn)
                                   .parser(parserTextIn)
                                   .toString();

            QVERIFY(envsInBytes==envsOutBytesA || envsInBytes==envsOutBytesB);
        }
    }

    Q_INVOKABLE void test_replaceSystemEnvs()
    {
        const auto envsIn=QVariantHash{{"user",QByteArray{getenv("USER")}}};

        {
            auto parserTextIn="user=${user}";
            auto envsOutBytes=QString("user=%1").arg(QByteArray{getenv("USER")});

            auto envsInBytes=envs
                                   .customEnvs(envsIn)
                                   .parser(parserTextIn)
                                   .toString();

            QCOMPARE_EQ(envsOutBytes, envsInBytes);
        }
    }

};

}
