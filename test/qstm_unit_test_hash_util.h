#pragma once

#include "./qstm_test.h"
#include "../src/qstm_util_hash.h"
#include <QCryptographicHash>
#include <QVariant>
#include <QDate>
#include <QTime>
#include <QDateTime>

namespace QStm {
class UT_HashUtil : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_HashUtil)
public:
    HashUtil hu;

    Q_INVOKABLE void test_Is()
    {
        {//uuid
            auto varA=QUuid::createUuid();
            auto varB=varA.toByteArray();
            auto varC=QUuid();
            auto varD=QUuid().toString();
            auto varE=QUuid().toString().replace("0","c");
            auto varF=QCryptographicHash::hash("1", QCryptographicHash::Md5).toHex().replace("-","").replace("{","").replace("}","");

            QVERIFY(hu.isUuid(varA));
            QVERIFY(hu.isUuid(varB));
            QVERIFY(!hu.isUuid(varC));
            QVERIFY(!hu.isUuid(varD));
            QVERIFY(hu.isUuid(varE));
            QVERIFY(hu.isUuid(varF));
        }

        {//hexa
            for (int var = 0; var < 100; ++var) {
                auto varA=QCryptographicHash::hash(QUuid::createUuid().toByteArray(), QCryptographicHash::Md5).toHex().replace("-","").replace("{","").replace("}","");
                auto varB=varA.toUpper();
                auto varC=varA+"1234";
                auto varD="1234"+varA;
                auto varE=varA+"---";
                auto varF="teste123";
                QVERIFY(hu.isHex(varA));
                QVERIFY(hu.isHex(varB));
                QVERIFY(hu.isHex(varC));
                QVERIFY(hu.isHex(varD));
                QVERIFY(!hu.isHex(varE));
                QVERIFY(!hu.isHex(varF));
            }
        }

        {//Base64
            for (int var = 0; var < 100; ++var) {
                auto varA=QCryptographicHash::hash(QUuid::createUuid().toByteArray(), QCryptographicHash::Md5).toHex().replace("-","").replace("{","").replace("}","").toBase64();
                auto varB=varA.toUpper();
                auto varC="1234";
                QVERIFY(hu.isBase64(varA));
                QVERIFY(hu.isBase64(varB));
                QVERIFY(hu.isBase64(varC));
            }
        }
    }

    Q_INVOKABLE void test_ToUuid()
    {
        auto var01=QUuid::fromString("{61aeafe9-ad94-ffd2-7548-961f4aabe3dd}");
        auto var02=var01.toString().replace("-","").replace("{","").replace("}","");
        auto var03=QUuid::createUuid();
        auto var04=var03.toString();
        auto var06=QUuid();
        auto var07="";
        auto var08=QVariant{};

        QCOMPARE_EQ(hu.toUuid(var01), var01);
        QCOMPARE_EQ(hu.toUuid(var02), var01);
        QCOMPARE_EQ(hu.toUuid(var03), var03);
        QCOMPARE_EQ(hu.toUuid(var04), var03);
        QCOMPARE_EQ(hu.toUuid(var06), QUuid{});
        QCOMPARE_EQ(hu.toUuid(var07), QUuid{});
        QCOMPARE_EQ(hu.toUuid(var08), QUuid{});
    }

    Q_INVOKABLE void test_ToMd5()
    {
        auto var01="5s6d45a6s";
        auto var02=QUuid::fromString("{61aeafe9-ad94-ffd2-7548-961f4aabe3dd}");
        auto var03=var02.toString();
        auto var04=var02.toString().replace("-","").replace("{","").replace("}","");

        auto valueCheck = QCryptographicHash::hash(var01, QCryptographicHash::Md5).toHex();

        QCOMPARE_EQ(hu.toMd5(var01), valueCheck);
        QCOMPARE_EQ(hu.toMd5(var02), valueCheck);
        QCOMPARE_EQ(hu.toMd5(var03), valueCheck);
        QCOMPARE_EQ(hu.toMd5(var04), valueCheck);
    }

    Q_INVOKABLE void test_ToMd5Uuid()
    {
        auto valueCheck=QUuid::fromString("{61aeafe9-ad94-ffd2-7548-961f4aabe3dd}");
        auto var01="5s6d45a6s";
        auto var02=QCryptographicHash::hash(var01, QCryptographicHash::Md5).toHex();
        auto var03=valueCheck.toByteArray();
        auto var04=valueCheck.toString().replace("-","").replace("{","").replace("}","");

        QCOMPARE_EQ(hu.toMd5Uuid(var01), valueCheck);
        QCOMPARE_EQ(hu.toMd5Uuid(var02), valueCheck);
        QCOMPARE_EQ(hu.toMd5Uuid(var03), valueCheck);
        QCOMPARE_EQ(hu.toMd5Uuid(var04), valueCheck);
    }
};
}
