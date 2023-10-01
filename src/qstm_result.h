#pragma once

#include <QVariant>
#include <QUuid>
#include <QUrl>
#include <QChar>
#include <QBitArray>
#include <QByteArray>
#include <QString>
#include <QVariantHash>
#include <QVariantMap>
#include <QStringList>
#include <QVariantList>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include "./qstm_global.h"
#include "./qstm_result_info.h"

#define __QSTM_DECLARE_RESULT_READ_OPERATOR(TYPE) \
    ResultValue &operator = (const TYPE value){ return this->setResult(value);}\
    ResultValue &operator << (const TYPE value){ return this->setResult(value);}\
    ResultValue &operator += (const TYPE value){ return this->setResult(value);}

#define __QSTM_DECLARE_RESULT_READ_LR(TYPE) ResultValue &lr(const TYPE value)const{ return this->lr().setResult(value);}

class ResultValuePvt;
//!
//! \brief The ResultValue class
//!class for carrying status and varying values
class Q_STM_EXPORT ResultValue : public QObject
{
    Q_OBJECT

    friend class QStmThreadReturnItem;

    Q_PROPERTY(QVariantHash data READ data NOTIFY dataChanged)
public:

    //!
    //! \brief ResultValue
    //! \param parent
    //!
    //! constructor
    Q_INVOKABLE explicit ResultValue(QObject *parent = nullptr);

    //!
    //! \brief operator ==
    //! \param value
    //! \return
    //!
    bool operator==(const ResultValue &value);

    //!
    //! \brief operator !=
    //! \param value
    //! \return
    //!
    bool operator!=(const ResultValue &value);

    //!
    //! \brief operator bool
    //!
    operator bool() const;

    //!
    //! \brief operator =
    //! \param value
    //! \return
    //!
    __QSTM_DECLARE_RESULT_READ_OPERATOR(ResultValue&)
    __QSTM_DECLARE_RESULT_READ_OPERATOR(QVariant &)

    //!
    //! \brief setResult
    //! \param value
    //! \return
    //!
    ResultValue &setResult(const ResultValue &value)const;
    ResultValue &setResult(const QVariant &value)const;

    //!
    //! \brief resultInfo
    //! \return
    //!
    QStm::ResultInfo &resultInfo()const;

    //!
    //! \brief printInfo
    //! \param v
    //! \return
    //!
    ResultValue &printInfo(const QVariant &v = {});

    //!
    //! \brief printWarning
    //! \param v
    //! \return
    //!
    ResultValue &printWarning(const QVariant &v = {});

    //!
    //! \brief resultVariant
    //! \return
    //!
    QVariant &resultVariant() const;

    //!
    //! \brief resultVariantInfo
    //! \return
    //!
    Q_INVOKABLE QVariant resultVariantInfo() const;

    //!
    //! \brief resultHash
    //! \return
    //!
    QVariantHash resultHash() const;

    //!
    //! \brief resultList
    //! \return
    //!
    QVariantList resultList() const;

    //!
    //! \brief resultStringList
    //! \return
    //!
    QStringList resultStringList() const;

    //!
    //! \brief resultToList
    //! \return
    //!
    QVariantList resultToList() const;

    //!
    //! \brief resultString
    //! \return
    //!
    QString resultString() const;

    //!
    //! \brief resultByteArray
    //! \return
    //!
    QByteArray resultByteArray() const;

    //!
    //! \brief resultUuid
    //! \return
    //!
    QUuid resultUuid() const;

    //!
    //! \brief resultInt
    //! \return
    //!
    qlonglong resultInt() const;

    //!
    //! \brief resultLongLong
    //! \return
    //!
    qlonglong resultLongLong() const;

    //!
    //! \brief resultDouble
    //! \return
    //!
    double resultDouble() const;

    //!
    //! \brief resultDate
    //! \return
    //!
    QDate resultDate() const;

    //!
    //! \brief resultDateTime
    //! \return
    //!
    QDateTime resultDateTime() const;

    //!
    //! \brief resultTime
    //! \return
    //!
    QTime resultTime() const;

    //!
    //! \brief resultUrl
    //! \return
    //!
    QUrl resultUrl() const;

    //!
    //! \brief resultObject
    //! \return
    //!
    template<class T=QObject>
    T *resultObject()
    {
        return resultVariant().value<T*>();
    }

    //!
    //! \brief clear
    //! \return
    //!
    ResultValue &clear();

    //!
    //! \brief clearScope
    //! \return
    //!
    ResultValue &clearScope();

    //!
    //! \brief toMap
    //! \return
    //!
    const QVariantMap &toMap();

    //!
    //! \brief toHash
    //! \return
    //!
    const QVariantHash &toHash();

    //!
    //! \brief returnHash
    //! \return
    //!
    QByteArray &returnHash() const;

    //!
    //! \brief returnType
    //! \return
    //!
    QStm::ResultInfo::MessageType returnType() const;

    //!
    //! \brief returnCode
    //! \return
    //!
    QByteArray &returnCode() const;

    //!
    //! \brief returnText
    //! \return
    //!
    QString &returnText() const;

    //!
    //! \brief setMsg
    //! \param value
    //! \return
    //!
    ResultValue &setMsg(const QVariant &value);

    //!
    //! \brief setInformation
    //! \param value
    //! \return
    //!
    ResultValue &setInformation(const QVariant &value);

    //!
    //! \brief setWarning
    //! \return
    //!
    ResultValue &setWarning();

