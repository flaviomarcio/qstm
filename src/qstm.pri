QT += core
QT += network
QT += websockets
QT += sql

#INCLUDEPATH+=$$PWD

HEADERS += \
    $$PWD/private/p_qstm_base_setting.h \
    $$PWD/private/p_qstm_setting_manager.h \
    $$PWD/private/p_qstm_startup.h \
    $$PWD/qstm.h \
    $$PWD/qstm_cache_io.h \
    $$PWD/qstm_cache_pool.h \
    $$PWD/qstm_const.h \
    $$PWD/qstm_crypto_util.h \
    $$PWD/qstm_currency.h \
    $$PWD/qstm_envs.h \
    $$PWD/qstm_fileinfo.h \
    $$PWD/qstm_global.h \
    $$PWD/qstm_log.h \
    $$PWD/qstm_logger.h \
    $$PWD/qstm_macro.h \
    $$PWD/qstm_message.h \
    $$PWD/qstm_meta_enum.h \
    $$PWD/qstm_meta_types.h \
    $$PWD/qstm_network_types.h \
    $$PWD/qstm_object.h \
    $$PWD/qstm_object_wrapper.h \
    $$PWD/qstm_result.h \
    $$PWD/qstm_result_info.h \
    $$PWD/qstm_setting_base.h \
    $$PWD/qstm_setting_file.h \
    $$PWD/qstm_setting_manager.h \
    $$PWD/qstm_startup.h \
    $$PWD/qstm_types.h \
    $$PWD/qstm_types_const.h \
    $$PWD/qstm_url.h \
    $$PWD/qstm_util_date.h \
    $$PWD/qstm_util_double.h \
    $$PWD/qstm_util_formatting.h \
    $$PWD/qstm_util_meta_object.h \
    $$PWD/qstm_util_path.h \
    $$PWD/qstm_util_variant.h \
    $$PWD/qstm_vvm.h \

SOURCES += \
    $$PWD/private/p_qstm_base_setting.cpp \
    $$PWD/private/p_qstm_setting_manager.cpp \
    $$PWD/private/p_qstm_startup.cpp \
    $$PWD/qstm_cache_io.cpp \
    $$PWD/qstm_cache_pool.cpp \
    $$PWD/qstm_crypto_util.cpp \
    $$PWD/qstm_currency.cpp \
    $$PWD/qstm_envs.cpp \
    $$PWD/qstm_fileinfo.cpp \
    $$PWD/qstm_log.cpp \
    $$PWD/qstm_logger.cpp \
    $$PWD/qstm_message.cpp \
    $$PWD/qstm_meta_types.cpp \
    $$PWD/qstm_network_types.cpp \
    $$PWD/qstm_object.cpp \
    $$PWD/qstm_object_wrapper.cpp \
    $$PWD/qstm_result.cpp \
    $$PWD/qstm_result_info.cpp \
    $$PWD/qstm_setting_base.cpp \
    $$PWD/qstm_setting_file.cpp \
    $$PWD/qstm_setting_manager.cpp \
    $$PWD/qstm_startup.cpp \
    $$PWD/qstm_types.cpp \
    $$PWD/qstm_url.cpp \
    $$PWD/qstm_util_date.cpp \
    $$PWD/qstm_util_double.cpp \
    $$PWD/qstm_util_formatting.cpp \
    $$PWD/qstm_util_meta_object.cpp \
    $$PWD/qstm_util_path.cpp \
    $$PWD/qstm_util_variant.cpp \
    $$PWD/qstm_vvm.cpp \

