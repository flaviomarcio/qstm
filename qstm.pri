QTREFORCE_QSTM=true
DEFINES+=QTREFORCE_QSTM

include($$PWD/3rdparty/bcmath/bcmath.pri)
include($$PWD/src/qstm.pri)

INCLUDEPATH += $$PWD/includes

HEADERS+= \
    $$PWD/includes/QStm
