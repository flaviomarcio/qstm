#pragma once

#include <QObject>
#include "./qstm_global.h"

namespace QStm {

//!
//! \brief The Log class
//!
class Q_STM_EXPORT Log : public QObject
{
    Q_OBJECT
public:

    //!
    //! \brief Log
    //! \param parent
    //!
    Q_INVOKABLE explicit Log(QObject *parent = nullptr);

    //!
    //! \brief qLogEnabled
    //! \return
    //!
    //! it is loaded using env Q_LOG_ENABLED
    static bool qLogEnabled();

    //!
    //! \brief enabled
    //!
    static void enabled();

    //!
    //! \brief disabled
    //!
    static void disabled();
};

}

