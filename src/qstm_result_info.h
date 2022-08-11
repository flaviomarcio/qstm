#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QVariantHash>
#include "./qstm_object_wrapper.h"

namespace QStm {
class ResultInfoPvt;
//!
//! \brief The ResultInfo class
//!
//! class page navigator
class Q_STM_EXPORT ResultInfo:public ObjectWrapper
{
    Q_OBJECT

    Q_PROPERTY(QVariantList errors READ errors WRITE setErrors NOTIFY errorsChanged)
    Q_PROPERTY(QVariantList messages READ messages WRITE setMessages NOTIFY messagesChanged)
    Q_PROPERTY(bool success READ success WRITE setSuccess NOTIFY successChanged)
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(int per_page READ perPage WRITE setPerPage NOTIFY perPageChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int total_count READ totalCount WRITE setTotalCount NOTIFY totalCountChanged)
    Q_PROPERTY(int total_pages READ totalPages WRITE setTotalPages NOTIFY totalPagesChanged)

public:
    //!
    //! \brief ResultInfo constructor
    //! \param parent
    //!
    Q_INVOKABLE explicit ResultInfo(QObject *parent=nullptr);

    //! \brief ResultInfo destructor
    //!
    ~ResultInfo();

    //!
    //! \brief clear
    //!
    //!
    //! clear all values
    virtual void clear();

    //!
    //! \brief clearErrors
    //!
    //!
    //! clear all errors info
    virtual void clearErrors();


    //!
    //! \brief errors
    //! \return
    //! activity resource
    virtual bool enabled();

    //!
    //! \brief setEnabled
    //! \param value
    //!
    virtual void setEnabled(bool value);

    //!
    //! \brief errors
    //! \return
    //! return errors to server
    virtual QVariantList &errors();

    //!
    //! \brief setErrors
    //! \param value
    //!
    virtual void setErrors(const QVariantList &value);

    //!
    //! \brief messages
    //! \return
    //!
    //!
    //! return message to server
    virtual QVariantList &messages();

    //!
    //! \brief setMessages
    //! \param value
    //!
    void setMessages(const QVariantList &value);

    //!
    //! \brief success
    //! \return
    //!
    //! return success true ou false to request
    int success() const;

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

//    //!
//    //! \brief toMap
//    //! \return
//    //!
//    virtual QVariantMap toMap() const;

//    //!
//    //! \brief toHash
//    //! \return
//    //!
//    virtual QVariantHash toHash() const;

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

private:
    ResultInfoPvt *p=nullptr;
signals:
    void errorsChanged();
    void messagesChanged();
    void successChanged();
    void pageChanged();
    void perPageChanged();
    void countChanged();
    void totalCountChanged();
    void totalPagesChanged();
};

}
