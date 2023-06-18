#pragma once

#include "./qstm_test.h"
#include "../src/qstm_object.h"
#include "../src/qstm_currency.h"
#include "../src/qstm_util_variant.h"

namespace QStm {

class UT_ResultValue : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_ResultValue)
public:

    Q_INVOKABLE void test_operatorBool()
    {
        QStm::Object objectA;
        QStm::Object objectB;

        QCOMPARE_EQ(objectA.lr()==objectB.lr(),true);
        QCOMPARE_EQ(objectA.lr().isOk(),true);

        QCOMPARE_EQ(objectA.lr().setCritical("error"),false);
        QCOMPARE_EQ(objectA.lr()==objectB.lr(),false);
        QCOMPARE_EQ(objectA.lr().toString()=="error",true);

        objectA.lr().clear();
        QCOMPARE_EQ(objectA.lr()==objectB.lr(),true);
        QCOMPARE_EQ(objectA.lr().isOk(),true);

        QCOMPARE_EQ(objectA.lr().setCritical("error"),false);
        QCOMPARE_EQ(objectA.lr()==objectB.lr(),false);
        QCOMPARE_EQ(objectA.lr().toString()=="error",true);

        objectA.lr(objectB.lr());
        QCOMPARE_EQ(objectA.lr()==objectB.lr(),true);
        QCOMPARE_EQ(objectA.lr().isOk(),true);
    }

    Q_INVOKABLE void test_checkOperatorResultValue()
    {
        QStm::Object objectA;
        QStm::Object objectB;

        QCOMPARE_EQ(objectA.lr()==objectB.lr(),true);
        QCOMPARE_EQ(objectA.lr().setCritical("error"),false);
        objectA.lr()=objectB.lr();
        QCOMPARE_EQ(objectA.lr()==objectB.lr(),true);
    }

    Q_INVOKABLE void test_checkToString()
    {
        QStm::Object objectA;
        QStm::Object objectB;

        QCOMPARE_EQ(objectA.lr()==objectB.lr(),true);
        QCOMPARE_EQ(objectA.lr().setCritical("error"),false);
        QCOMPARE_EQ(objectA.lr()==objectB.lr(),false);
        QCOMPARE_EQ(objectA.lr().toString()=="error",true);
    }

    Q_INVOKABLE void test_checkResult()
    {
        QStm::Object objectA;
        QStm::Object objectB;
        QCOMPARE_EQ(objectA.lr().resultVariant().isValid(),false);

        QVariantList vListValues;

        vListValues<<'1';
        vListValues<<"2";
        vListValues<<int(1);
        vListValues<<qlonglong(1);
        vListValues<<qulonglong(1);
        vListValues<<uint(1);
        vListValues<<double(1);
        vListValues<<1.0;
        vListValues<<QCurrency(1.2);
        vListValues<<QByteArrayLiteral("bytearray");
        vListValues<<QStringLiteral("string");
        vListValues<<QVariantList{1,2,3};
        vListValues<<QVariantHash{{QStringLiteral("A"),QDate::currentDate()}, {"B",QDateTime::currentDateTime()}};
        vListValues<<QDate::currentDate();

        Q_DECLARE_VU;

        for(auto&v:vListValues){
            objectA.lr().clear()=v;
            QCOMPARE_EQ(objectA.lr().resultVariant().isValid(),true);
            auto vA=vu.toStr(v);
            auto vB=vu.toStr(objectA.lr().resultVariant());
            QCOMPARE_EQ(vA,vB);
            objectA.lr().clear();
            QCOMPARE_EQ(objectA.lr().resultVariant().isValid(),false);
        }
    }
};

}
