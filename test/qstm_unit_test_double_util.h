#pragma once

#include "./qstm_test.h"
#include "../src/qstm_util_double.h"

namespace QStm {

class UT_DoubleUtil : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_DoubleUtil)
public:
    DoubleUtil u;

    Q_INVOKABLE void test_CheckConversor()
    {
        const double v=12.55;
        u=v;
        QVERIFY(u.toDouble()==v);
    }

    Q_INVOKABLE void test_CheckBetween()
    {

        {
            qlonglong v1=2,v2=1;
            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);

            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);
        }

        {
            int v1=2,v2=1;
            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);

            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);
        }

        {
            const double cV1=1.05, cV2=1.06;
            double v1=cV2,v2=cV1;

            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);
            QVERIFY(v1==cV1);
            QVERIFY(v2==cV2);

            QVERIFY(u.checkBetween(v1,v2));
            QVERIFY(v1<v2);
            QVERIFY(v1==cV1);
            QVERIFY(v2==cV2);
        }
    }

};

}
