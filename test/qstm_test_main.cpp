#include "qstm_test_main.h"
#include <QTest>

#include "./qstm_test_main.h"
#include "./qstm_unit_test_date_util.h"
#include "./qstm_unit_test_double_util.h"
#include "./qstm_unit_test_envs.h"
#include "./qstm_unit_test_formatting_util.h"
#include "./qstm_unit_test_object.h"
#include "./qstm_unit_test_result_value.h"
#include "./qstm_unit_test_setting.h"
#include "./qstm_unit_test_variant_util.h"

#define Q_STM_TEST_RUNNER(CLASS){CLASS var; var.execute();}

namespace QStm{

void TestMain::execute()
{
    Q_STM_TEST_RUNNER(UT_DateUtil);
    Q_STM_TEST_RUNNER(UT_DoubleUtil);
    Q_STM_TEST_RUNNER(UT_DoubleUtil);
    Q_STM_TEST_RUNNER(UT_Envs);
    Q_STM_TEST_RUNNER(UT_FormattingUtil);
    Q_STM_TEST_RUNNER(UT_Object);
    Q_STM_TEST_RUNNER(UT_ResultValue);
    Q_STM_TEST_RUNNER(UT_Setting);
    Q_STM_TEST_RUNNER(UT_VariantUtil);
}

}
QTEST_MAIN(QStm::TestMain)
#include "moc_qstm_test_main.cpp"


