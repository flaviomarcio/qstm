#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QVariantHash>
#include <QUuid>
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
    Q_STM_OBJECT_WRAPPER(ResultInfo)

    Q_PROPERTY(bool success READ success WRITE success NOTIFY successChanged)
    Q_PROPERTY(int code READ code WRITE code RESET resetCode NOTIFY codeChanged)
    Q_PROPERTY(int count READ count WRITE count RESET resetCount NOTIFY countChanged)
    Q_PROPERTY(int page READ page WRITE page NOTIFY pageChanged)
    Q_PROPERTY(int perPage READ perPage WRITE perPage RESET resetPerPage NOTIFY perPageChanged)
    Q_PROPERTY(int totalCount READ totalCount WRITE totalCount RESET resetTotalCount NOTIFY totalCountChanged)
    Q_PROPERTY(int totalPages READ totalPages NOTIFY totalPagesChanged)
    Q_PROPERTY(QVariant messageType READ messageType WRITE messageType RESET resetMessageType NOTIFY messageTypeChanged)
    Q_PROPERTY(QStringList messages READ messages WRITE messages NOTIFY messagesChanged)
    Q_PROPERTY(QUuid md5Counter READ md5Counter WRITE md5Counter RESET resetMd5Counter NOTIFY md5CounterChanged)

public:

    enum MessageType {
        None = 1,
        Information = 2,
        Warning = 4,
        Validation = 8,
        Critical = 16,
        Unauthorized = 32,
        Notfound = 64,
        BadRequest = 128,
        UnsupportedMediaType = 256,
        NoContent = 512,
        NotImplemented = 1024,
    };
    Q_ENUM(MessageType)

    //!
    //! \brief ResultInfo constructor
    //! \param parent
    //!
    Q_INVOKABLE explicit ResultInfo(QObject *parent=nullptr);

    //!
    //! \brief enabled
    //! \return
    //!
    virtual bool enabled() const;
    virtual ResultInfo &enabled(const QVariant &value);
    virtual ResultInfo &resetEnabled();

    //!
    //! \brief md5Counter
    //! \return
    //!
    virtual QUuid md5Counter();
    ResultInfo &md5Counter(const QVariant &value);
    ResultInfo &resetMd5Counter();

    //!
    //! \brief code
    //! \return
    //!
    virtual int code();
    ResultInfo &code(const QVariant &value);
    ResultInfo &resetCode();

    //!
    //! \brief messageType
    //! \return
    //!
    QStm::ResultInfo::MessageType messageType() const;
    ResultInfo &messageType(const QVariant &value);
    ResultInfo &resetMessageType();

    //!
    //! \brief messages
    //! \return
    //!
    //! return message to server
    virtual QStringList &messages();
    ResultInfo &messages(const QVariant &value);
    ResultInfo &resetMessages();
    virtual QString &messagesText();

    //!
    //! \brief success
    //! \return
    //!
    //! return success true ou false to request
    bool success() const;
    ResultInfo &success(const QVariant &value);
    ResultInfo &resetSuccess();

    //!
    //! \brief page
    //! \return
    //!
    //! currenty page
    int page() const;
    ResultInfo &page(const QVariant &value);
    ResultInfo &resetPage();

    //!
    //! \brief perPage
    //! \return
    //!
    //! rows per page
    int perPage() const;
    ResultInfo &perPage(const QVariant &value);
    ResultInfo &resetPerPage();

    //!
    //! \brief count
    //! \return
    //!
    //! returneds row per page
    int count() const;
    ResultInfo &count(const QVariant &value);
    ResultInfo &resetCount();

    //!
    //! \brief totalCount
    //! \return
    //! total rows to all pages
    int totalCount() const;
    ResultInfo &totalCount(const QVariant &value);
    ResultInfo &resetTotalCount();

    //!
    //! \brief totalPages
    //! \return
    //! total pages
    int totalPages() const;

    //!
    //! \brief offSetRecords
    //! \param offSetPages
    //! \return
    //!
    int offSetRecords(int offSetPages=-1) const;

public:
    ResultInfoPvt *p=nullptr;
signals:
    void codeChanged();
    void countChanged();
    void md5CounterChanged();
    void messagesChanged();
    void messageTypeChanged();
    void pageChanged();
    void perPageChanged();
    void successChanged();
    void totalCountChanged();
    void totalPagesChanged();
};

}
