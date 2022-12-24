cmake_minimum_required(VERSION 3.14)

project(QStm LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)


if(MSVC)
    add_compile_options(/W3 /WX /Zc:lambda)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

#include(${CMAKE_CURRENT_SOURCE_DIR}/src/src.cmake)
set(QStm_SRCS
    src/private/p_qstm_base_setting.cpp
    src/private/p_qstm_base_setting.h
    src/private/p_qstm_result_info.cpp
    src/private/p_qstm_result_info.h
    src/private/p_qstm_setting_manager.cpp
    src/private/p_qstm_setting_manager.h
    src/private/p_qstm_startup.cpp
    src/private/p_qstm_startup.h
    src/qstm.h
    src/qstm_cache_pool.cpp
    src/qstm_cache_pool.h
    src/qstm_const.h
    src/qstm_crypto_util.cpp
    src/qstm_crypto_util.h
    src/qstm_currency.cpp
    src/qstm_currency.h
    src/qstm_fileinfo.cpp
    src/qstm_fileinfo.h
    src/qstm_global.h
    src/qstm_log.cpp
    src/qstm_log.h
    src/qstm_logger.cpp
    src/qstm_logger.h
    src/qstm_macro.h
    src/qstm_message.cpp
    src/qstm_message.h
    src/qstm_meta_enum.h
    src/qstm_meta_types.cpp
    src/qstm_meta_types.h
    src/qstm_network_types.cpp
    src/qstm_network_types.h
    src/qstm_object.cpp
    src/qstm_object.h
    src/qstm_object_wrapper.cpp
    src/qstm_object_wrapper.h
    src/qstm_result.cpp
    src/qstm_result.h
    src/qstm_result_info.cpp
    src/qstm_result_info.h
    src/qstm_setting_base.cpp
    src/qstm_setting_base.h
    src/qstm_setting_manager.cpp
    src/qstm_setting_manager.h
    src/qstm_startup.cpp
    src/qstm_startup.h
    src/qstm_types.cpp
    src/qstm_types.h
    src/qstm_types_const.h
    src/qstm_url.cpp
    src/qstm_url.h
    src/qstm_util_date.cpp
    src/qstm_util_date.h
    src/qstm_util_double.cpp
    src/qstm_util_double.h
    src/qstm_util_formatting.cpp
    src/qstm_util_formatting.h
    src/qstm_util_meta_object.cpp
    src/qstm_util_meta_object.h
    src/qstm_util_path.cpp
    src/qstm_util_path.h
    src/qstm_util_variant.cpp
    src/qstm_util_variant.h
    src/qstm_vvm.cpp
    src/qstm_vvm.h
)


if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    add_library(QStm STATIC ${QStm_SRCS})
else()
    add_library(QStm SHARED ${QStm_SRCS})
endif()


target_link_libraries(QStm PRIVATE Qt${QT_VERSION_MAJOR}::Core)
#target_link_libraries(QStm PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(QStm PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(QStm PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_compile_definitions(QStm PRIVATE QStm_LIBRARY)

target_link_libraries(
    QStm # cmake-format: sortable
)

target_include_directories(QStm PUBLIC src)
target_include_directories(QStm PRIVATE src/private)

install(TARGETS QStm
    DESTINATION ${HOME}/qtreforce.sdk/lib
)
