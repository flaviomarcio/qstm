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
#include "./qstm_types.h"

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

    Q_STM_DECLARE_MESSAGE_TYPE

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
    __QSTM_DECLARE_RESULT_READ_OPERATOR(QVariant&)

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
    virtual QStm::ResultInfo &resultInfo()const;

    //!
    //! \brief printInfo
    //! \param v
    //! \return
    //!
    virtual ResultValue &printInfo(const QVariant &v = {});

    //!
    //! \brief printWarning
    //! \param v
    //! \return
    //!
    virtual ResultValue &printWarning(const QVariant &v = {});

    //!
    //! \brief resultVariant
    //! \return
    //!
    virtual QVariant &resultVariant() const;

    //!
    //! \brief resultVariantInfo
    //! \return
    //!
    Q_INVOKABLE virtual QVariant resultVariantInfo() const;

    //!
    //! \brief resultMap
    //! \return
    //!
    virtual QVariantMap resultMap() const;

    //!
    //! \brief resultHash
    //! \return
    //!
    virtual QVariantHash resultHash() const;

    //!
    //! \brief resultList
    //! \return
    //!
    virtual QVariantList resultList() const;

    //!
    //! \brief resultStringList
    //! \return
    //!
    virtual QStringList resultStringList() const;

    //!
    //! \brief resultToList
    //! \return
    //!
    virtual QVariantList resultToList() const;

    //!
    //! \brief resultString
    //! \return
    //!
    virtual QString resultString() const;

    //!
    //! \brief resultByteArray
    //! \return
    //!
    virtual QByteArray resultByteArray() const;

    //!
    //! \brief resultUuid
    //! \return
    //!
    virtual QUuid resultUuid() const;

    //!
    //! \brief resultInt
    //! \return
    //!
    virtual qlonglong resultInt() const;

    //!
    //! \brief resultLongLong
    //! \return
    //!
    virtual qlonglong resultLongLong() const;

    //!
    //! \brief resultDouble
    //! \return
    //!
    virtual double resultDouble() const;

    //!
    //! \brief resultDate
    //! \return
    //!
    virtual QDate resultDate() const;

    //!
    //! \brief resultDateTime
    //! \return
    //!
    virtual QDateTime resultDateTime() const;

    //!
    //! \brief resultTime
    //! \return
    //!
    virtual QTime resultTime() const;

    //!
    //! \brief resultUrl
    //! \return
    //!
    virtual QUrl resultUrl() const;

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
    virtual ResultValue &clear();

    //!
    //! \brief clearScope
    //! \return
    //!
    virtual ResultValue &clearScope();

    //!
    //! \brief toMap
    //! \return
    //!
    virtual const QVariantMap &toMap();

    //!
    //! \brief toHash
    //! \return
    //!
    virtual const QVariantHash &toHash();

    //!
    //! \brief returnHash
    //! \return
    //!
    virtual QByteArray &returnHash() const;

    //!
    //! \brief returnType
    //! \return
    //!
    virtual MessageType returnType() const;

    //!
    //! \brief returnCode
    //! \return
    //!
    virtual QByteArray &returnCode() const;

    //!
    //! \brief returnText
    //! \return
    //!
    virtual QString &returnText() const;

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
    virtual bool isOk() const;

    //!
    //! \brief isNotOk
    //! \return
    //!
    virtual bool isNotOk() const;

    //!
    //! \brief isInformation
    //! \return
    //!
    virtual bool isInformation() const;

    //!
    //! \brief isWarning
    //! \return
    //!
    virtual bool isWarning() const;

    //!
    //! \brief isValidation
    //! \return
    //!
    virtual bool isValidation() const;

    //!
    //! \brief isCritical
    //! \return
    //!
    virtual bool isCritical() const;

    //!
    //! \brief isNoContent
    //! \return
    //!
    virtual bool isNoContent() const;

    //!
    //! \brief isUnauthorized
    //! \return
    //!
    virtual bool isUnauthorized() const;

    //!
    //! \brief isNotfound
    //! \return
    //!
    virtual bool isNotfound() const;

    //!
    //! \brief isBadRequest
    //! \return
    //!
    virtual bool isBadRequest() const;

    //!
    //! \brief data
    //! \return
    //!
    virtual QVariantHash data() const;

    //!
    //! \brief stateCode
    //! \return
    //!
    Q_INVOKABLE virtual QVariant stateCode() const;

    //!
    //! \brief sc
    //! \return
    //!
    Q_INVOKABLE virtual QVariant sc() const;

    //!
    //! \brief setCode
    //! \param statusCode
    //! \param reasonPhrase
    //! \return
    //!
    virtual ResultValue &setCode(const QVariant &statusCode, const QVariant &reasonPhrase);

    //!
    //! \brief toString
    //! \return
    //!
    Q_INVOKABLE virtual QString toString() const;
public:
    ResultValuePvt *p = nullptr;
signals:
    void dataChanged();
};
