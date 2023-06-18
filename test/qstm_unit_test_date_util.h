#pragma once

#include "./qstm_test.h"
#include "../src/qstm_util_date.h"

namespace QStm {

class UT_DateUtil : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_DateUtil)
public:
    DateUtil u;

    const QTime minTime=QTime{0,0,0,0};
    const QTime maxTime=QTime{23,59,59,999};
    const QDate minDate=QDate{1901,1,1};
    const QDate maxDate=QDate{2500,1,1};

    Q_INVOKABLE void test_CheckLists()
    {
        {
            const QDateTime v(minDate, QTime(11,22,33));
            u=v;
            QVERIFY(u.toDateTime()==v);
        }


        QVERIFY(u.firstMonthDate(QDate(1901,1,10)).date()==minDate);
        QVERIFY(u.lastMonthDate(QDate(1901,1,10)).date()==QDate(1901,1,31));
        QVERIFY(u.firstYearDate(QDate(1901,1,10)).date()==minDate);
        QVERIFY(u.lastYearDate(QDate(1901,1,10)).date()==QDate(1901,12,31));
        QVERIFY(u.listMonthDaysToDate(minDate.addDays(9)).count()==10);
        QVERIFY(u.listMonthDays(minDate).count()==31);
        QVERIFY(u.listYearDaysToDate(minDate.addMonths(1)).count()==32);

        QVERIFY(u.listYearDays(QDate(2023,1,1)).count()==365);
        QVERIFY(u.listYearDays(QDateTime(QDate(2023,1,1),QTime())).count()==365);

        QVERIFY(u.listYearDays("2023").count()==365);
        QVERIFY(u.listYearDays("2024").count()==366);
        QVERIFY(u.listYearDays("2023-01-01").count()==365);
        QVERIFY(u.listYearDays("2024-01-01").count()==366);
        QVERIFY(u.listYearDays(2023).count()==365);
        QVERIFY(u.listYearDays(2024).count()==366);


        QVERIFY(u.minTime()==minTime);
        QVERIFY(u.maxTime()==maxTime);

        QVERIFY(u.minDateTime()==QDateTime(QDate::currentDate(), minTime));
        QVERIFY(u.maxDateTime()==QDateTime(QDate::currentDate(), maxTime));

        QVERIFY(u.minDateTime(QDate::currentDate().addDays(1))==QDateTime(QDate::currentDate().addDays(1), minTime));
        QVERIFY(u.maxDateTime(QDate::currentDate().addDays(1))==QDateTime(QDate::currentDate().addDays(1), maxTime));

        QVERIFY(u.minMonthDateTime()==QDateTime(QDate::currentDate(), minTime));
        QVERIFY(u.minMonthDateTime(minDate.addDays(1))==QDateTime(minDate, minTime));

        QVERIFY(u.maxMonthDateTime()==QDateTime(QDate(QDate::currentDate().year(),QDate::currentDate().month(),1).addMonths(1).addDays(-1), maxTime));
        QVERIFY(u.maxMonthDateTime(minDate.addDays(1))==QDateTime(minDate.addMonths(1).addDays(-1), maxTime));
    }

    Q_INVOKABLE void test_CheckConversor()
    {
        QVERIFY(u.toDateTime(QDate::currentDate().addDays(1))==QDateTime(QDate::currentDate().addDays(1), {}));
        QVERIFY(u.toDateTime(QDateTime(QDate(1901,2,2), QTime(12,12,13)))==QDateTime(QDate(1901,2,2), QTime(12,12,13)));
        QVERIFY(!u.toDateTime(QTime(12,12,13)).isValid());
        QVERIFY(u.toDateTime("1901-03-04T12:12:13")==QDateTime(QDate(1901,03,04), QTime(12,12,13)));
        QVERIFY(u.toDateTime("1901-03-04")==QDateTime(QDate(1901,03,04), {}));
        QVERIFY(u.toDateTime("12:12:13")==QDateTime(QDate(), QTime(12,12,13)));


        QVERIFY(u.toDate(QDate::currentDate())==QDate::currentDate());
        QVERIFY(u.toDate(QDateTime(QDate(1901,2,2), QTime(12,12,13)))==QDate(1901,2,2));
        QVERIFY(u.toDate(QTime(12,12,13))==QDate());
        QVERIFY(u.toDate("1901-03-04T12:12:13")==QDate(1901,03,04));
        QVERIFY(u.toDate("1901-03-04")==QDate(1901,03,04));
        QVERIFY(u.toDate("12:12:13")==QDate());

        QVERIFY(u.toTime(QDate::currentDate())==QTime{});
        QVERIFY(u.toTime(QDateTime(QDate(1901,2,2), QTime(12,12,13)))==QTime(12,12,13));
        QVERIFY(u.toTime(QTime(12,12,13))==QTime(12,12,13));
        QVERIFY(u.toTime("1901-03-04T12:12:13")==QTime(12,12,13));
        QVERIFY(u.toTime("1901-03-04")==QTime{});
        QVERIFY(u.toTime("12:12:13")==QTime(12,12,13));

    }

    Q_INVOKABLE void test_CheckMax()
    {
        {
            QDateTime vMin, vMax;
            vMin=QDateTime(maxDate, QTime());
            vMax=QDateTime(minDate, QTime());
            QVERIFY(u.setMax(vMin,vMax));
            QVERIFY(vMin==QDateTime(minDate, minTime));
            QVERIFY(vMax==QDateTime(maxDate, maxTime));

            vMin=QDateTime(QDate(), QTime());
            vMax=QDateTime(QDate(), QTime());
            QVERIFY(u.setMax(vMin,vMax));
            QVERIFY(vMin==QDateTime(QDate::currentDate(), minTime));
            QVERIFY(vMax==QDateTime(QDate::currentDate(), maxTime));
        }

        {
            QTime vMin, vMax;
            vMin=QTime();
            vMax=QTime();
            QVERIFY(u.setMax(vMin,vMax));
            QVERIFY(vMin==minTime);
            QVERIFY(vMax==maxTime);

            vMin=QTime::currentTime();
            vMax=QTime::currentTime();
            QVERIFY(u.setMax(vMin,vMax));
            QVERIFY(vMin==minTime);
            QVERIFY(vMax==maxTime);
        }
    }

    Q_INVOKABLE void test_CheckRange()
    {
        {
            QDate vMin, vMax;
            vMin=QDate();
            vMax=QDate();
            u=QDate(1901,1,4);
            QVERIFY(u.setMonthRange(vMin,vMax));
            QVERIFY(vMin==minDate);
            QVERIFY(vMax==QDate(1901,1,31));
        }

        {
            QDateTime vMin, vMax;
            vMin=QDateTime();
            vMax=QDateTime();
            u=QDate(1901,1,4);
            QVERIFY(u.setMonthRange(vMin,vMax));
            QVERIFY(vMin==QDateTime(minDate, minTime));
            QVERIFY(vMax==QDateTime(QDate(1901,1,31), maxTime));
        }

        {
            QDate vMin, vMax;
            vMin=QDate();
            vMax=QDate();
            u=QDate(1901,1,4);
            QVERIFY(u.setYearRange(vMin,vMax));
            QVERIFY(vMin==minDate);
            QVERIFY(vMax==QDate(1901,12,31));
        }

        {
            QDateTime vMin, vMax;
            vMin=QDateTime();
            vMax=QDateTime();
            u=QDate(1901,1,4);
            QVERIFY(u.setYearRange(vMin,vMax));
            QVERIFY(vMin==QDateTime(minDate, minTime));
            QVERIFY(vMax==QDateTime(QDate(1901,12,31), maxTime));
        }
    }

    Q_INVOKABLE void test_CheckBetween()
    {


        {
            QDateTime vVal, vMin, vMax;
            vVal=QDateTime();
            vMin=QDateTime();
            vMax=QDateTime();
            QVERIFY(!u.validBetween(vVal, vMin,vMax));

            vVal=QDateTime(maxDate, QTime(12,12,12));
            vMin=QDateTime(maxDate, QTime(13,12,12));
            vMax=QDateTime(maxDate, maxTime);
            QVERIFY(!u.validBetween(vVal, vMin,vMax));

            vVal=QDateTime(maxDate, QTime(12,12,12));
            vMin=QDateTime(maxDate, QTime(12,12,12));
            vMax=QDateTime(maxDate, QTime(12,12,12));
            QVERIFY(u.validBetween(vVal, vMin,vMax));
        }


        {
            QDateTime v1=QDateTime(maxDate, QTime()), v2=QDateTime(minDate, QTime());
            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);

            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);
        }

        {
            QDate v1=maxDate, v2=minDate;
            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);

            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);
        }

        {
            QTime v1=QTime(23,59,59,999), v2=QTime(00,00,00);
            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);

            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);
        }
    }

    Q_INVOKABLE void test_CheckParseInterval()
    {
        QHash<QStringList,qlonglong> vHashCheck=
            {
                {{"1s","1sc","1second"},1},
                {{"10s","10sc","10second"},10},
                {{"1m","1mn","1minute"},60},
                {{"10m","10mn","10minute"},60*10},
                {{"10h","10hr","10hour"},60*60*10},
                {{"1d","1dd","1day"},60*60*24},
                {{"10d","10dd","10day"},60*60*24*10},
                {{"10mo","10month"},60*60*24*30*10},
                {{"1y","1yy","1year"},60*60*24*30*12}
            };

        QHashIterator<QStringList, qlonglong> i(vHashCheck);
        while (i.hasNext()) {
            i.next();
            qlonglong v2=i.value()*1000;
            for(auto&v:i.key()){
                qlonglong v1=u.parseInterval(v).toLongLong();
                QCOMPARE_EQ(v1,v2);
            }
        }

    }

};

}

