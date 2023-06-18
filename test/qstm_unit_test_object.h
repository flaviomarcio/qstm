#pragma once

#include "./qstm_test.h"

namespace QStm {

class UT_Object : public QStm::ObjectTest
{
    Q_OBJECT
    Q_STM_OBJECT_TEST(UT_Object)
public:
};

}
