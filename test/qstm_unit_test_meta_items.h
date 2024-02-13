#pragma once

#include "./qstm_test.h"
#include "../src/qstm_meta_items.h"
#include <QCryptographicHash>
#include <QVariant>
#include <QDate>
#include <QTime>
#include <QDateTime>

namespace QStm {

class TestMetaItem:public QObject
{
    Q_INVOKABLE TestMetaItem(QObject *parent=nullptr):QObject{parent}{}
};

class UT_MetaItems : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_MetaItems)
public:
    QStm::MetaItems<TestMetaItem> items;

    Q_INVOKABLE void test_ToMd5Uuid()
    {
        // auto valueCheck=QUuid::fromString("{61aeafe9-ad94-ffd2-7548-961f4aabe3dd}");
        // auto var01="5s6d45a6s";
        // auto var02=QCryptographicHash::hash(var01, QCryptographicHash::Md5).toHex();
        // auto var03=valueCheck.toByteArray();
        // auto var04=valueCheck.toString().replace("-","").replace("{","").replace("}","");

        // QCOMPARE_EQ(hu.toMd5Uuid(var01), valueCheck);
        // QCOMPARE_EQ(hu.toMd5Uuid(var02), valueCheck);
        // QCOMPARE_EQ(hu.toMd5Uuid(var03), valueCheck);
        // QCOMPARE_EQ(hu.toMd5Uuid(var04), valueCheck);
    }
};
}
