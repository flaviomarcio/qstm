#pragma once

#include "./qstm_test.h"
#include "../src/qstm_util_variant.h"
#include "../src/qstm_util_date.h"
#include <QCryptographicHash>
#include <QVariant>
#include <QDate>
#include <QTime>
#include <QDateTime>

namespace QStm {
class UT_VariantUtil : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_VariantUtil)
public:
    VariantUtil vu;

    Q_INVOKABLE void test_ToAlphaNumber()
    {
        static auto valueIn=QStringLiteral("-0123456789-.,A/?");
        static auto valueCheck=QStringLiteral("0123456789");
        auto valueOut=vu.toAlphaNumber(valueIn);
        QCOMPARE_EQ(valueCheck,valueOut);
    }

    Q_INVOKABLE void test_ToAlphaNumeric()
    {
        static auto valueIn=QStringLiteral("-0123456789-.,A/?");
        static auto valueCheck=QStringLiteral("-0123456789-.,");
        auto valueOut=vu.toAlphaNumeric(valueIn);
        QCOMPARE_EQ(valueCheck,valueOut);
    }

    Q_INVOKABLE void test_ToAlphaText()
    {
        static auto valueIn=QStringLiteral("-0123456789-.,AbC/?");
        static auto valueCheck=QStringLiteral("--.,AbC/?");
        auto valueOut=vu.toAlphaText(valueIn);
        QCOMPARE_EQ(valueCheck,valueOut);
    }

    Q_INVOKABLE void test_ToByteArray()
    {
        auto vHash=QVariantHash
            {
             {"propertyA","valueA"},
             {"propertyC",QDate::currentDate()},
             {"propertyD",QTime::currentTime()},
             {"propertyE",QDateTime::currentDateTime()},
             {"propertyF",1},
             {"propertyG",1.01},
             {"propertyH",true},
             };

        {
            //toByteArray
            {
                //hash
                auto bytesA=QJsonDocument::fromVariant(vHash).toJson(QJsonDocument::Compact);
                auto bytesB=vu.toByteArray(vHash);
                QCOMPARE_EQ(bytesA, bytesB);
            }
            {
                QVariantList vList={vHash};
                vHash.insert("propertyX",vHash);
                vList.append(vHash);

                auto bytesA=QJsonDocument::fromVariant(vList).toJson(QJsonDocument::Compact);
                auto bytesB=vu.toByteArray(vList);
                QCOMPARE_EQ(bytesA, bytesB);
            }
        }
        {
            QVariant varA=1234;
            QVariant varB="A957";
            QVariant varC=QUuid();
            QVariant varD=QUuid().toString();
            QVariant varE=QUrl::fromLocalFile("/tmp/Teste.txt");
            QVariant varF=QUrl("http://Localhost");
            QVariant varG=false;
            QVariant varH=true;
            QVariant varI=QDate(1901,01,01);
            QVariant varJ=QTime(23,59,59,999);
            QVariant varL=QDateTime(varI.toDate(),varJ.toTime());

            QCOMPARE_EQ(vu.toStr(varA),varA.toString());
            QCOMPARE_EQ(vu.toStr(varB),varB.toString());
            QCOMPARE_EQ(vu.toStr(varC),varC.toString());
            QCOMPARE_EQ(vu.toStr(varD),varD.toString());
            QCOMPARE_EQ(vu.toStr(varE),varE.toString());
            QCOMPARE_EQ(vu.toStr(varF),varF.toString());
            QCOMPARE_EQ(vu.toStr(varG),varG.toString());
            QCOMPARE_EQ(vu.toStr(varH),varH.toString());
            QCOMPARE_EQ(vu.toStr(varI),varI.toDate().toString(Qt::ISODate));
            QCOMPARE_EQ(vu.toStr(varJ),varJ.toTime().toString(Qt::ISODateWithMs));
            QCOMPARE_EQ(vu.toStr(varL),varL.toDateTime().toString(Qt::ISODateWithMs));
        }
    }

    Q_INVOKABLE void test_ToStr()
    {
        auto vHash=QVariantHash
            {
             {"propertyA","valueA"},
             {"propertyC",QDate::currentDate()},
             {"propertyD",QTime::currentTime()},
             {"propertyE",QDateTime::currentDateTime()},
             {"propertyF",1},
             {"propertyG",1.01},
             {"propertyH",true},
             };

        {
            //hash
            auto bytesA=QJsonDocument::fromVariant(vHash).toJson(QJsonDocument::Compact);
            auto bytesB=vu.toStr(vHash);
            QCOMPARE_EQ(bytesA, bytesB);
        }
        {
            QVariantList vList={vHash};
            vHash.insert("propertyX",vHash);
            vList.append(vHash);

            auto bytesA=QJsonDocument::fromVariant(vList).toJson(QJsonDocument::Compact);
            auto bytesB=vu.toStr(vList);
            QCOMPARE_EQ(bytesA, bytesB);
        }
        {
            QVariant varA=1234;
            QVariant varB="A957";
            QVariant varC=QUuid();
            QVariant varD=QUuid().toString();
            QVariant varE=QUrl::fromLocalFile("/tmp/Teste.txt");
            QVariant varF=QUrl("http://Localhost");
            QVariant varG=false;
            QVariant varH=true;
            QVariant varI=QDate(1901,01,01);
            QVariant varJ=QTime(23,59,59,999);
            QVariant varL=QDateTime(varI.toDate(),varJ.toTime());

            QCOMPARE_EQ(vu.toStr(varA),varA.toString());
            QCOMPARE_EQ(vu.toStr(varB),varB.toString());
            QCOMPARE_EQ(vu.toStr(varC),varC.toString());
            QCOMPARE_EQ(vu.toStr(varD),varD.toString());
            QCOMPARE_EQ(vu.toStr(varE),varE.toString());
            QCOMPARE_EQ(vu.toStr(varF),varF.toString());
            QCOMPARE_EQ(vu.toStr(varG),varG.toString());
            QCOMPARE_EQ(vu.toStr(varH),varH.toString());
            QCOMPARE_EQ(vu.toStr(varI),varI.toDate().toString(Qt::ISODate));
            QCOMPARE_EQ(vu.toStr(varJ),varJ.toTime().toString(Qt::ISODateWithMs));
            QCOMPARE_EQ(vu.toStr(varL),varL.toDateTime().toString(Qt::ISODateWithMs));
        }
    }

    Q_INVOKABLE void test_ToChar()
    {
        auto varA=1234;
        auto varB="A957";
        auto varC=QUuid();
        auto varD=QUuid().toString();
        auto varE=QUrl::fromLocalFile("/tmp/Teste.txt");
        auto varF=QUrl("http://Localhost");
        auto varG=false;
        auto varH=true;

        QCOMPARE_EQ(vu.toChar(varA),'1');
        QCOMPARE_EQ(vu.toChar(varB),'A');
        QCOMPARE_EQ(vu.toChar(varC),'{');
        QCOMPARE_EQ(vu.toChar(varD),'{');
        QCOMPARE_EQ(vu.toChar(varE),'f');
        QCOMPARE_EQ(vu.toChar(varF),'h');
        QCOMPARE_EQ(vu.toChar(varG),'f');
        QCOMPARE_EQ(vu.toChar(varH),'t');
    }

    Q_INVOKABLE void test_ToInt()
    {
        auto vList=QVariantList{1,1.2,9.7,1000,"1", "2.77", "2.777",""};
        for(auto&v:vList)
            QCOMPARE_EQ(vu.toInt(v),v.toInt());
    }

    Q_INVOKABLE void test_ToLongLong()
    {
        auto vList=QVariantList{1,1.2,9.7,1000,"1", "2.77", "2.777",""};
        for(auto&v:vList)
            QCOMPARE_EQ(vu.toLongLong(v),v.toLongLong());
    }

    Q_INVOKABLE void test_ToDouble()
    {
        auto vList=QVariantList{1,1.2,9.7,1000,"1", "2.77", "2.777",""};
        for(auto&v:vList)
            QCOMPARE_EQ(vu.toDouble(v),v.toDouble());
    }

    Q_INVOKABLE void test_ToDate()
    {
        DateUtil du;
        QVariant varA=QDate(1901,01,01);
        QVariant varB=QDate(1901,01,01).toString(Qt::ISODate);
        QVariant varC="1901-01-30";
        QVariant varD="1901-01-60";
        QVariant varE=QDateTime(QDate(1901,01,01),QTime::currentTime());

        QCOMPARE_EQ(vu.toDate(varA), du.toDate(varA));
        QCOMPARE_EQ(vu.toDate(varB), du.toDate(varB));
        QCOMPARE_EQ(vu.toDate(varC), du.toDate(varC));
        QCOMPARE_EQ(vu.toDate(varD), du.toDate(varD));
        QCOMPARE_EQ(vu.toDate(varE), du.toDate(varE));
    }

    Q_INVOKABLE void test_ToTime()
    {
        DateUtil du;
        QVariant varA=QTime(23,23,23);
        QVariant varB=QTime(23,23,23).toString(Qt::ISODateWithMs);
        QVariant varC="12:12:00";
        QVariant varD="1901-01-60T12:12:00";
        QVariant varE=QDateTime(QDate::currentDate(),QTime(23,23,12));

        QCOMPARE_EQ(vu.toTime(varA), du.toTime(varA));
        QCOMPARE_EQ(vu.toTime(varB), du.toTime(varB));
        QCOMPARE_EQ(vu.toTime(varC), du.toTime(varC));
        QCOMPARE_EQ(vu.toTime(varD), du.toTime(varD));
        QCOMPARE_EQ(vu.toTime(varE), du.toTime(varE));
    }

    Q_INVOKABLE void test_ToList()
    {
        auto var1=QVariantList{1,2,3};
        auto var2="[1,2,3]";
        auto var3="12:12:00";
        auto var4="1901-01-60T12:12:00";
        auto var5=QDateTime(QDate::currentDate(),QTime(23,23,12));
        auto var6=QVariantHash{{"A",var1},{"B",var2},{"C",var3}};
        auto var7="[]";
        auto var8="{}";

        QCOMPARE_EQ(vu.toList(var1).size(),3);
        QCOMPARE_EQ(vu.toList(var2).size(),3);
        QCOMPARE_EQ(vu.toList(var3).size(),1);
        QCOMPARE_EQ(vu.toList(var4).size(),1);
        QCOMPARE_EQ(vu.toList(var5).size(),1);
        QCOMPARE_EQ(vu.toList(var6).size(),1);
        QCOMPARE_EQ(vu.toStringList(var7).size(),0);
        QCOMPARE_EQ(vu.toStringList(var8).size(),0);
    }

    Q_INVOKABLE void test_ToStringList()
    {
        auto var1=QVariantList{1,2,3,QVariantHash{{"A",QDate::currentDate()}}};
        auto var2="[1,2,3]";
        auto var3="12:12:00";
        auto var4="1901-01-60T12:12:00";
        auto var5=QDateTime(QDate::currentDate(),QTime(23,23,12));
        auto var6=QVariantHash{{"A",var1},{"B",var2},{"C",var3}};
        auto var7="[]";
        auto var8="{}";

        QCOMPARE_EQ(vu.toStringList(var1).size(),4);
        QCOMPARE_EQ(vu.toStringList(var2).size(),3);
        QCOMPARE_EQ(vu.toStringList(var3).size(),1);
        QCOMPARE_EQ(vu.toStringList(var4).size(),1);
        QCOMPARE_EQ(vu.toStringList(var5).size(),1);
        QCOMPARE_EQ(vu.toStringList(var6).size(),1);
        QCOMPARE_EQ(vu.toStringList(var7).size(),0);
        QCOMPARE_EQ(vu.toStringList(var8).size(),0);
    }

    Q_INVOKABLE void test_ToHash()
    {
        auto var1=QVariantHash{{"A",QDate::currentDate()},{"B",QVariantList{1,2,3}}};
        auto var2=QJsonDocument::fromVariant(var1).toJson(QJsonDocument::Compact);
        auto var3="[]";
        auto var4="{}";
        auto var5=QVariant(var1).toMap();

        QCOMPARE_EQ(vu.toHash(var1).size(),2);
        QCOMPARE_EQ(vu.toHash(var2).size(),2);
        QCOMPARE_EQ(vu.toHash(var3).size(),0);
        QCOMPARE_EQ(vu.toHash(var4).size(),0);
        QCOMPARE_EQ(vu.toHash(var5).size(),2);
    }

    Q_INVOKABLE void test_ToMap()
    {
        auto var1=QVariantHash{{"A",QDate::currentDate()},{"B",QVariantList{1,2,3}}};
        auto var2=QJsonDocument::fromVariant(var1).toJson(QJsonDocument::Compact);
        auto var3="[]";
        auto var4="{}";
        auto var5=QVariant(var1).toMap();

        QCOMPARE_EQ(vu.toMap(var1).size(),2);
        QCOMPARE_EQ(vu.toMap(var2).size(),2);
        QCOMPARE_EQ(vu.toMap(var3).size(),0);
        QCOMPARE_EQ(vu.toMap(var4).size(),0);
        QCOMPARE_EQ(vu.toMap(var5).size(),2);
    }

    Q_INVOKABLE void test_ToBool()
    {
        auto var01=true ;
        auto var02=false;
        auto var03='t';
        auto var04="f";
        auto var05="v";
        auto var06="s";
        auto var07=1;
        auto var08=0;
        auto var09="y";
        auto var10="n";

        QCOMPARE_EQ(vu.toBool(var01),true );
        QCOMPARE_EQ(vu.toBool(var02),false);
        QCOMPARE_EQ(vu.toBool(var03),true );
        QCOMPARE_EQ(vu.toBool(var04),false);
        QCOMPARE_EQ(vu.toBool(var05),true );
        QCOMPARE_EQ(vu.toBool(var06),true );
        QCOMPARE_EQ(vu.toBool(var07),true );
        QCOMPARE_EQ(vu.toBool(var08),false);
        QCOMPARE_EQ(vu.toBool(var09),true );
        QCOMPARE_EQ(vu.toBool(var10),false);
    }

};
}
