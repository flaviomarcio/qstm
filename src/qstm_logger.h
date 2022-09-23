#pragma once

#include "./qstm_global.h"
#include "./qobject.h"


#define Q_STM_DECLARE_LOGGER(CLASS) Logger(#CLASS)

namespace QStm {

//!
//! \brief The Logger class
//!
struct Q_STM_EXPORT Logger
{
public:

    //!
    //! \brief Logger
    //! \param parent
    //!
    explicit Logger(const QByteArray &className);

    //!
    //! \brief trace
    //! \param format
    //! \return
    //!
    Logger &trace(QString format, ...);

    //!
    //! \brief debug
    //! \param format
    //! \return
    //!
    Logger &debug(QString format, ...);

    //!
    //! \brief info
    //! \param format
    //! \return
    //!
    Logger &info(QString format, ...);

    //!
    //! \brief warn
    //! \param format
    //! \return
    //!
    Logger &warn(QString format, ...);

    //!
    //! \brief error
    //! \param format
    //! \return
    //!
    Logger &error(QString format, ...);
private:
    QByteArray className;
};

} // namespace QStm

