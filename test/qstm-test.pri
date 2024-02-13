CONFIG += console
CONFIG -= debug_and_release
CONFIG += testcase
QT += testlib

LC_ALL=en_US.UTF-8

include($$PWD/../qstm.pri)

Q_STM_TEST=true
QMAKE_CXXFLAGS += -DQ_STM_TEST=\\\"$$Q_STM_TEST\\\"

INCLUDEPATH+=$$PWD/src

HEADERS += \
    $$PWD/qstm_test.h \
    $$PWD/qstm_test_main.h \
    $$PWD/qstm_unit_test_date_util.h \
    $$PWD/qstm_unit_test_double_util.h \
    $$PWD/qstm_unit_test_envs.h \
    $$PWD/qstm_unit_test_formatting_util.h \
    $$PWD/qstm_unit_test_hash_util.h \
    $$PWD/qstm_unit_test_meta_items.h \
    $$PWD/qstm_unit_test_object.h \
    $$PWD/qstm_unit_test_result_value.h \
    $$PWD/qstm_unit_test_setting.h \
    $$PWD/qstm_unit_test_variant_util.h \

SOURCES += \
    $$PWD/qstm_test.cpp \
    $$PWD/qstm_test_main.cpp \
