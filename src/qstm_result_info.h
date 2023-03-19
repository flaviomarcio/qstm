#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QVariantHash>
#include "./qstm_object_wrapper.h"
#include "./qstm_types.h"

namespace QStm {
class ResultInfoPvt;
//!
//! \brief The ResultInfo class
//!
//! class page navigator
class Q_STM_EXPORT ResultInfo:public ObjectWrapper
{
    Q_OBJECT
    Q_STM_OBJECT_WRAPPER(ResultInfo)

    Q_STM_DECLARE_MESSAGE_TYPE

    Q_PROPERTY(QVariant code READ code WRITE setCode RESET resetCode NOTIFY codeChanged)
    Q_PROPERTY(QVariant messageType READ messageType WRITE setMessageType RESET resetMessageType NOTIFY messageTypeChanged)
    Q_PROPERTY(QVariant messages READ messages WRITE setMessages NOTIFY messagesChanged)
    Q_PROPERTY(bool success READ success WRITE setSuccess NOTIFY successChanged)
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(int perPage READ perPage WRITE setPerPage NOTIFY perPageChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int totalCount READ totalCount WRITE setTotalCount NOTIFY totalCountChanged)
    Q_PROPERTY(int totalPages READ totalPages WRITE setTotalPages NOTIFY totalPagesChanged)

public:
    //!
    //! \brief ResultInfo constructor
    //! \param parent
    //!
    Q_INVOKABLE explicit ResultInfo(QObject *parent=nullptr);

    //!
    //! \brief clear
    //!
    //!
    //! clear all values
    virtual void clear();

    //!
    //! \brief enabled
    //! \return
    //!
    virtual bool enabled();

    //!
    //! \brief setEnabled
    //! \param value
    //!
    virtual void setEnabled(bool value);

    //!
    //! \brief code
    //! \return
    //!
    virtual int code();
    ResultInfo &setCode(const QVariant &value);
    ResultInfo &resetCode();

    //!
    //! \brief messageType
    //! \return
    //!
    QStm::ResultInfo::MessageType messageType() const;
    void setMessageType(const QVariant &value);
    void resetMessageType();

    //!
    //! \brief messages
    //! \return
    //!
    //!
    //! return message to server
    virtual QStringList &messages();

    //!
    //! \brief setMessages
    //! \param value
    //!
    void setMessages(const QVariant &value);

    //!
    //! \brief success
    //! \return
    //!
    //! return success true ou false to request
    bool success() const;

    //!
    //! \brief setSuccess
    //! \param value
    //!
    void setSuccess(bool value);

    //!
    //! \brief page
    //! \return
    //!
    //! currenty page
    int page() const;

    //!
    //! \brief setPage
    //! \param value
    //!
    void setPage(int value);

    //!
    //! \brief perPage
    //! \return
    //!
    //! rows per page
    int perPage() const;

    //!
    //! \brief setPerPage
    //! \param value
    //!
    void setPerPage(int value);

    //!
    //! \brief count
    //! \return
    //!
    //! returneds row per page
    int count() const;

    //!
    //! \brief setCount
    //! \param value
    //!
    void setCount(int value);

    //!
    //! \brief totalCount
    //! \return
    //! total rows to all pages
    int totalCount() const;

    //!
    //! \brief setTotalCount
    //! \param value
    //!
    void setTotalCount(int value);

    //!
    //! \brief totalPages
    //! \return
    //! total pages
    int totalPages() const;
    void setTotalPages(int value);

    //!
    //! \brief toRequestHash
    //! \return
    //!
    virtual const QVariantHash toRequestHash() const;

    //!
    //! \brief toVar
    //! \return
    //!
    virtual QVariant toVar() const;

    //!
    //! \brief fromVar
    //! \param v
    //! \return
    //!
    virtual bool fromVar(const QVariant &v);

    //!
    //! \brief fromMap
    //! \param map
    //! \return
    //!
    virtual bool fromMap(const QVariantMap &map);

    //!
    //! \brief fromHash
    //! \param map
    //! \return
    //!
    virtual bool fromHash(const QVariantHash &map);

    //!
    //! \brief fromResultInfo
    //! \param map
    //! \return
    //!
    virtual bool fromResultInfo(const ResultInfo &resultInfo);

public:
    ResultInfoPvt *p=nullptr;
signals:
    void messagesChanged();
    void successChanged();
    void pageChanged();
    void perPageChanged();
    void countChanged();
    void totalCountChanged();
    void totalPagesChanged();
    void messageTypeChanged();
};

}