    //!
    //! \brief setWarning
    //! \param value
    //! \return
    //!
    ResultValue &setWarning(const QVariant &value);

    //!
    //! \brief setValidation
    //! \return
    //!
    ResultValue &setValidation();

    //!
    //! \brief setValidation
    //! \param value
    //! \return
    //!
    ResultValue &setValidation(const QVariant &value);

    //!
    //! \brief setValidation
    //! \param lr
    //! \return
    //!
    ResultValue &setValidation(const ResultValue &lr);

    //!
    //! \brief setResponse
    //! \param value
    //! \return
    //!
    ResultValue &setResponse(const QVariant &value);

    //!
    //! \brief setUnauthorized
    //! \return
    //!
    ResultValue &setUnauthorized();

    //!
    //! \brief setUnauthorized
    //! \param value
    //! \return
    //!
    ResultValue &setUnauthorized(const QVariant &value);

    //!
    //! \brief setUnauthorized
    //! \param lr
    //! \return
    //!
    ResultValue &setUnauthorized(const ResultValue &lr);

    //!
    //! \brief setNotImplemented
    //! \return
    //!
    ResultValue &setNotImplemented();

    //!
    //! \brief setNotImplemented
    //! \param value
    //! \return
    //!
    ResultValue &setNotImplemented(const QVariant &value);

    //!
    //! \brief setNotImplemented
    //! \param lr
    //! \return
    //!
    ResultValue &setNotImplemented(const ResultValue &lr);

    //!
    //! \brief setBadRequest
    //! \return
    //!
    ResultValue &setBadRequest();

    //!
    //! \brief setBadRequest
    //! \param value
    //! \return
    //!
    ResultValue &setBadRequest(const QVariant &value);

    //!
    //! \brief setBadRequest
    //! \param lr
    //! \return
    //!
    ResultValue &setBadRequest(const ResultValue &lr);

    //!
    //! \brief setNoContent
    //! \return
    //!
    ResultValue &setNoContent();

    //!
    //! \brief setNoContent
    //! \param value
    //! \return
    //!
    ResultValue &setNoContent(const QVariant &value);

    //!
    //! \brief setNotFound
    //! \return
    //!
    ResultValue &setNotFound();

    //!
    //! \brief setNotFound
    //! \param value
    //! \return
    //!
    ResultValue &setNotFound(const QVariant &value);

    //!
    //! \brief setNotFound
    //! \param lr
    //! \return
    //!
    ResultValue &setNotFound(const ResultValue &lr);

    //!
    //! \brief setUnsupportedMediaType
    //! \return
    //!
    ResultValue &setUnsupportedMediaType();

    //!
    //! \brief setUnsupportedMediaType
    //! \param value
    //! \return
    //!
    ResultValue &setUnsupportedMediaType(const QVariant &value);

    //!
    //! \brief setUnsupportedMediaType
    //! \param lr
    //! \return
    //!
    ResultValue &setUnsupportedMediaType(const ResultValue &lr);

    //!
    //! \brief setCritical
    //! \return
    //!
    ResultValue &setCritical();

    //!
    //! \brief setCritical
    //! \param value
    //! \return
    //!
    ResultValue &setCritical(const QString &value);

    //!
    //! \brief setCritical
    //! \param value
    //! \return
    //!
    ResultValue &setCritical(const QVariantHash &value);

    //!
    //! \brief setCritical
    //! \param value
    //! \return
    //!
    ResultValue &setCritical(const QVariantList &value);

    //!
    //! \brief setCritical
    //! \param lr
    //! \return
    //!
    ResultValue &setCritical(const ResultValue &lr);

    //!
    //! \brief setCritical
    //! \param code
    //! \param message
    //! \return
    //!
    ResultValue &setCritical(const QVariant &code, const QVariant &message);

    //!
    //! \brief isOk
    //! \return
    //!
    bool isOk() const;

    //!
    //! \brief isNotOk
    //! \return
    //!
    bool isNotOk() const;

    //!
    //! \brief isInformation
    //! \return
    //!
    bool isInformation() const;

    //!
    //! \brief isWarning
    //! \return
    //!
    bool isWarning() const;

    //!
    //! \brief isValidation
    //! \return
    //!
    bool isValidation() const;

    //!
    //! \brief isCritical
    //! \return
    //!
    bool isCritical() const;

    //!
    //! \brief isNoContent
    //! \return
    //!
    bool isNoContent() const;

    //!
    //! \brief isUnauthorized
    //! \return
    //!
    bool isUnauthorized() const;

    //!
    //! \brief isNotfound
    //! \return
    //!
    bool isNotfound() const;

    //!
    //! \brief isBadRequest
    //! \return
    //!
    bool isBadRequest() const;

    //!
    //! \brief data
    //! \return
    //!
    QVariantHash data() const;

    //!
    //! \brief stateCode
    //! \return
    //!
    Q_INVOKABLE QVariant stateCode() const;

    //!
    //! \brief sc
    //! \return
    //!
    Q_INVOKABLE QVariant sc() const;

    //!
    //! \brief setCode
    //! \param statusCode
    //! \param reasonPhrase
    //! \return
    //!
    ResultValue &setCode(const QVariant &statusCode, const QVariant &reasonPhrase);

    //!
    //! \brief toString
    //! \return
    //!
    Q_INVOKABLE QString toString() const;
public:
    ResultValuePvt *p = nullptr;
signals:
    void dataChanged();
};
