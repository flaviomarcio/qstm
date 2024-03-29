#pragma once

#include "./qstm_test.h"
#include "../src/qstm_util_formatting.h"

namespace QStm {

class UT_FormattingUtil : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_FormattingUtil)
public:
    FormattingUtil u;

    Q_INVOKABLE void test_checkMasks()
    {
        auto check_mask_date = u.masks().date();
        auto check_mask_time = u.masks().time();
        auto check_mask_dateTime = u.masks().dateTime();
        auto check_mask_currency = u.masks().currency();
        auto check_mask_number = u.masks().number();
        auto check_mask_numeric = u.masks().numeric();
        auto check_mask_percent = u.masks().percent();
        auto check_mask_booleanTrue = u.masks().boolean(true);
        auto check_mask_booleanFalse = u.masks().boolean(false);

        auto expected_mask_date = QStringLiteral("dd/MM/yyyy");
        auto expected_mask_time = QStringLiteral("hh:mm:ss");
        auto expected_mask_dateTime = QStringLiteral("dd/MM/yyyy hh:mm:ss");
        auto expected_mask_currency = "";
        auto expected_mask_number = "";
        auto expected_mask_numeric = "";
        auto expected_mask_percent = "";
        auto expected_mask_booleanTrue = QStringLiteral("Sim");
        auto expected_mask_booleanFalse = QStringLiteral("Não");

            QCOMPARE_EQ( expected_mask_date, check_mask_date );
        QCOMPARE_EQ( expected_mask_time, check_mask_time );
        QCOMPARE_EQ( expected_mask_dateTime, check_mask_dateTime );
        QCOMPARE_EQ( expected_mask_currency, check_mask_currency );
        QCOMPARE_EQ( expected_mask_number, check_mask_number );
        QCOMPARE_EQ( expected_mask_numeric, check_mask_numeric );
        QCOMPARE_EQ( expected_mask_percent, check_mask_percent );
        QCOMPARE_EQ( expected_mask_booleanTrue, check_mask_booleanTrue );
        QCOMPARE_EQ( expected_mask_booleanFalse, check_mask_booleanFalse);

    }

    Q_INVOKABLE void test_CheckType()
    {
        FormattingUtil u;
        QHash<QString, QVariant> valueVsExpected;

        valueVsExpected.insert("01/01/1901", QDate(1901,1,1));
        valueVsExpected.insert("01:02:59", QTime(01,02,59));
        valueVsExpected.insert("01/01/1901 01:02:59", QDateTime(QDate(1901,1,1), QTime(01,02,59)));
        valueVsExpected.insert("Sim", true);
        valueVsExpected.insert("Não", false);
            valueVsExpected.insert("Atenção", "Atenção");

        valueVsExpected.insert("50100.010000", (50100.01));
        valueVsExpected.insert("5000.010000", (5000.01));
        valueVsExpected.insert("501.010000", (501.01));
        valueVsExpected.insert("5", (5));
        valueVsExpected.insert("5.009000", (5.009));
        valueVsExpected.insert("5.004000", (5.004));

        valueVsExpected.insert("60100", u.toInt(60100.01));
        valueVsExpected.insert("6000", u.toInt(6000.01));
        valueVsExpected.insert("601", u.toInt(601.01));
        valueVsExpected.insert("6", u.toInt(6));
        valueVsExpected.insert("6", u.toInt(6.009));
        valueVsExpected.insert("6", u.toInt(6.004));

        valueVsExpected.insert("70100.010000", u.toDouble(70100.01));
        valueVsExpected.insert("7000.010000", u.toDouble(7000.01));
        valueVsExpected.insert("701.010000", u.toDouble(701.01));
        valueVsExpected.insert("7.000000", u.toDouble(7));
        valueVsExpected.insert("7.009000", u.toDouble(7.009));
        valueVsExpected.insert("7.004000", u.toDouble(7.004));


        valueVsExpected.insert("80100.01", u.toPercent(80100.01));
        valueVsExpected.insert("8000.01", u.toPercent(8000.01));
        valueVsExpected.insert("801.01", u.toPercent(801.01));
        valueVsExpected.insert("8.01", u.toPercent(8));
        valueVsExpected.insert("8.01", u.toPercent(8.009));
        valueVsExpected.insert("8.00", u.toCurrency(8.004));

        valueVsExpected.insert("90100.01", u.toCurrency(90100.01));
        valueVsExpected.insert("9000.01", u.toCurrency(9000.01));
        valueVsExpected.insert("901.01", u.toCurrency(901.01));
        valueVsExpected.insert("9.00", u.toCurrency(9));
        valueVsExpected.insert("9.01", u.toCurrency(9.005));
        valueVsExpected.insert("9.00", u.toCurrency(9.004));

        QHashIterator<QString, QVariant> i(valueVsExpected);
        while (i.hasNext()) {
            i.next();
            auto v=i.value();
            auto vExpected=i.key();
            auto vCheck=u.v(v);
            QVERIFY(vExpected==vCheck);
        }


    }
};
}
