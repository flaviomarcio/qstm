#include "./qstm_result.h"
#include "./qstm_macro.h"
#include "./qstm_object.h"
#include "./qstm_util_meta_object.h"
#include "./qstm_util_variant.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QMutex>
#include <QThread>

using namespace QStm;

class ResultValuePvt;
//struct QStmConstsResult
//{
//public:
//    QHash<QThread *, QByteArray> staticThreadError;
//    QMutex staticThreadLocker;
//};


struct QStmThreadResultItem{
public:
    ResultValue::MessageType returnType = ResultValue::None;
    QVariant returnCode;
    QByteArray returnHash;
    QString returnText;
    QVariant resultVariant;

    void operator=(const QStmThreadResultItem&item)
    {
        this->returnCode=item.returnCode;
        this->returnHash=item.returnHash;
        this->returnText=item.returnText;
        this->resultVariant=item.resultVariant;
    }


    void clear()
    {
        this->returnType = ResultValue::None;
        this->returnHash = {};
        this->returnCode = {};
        this->returnText.clear();
        this->resultVariant = {};
    }

    bool isOk()
    {
        if(this->returnType==ResultValue::Information || this->returnType==ResultValue::None)
            return true;

        if (this->returnCode.isValid())
            return false;

        if (this->returnCode.isValid())
            return false;

        return this->returnHash.trimmed().isEmpty();
    }

    QString toString() const
    {
        if (this->returnCode.isValid()) {
            if (this->returnCode.toString() == this->returnText)
                return this->returnText;
            return this->returnCode.toString() + QStringLiteral(":") + this->returnText;
        }
        return this->returnText;
    }
};

struct QStmResults{
public:
    QMutex mutex;
    QHash<QThread*, QStmThreadResultItem> results;

    bool insert(QStmThreadResultItem &newItem)
    {
        QMutexLocker<QMutex> locker(&mutex);
        if (newItem.returnHash.trimmed().isEmpty())
            return {};

        auto&item=results[QThread::currentThread()];
        if(item.returnHash==newItem.returnHash){
            newItem=item;
            return false;
        }
        item=newItem;
        return true;
    }

    void clear()
    {
        QMutexLocker<QMutex> locker(&mutex);
        auto thread=QThread::currentThread();
        if(results.contains(thread))
            results.remove(thread);
    }
};

typedef QHash<ResultValue::MessageType, int> MakeArStats;
typedef QVector<int> StatusCodeSuccessList;

//Q_GLOBAL_STATIC(QStmConstsResult, staticConsts)
Q_GLOBAL_STATIC(QStmResults, staticResults)
Q_GLOBAL_STATIC_WITH_ARGS(StatusCodeSuccessList, statusCodeSuccessList, ({0, 200, 201, 202}))
Q_GLOBAL_STATIC_WITH_ARGS(MakeArStats, staticMakeArStats, ({{ResultValue::None, 0},
                                                      {ResultValue::Information, 0},
                                                      {ResultValue::Warning, 406},
                                                      {ResultValue::Validation, 406},
                                                      {ResultValue::Notfound, 404},
                                                      {ResultValue::UnsupportedMediaType, 415},
                                                      {ResultValue::Critical, 500},
                                                      {ResultValue::Unauthorized, 401},
                                                      {ResultValue::NoContent, 204},
                                                      {ResultValue::BadRequest, 400}}))

class ResultValuePvt
{
public:
    QUuid uuid = QUuid::createUuid();
    bool resultBool = true;
    QStm::ResultInfo resultInfo;
    ResultValue *parent = nullptr;
    QVariantHash data;
    QStmThreadResultItem resultItem;

    explicit ResultValuePvt(ResultValue *parent)
    {
        this->parent = parent;
    }
    virtual ~ResultValuePvt()
    {
    }

    ResultValue &setMessage(ResultValue::MessageType msgType, const QVariant &code, const QVariant &message)
    {
        this->clear();
        if (message.isValid()) {
            resultItem.returnType = msgType;
            resultItem.returnCode = this->variantConvertToCode(code.isValid()?code:message);
            resultItem.returnText = this->variantConvertToText(message);
            this->makeResult();
        }
        return *parent;
    }

    ResultValue &setResult(const ResultValue &value)
    {
        this->readFull(value.p);
        return *this->parent;
    }

    ResultValue &setResult(const QVariant &value)
    {
        this->clearReturn();

        switch (value.typeId()) {
        case QMetaType::Bool:
            this->resultBool = value.toBool();
            break;
        case QMetaType::QObjectStar:
        {
            {
                ResultValue *object=value.value<ResultValue*>();
                if(object){
                    this->readFull(object->p);
                    break;
                }
            }

            {
                QObject *object=value.value<QObject*>();
                if(object){
                    MetaObjectUtil util;
                    resultItem.resultVariant=util.toHash(object);
                    break;
                }
            }
            break;
        }
        default:
            resultItem.resultVariant=value;
        };
        return *parent;
    }

    ResultValue &setCritical(const QVariant &code, const QVariant &value)
    {
        auto v = value.toString().trimmed().isEmpty() ? QVariant{} : value.toString();
        this->clear();
        if (!code.isValid() &&  !v.isValid())
            return *this->parent;

        resultItem.returnType = ResultValue::Critical;
        resultItem.returnCode = this->variantConvertToCode(code);
        resultItem.returnText = this->variantConvertToText(v);
        if (!resultItem.returnCode.isValid())
            resultItem.returnCode = this->toMd5(resultItem.returnText);
        this->makeResult();
        return *this->parent;
    }

    void makeResult()
    {
        if (this->isOk()) {
            this->resultInfo.clearErrors();
            resultInfo.setSuccess(true);
            auto &msgs=resultInfo.messages();
            msgs.clear();
            msgs.append(this->parent->returnText());
            return;
        }
        this->data = this->toHash();
        resultItem.returnHash = this->toMd5(this->variantConvertToCode(data));


        if(!staticResults->insert(resultItem))
            return;

        if(!this->parent->returnText().isEmpty()){
            resultInfo.setSuccess(false);
            auto &msgs=resultInfo.errors();
            msgs.clear();
            msgs.append(this->parent->returnText());
        }

        QList<ResultValue *> listParentResultValue;
        auto pp = dynamic_cast<QStm::Object *>(this->parent->parent());

        if(pp && pp==this->parent->parent())
            pp=dynamic_cast<QStm::Object *>(pp->parent());

        while (pp) {
            const auto&lr=&pp->lr();
            listParentResultValue.insert(0, lr);
            pp = dynamic_cast<QStm::Object *>(pp->parent());
        }
        sWarning() << this->resultItem.toString();
        for(int i=listParentResultValue.count()-1; i>0;i--){
            parent=listParentResultValue.at(i);
            if(parent->parent())
                sWarning() << parent->parent()->metaObject()->className();
            parent->p->read(this);
        }
    }

    bool isOk()
    {
        if (QThread::currentThread()->isInterruptionRequested())
            return false;

        return this->resultItem.isOk();
    }

    const QVariantHash &toHash()
    {
        this->data.clear();
        this->data.insert(QByteArrayLiteral("resultBool"), this->resultBool);
        data.insert(QByteArrayLiteral("returnType"), this->resultItem.returnType);
        if (this->resultItem.returnCode.isValid())
            data.insert(QByteArrayLiteral("returnCode"), this->resultItem.returnCode);
        if (!this->resultItem.returnHash.isEmpty())
            data.insert(QByteArrayLiteral("returnHash"), this->resultItem.returnHash);
        if (!this->resultItem.returnText.isEmpty())
            data.insert(QByteArrayLiteral("returnText"), this->resultItem.returnText);
        if (this->resultItem.resultVariant.isValid())
            data.insert(QByteArrayLiteral("resultVariant"), this->resultItem.resultVariant);
        if (this->resultItem.resultVariant.isValid())
            data.insert(QByteArrayLiteral("resultInfo"), this->resultInfo.toHash());
        return data;
    }

    void clear()
    {
        this->data.clear();
        this->resultBool = true;
        this->resultItem.clear();

        this->resultInfo.clear();
        staticResults->clear();
    }

    void clearReturn()
    {
        this->data.clear();
        this->resultInfo.clearErrors();
        this->resultBool = true;
    }

    void read(ResultValuePvt *pvt)
    {
        if (pvt == nullptr) {
            this->clear();
            return;
        }

        this->data = this->toHash();
        this->resultBool = pvt->resultBool;
        this->resultItem = pvt->resultItem;
        this->resultInfo.fromHash(pvt->resultInfo.toHash());
    }

    void readFull(const ResultValuePvt *pvt)
    {
        if (pvt == nullptr) {
            this->clear();
            return;
        }

        auto aux = (!this->isOk()) ? false : this->resultBool;
        this->data = pvt->data;
        this->resultItem = pvt->resultItem;
        this->resultBool = pvt->resultBool;
        this->resultBool = (aux && this->resultBool) ? aux : false;
        this->resultInfo.fromHash(pvt->resultInfo.toHash());
    }

    static QByteArray toMd5(const QVariant &v)
    {
        return QCryptographicHash::hash(v.toByteArray(), QCryptographicHash::Md5).toHex();
    }

    static QVariant variantConvertToCode(const QVariant &value)
    {
        if (!value.isValid())
            return {};

        auto typeId=value.typeId();
        switch (typeId) {
        case QMetaType::Double:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            return value.toByteArray();
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QChar:
        case QMetaType::QBitArray:
        {
            auto code = value.toByteArray().trimmed();
            if ((code.isEmpty()))
                return {};
            return code; //QCryptographicHash::hash(code, QCryptographicHash::Md5).toHex();
        }
        default:
            break;
        }

        QStringList list;
        switch (typeId) {
        case QMetaType::QStringList:
            list = value.toStringList();
            break;
        case QMetaType::QVariantList:
        {
            for (auto &v : value.toList())
                list.append(v.toByteArray());
            break;
        }
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        {
            Q_DECLARE_VU;
            auto vHash = vu.toHash(value);
            QStringList list;
            Q_V_HASH_ITERATOR(vHash){
                i.next();
                list.append(i.key());
                list.append(QByteArrayLiteral("."));
                list.append(i.value().toByteArray());
            }
            break;
        }
        default:
            break;
        }
        return QCryptographicHash::hash(list.join('.').toUtf8(), QCryptographicHash::Md5).toHex();
    }

    static QString variantConvertToText(const QVariant &value)
    {
        if (!value.isValid())
            return {};

        auto typeId=value.typeId();

        QStringList list;
        switch (typeId) {
        case QMetaType::Double:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            return QStringLiteral("%1: %2").arg(QObject::tr("Error value"), value.toString());
        case QMetaType::QStringList:
            list = value.toStringList();
            break;
        case QMetaType::QVariantList:
        {
            for (auto &v : value.toList())
                list.append(v.toByteArray());
            break;
        }
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
            {
                Q_V_HASH_ITERATOR(value.toHash()){
                    i.next();
                    list.append(QStringLiteral("%1==%2").arg(i.key(), i.value().toByteArray()));
                }
                break;
            }
        default:
            list.append(value.toString().trimmed());
            break;
        }
        return list.join(QByteArrayLiteral("\n")).toUtf8();
    }
};

ResultValue::ResultValue(QObject *parent) : QObject{parent}
{
    this->p = new ResultValuePvt{this};
}

ResultValue::~ResultValue()
{
    delete p;
}

ResultValue::operator bool() const
{
    auto __isOK = p->isOk();
    bool __return = (p->resultBool & p->isOk());
    p->resultBool = __isOK;
    return __return;
}

ResultValue &ResultValue::setResult(const ResultValue &value)const
{
    return p->setResult(value);
}

ResultValue &ResultValue::setResult(const QVariant &value)const
{
    return p->setResult(value);
}

bool ResultValue::operator==(const ResultValue &value)
{
    auto uuidA = this->returnHash();
    auto uuidB = value.returnHash();
    return (uuidA == uuidB);
}

bool ResultValue::operator!=(const ResultValue &value)
{
    auto uuidA = this->returnHash();
    auto uuidB = value.returnHash();
    return (uuidA != uuidB);
}

QStm::ResultInfo &ResultValue::resultInfo()const
{
    return p->resultInfo;
}

ResultValue &ResultValue::printInfo(const QVariant &v)
{
    VariantUtil vu(v);
    sInfo() << vu.toStr();
    return *this;
}

ResultValue &ResultValue::printWarning(const QVariant &v)
{
    VariantUtil vu(v);
    sWarning() << vu.toStr();
    return *this;
}

QVariant &ResultValue::resultVariant() const
{

    return p->resultItem.resultVariant;
}

QVariant ResultValue::resultVariantInfo() const
{
    if(p->resultItem.resultVariant.isNull() || !p->resultItem.resultVariant.isValid()){
        auto vHash=p->resultItem.resultVariant.toHash();
        vHash[QStringLiteral("resultInfo")]=p->resultInfo.toHash();
        return vHash;
    }

    switch (p->resultItem.resultVariant.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:{
        auto vHash=p->resultItem.resultVariant.toHash();
        vHash[QStringLiteral("resultInfo")]=p->resultInfo.toHash();
        return vHash;
    }
    default:
        return p->resultItem.resultVariant;
    }
}

QVariantMap ResultValue::resultMap() const
{
    return p->resultItem.resultVariant.toMap();
}

QVariantHash ResultValue::resultHash() const
{
    return p->resultItem.resultVariant.toHash();
}

QVariantList ResultValue::resultList() const
{

    auto &v=p->resultItem.resultVariant;
    switch (v.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return QVariantList{v};
    default:
        return p->resultItem.resultVariant.toList();
    }
}

QStringList ResultValue::resultStringList() const
{
    auto &v=p->resultItem.resultVariant;
    switch (v.typeId()) {
    case QMetaType::QStringList:
        return v.toStringList();
    case QMetaType::QVariantList:
    {
        Q_DECLARE_VU;
        QStringList __return;
        for(auto &row:v.toList()){
            switch (row.typeId()) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantList:
            case QMetaType::QStringList:
                __return.append(vu.toStr(row));
                break;
            case QMetaType::QUrl:
                __return.append(row.toUrl().toString());
                break;
            case QMetaType::QUuid:
                __return.append(row.toUuid().toString());
                break;
            default:
                __return.append(row.toString());
            }
        }
        return __return;
    }
    default:
        return {};
    }
}

QVariantList ResultValue::resultToList() const
{
    auto &v=p->resultItem.resultVariant;
    switch (v.typeId()) {
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return v.toList();
    default:
        if (v.isValid())
            return QVariantList{v};
        return {};
    }
}

QString ResultValue::resultString() const
{
    return p->resultItem.resultVariant.toString();
}

QByteArray ResultValue::resultByteArray() const
{
    return p->resultItem.resultVariant.toByteArray();
}

QUuid ResultValue::resultUuid() const
{
    return p->resultItem.resultVariant.toUuid();
}

qlonglong ResultValue::resultInt() const
{
    return p->resultItem.resultVariant.toLongLong();
}

qlonglong ResultValue::resultLongLong() const
{
    return p->resultItem.resultVariant.toLongLong();
}

double ResultValue::resultDouble() const
{
    return p->resultItem.resultVariant.toDouble();
}

QDate ResultValue::resultDate() const
{
    return p->resultItem.resultVariant.toDate();
}

QDateTime ResultValue::resultDateTime() const
{
    return p->resultItem.resultVariant.toDateTime();
}

QTime ResultValue::resultTime() const
{
    return p->resultItem.resultVariant.toTime();
}

QUrl ResultValue::resultUrl() const
{
    return p->resultItem.resultVariant.toUrl();
}

ResultValue &ResultValue::clear()
{
    p->clear();
    return *this;
}

const QVariantMap ResultValue::toMap()
{
    return QVariant(p->toHash()).toMap();
}

const QVariantHash &ResultValue::toHash()
{
    return p->toHash();
}

QByteArray &ResultValue::returnHash() const
{
    return p->resultItem.returnHash;
}

ResultValue::MessageType ResultValue::returnType() const
{
    return p->resultItem.returnType;
}

QVariant &ResultValue::returnCode() const
{
    return p->resultItem.returnCode;
}

QString &ResultValue::returnText() const
{
    return p->resultItem.returnText;
}

ResultValue &ResultValue::setMsg(const QVariant &value)
{
   return p->setMessage(MessageType::None, {}, value);
}

ResultValue &ResultValue::setInformation(const QVariant &value)
{
    return p->setMessage(MessageType::Information, {}, value);
}

ResultValue &ResultValue::setWarning()
{
    if (this->isOk())
        return this->setWarning(tr("Unknow error"));
    return *this;
}

ResultValue &ResultValue::setWarning(const QVariant &value)
{
    return p->setMessage(MessageType::Warning, {}, value);
}

ResultValue &ResultValue::setValidation()
{
    if (this->isOk())
        return this->setValidation(tr("Invalid information"));
    return *this;
}

ResultValue &ResultValue::setValidation(const QVariant &value)
{
    return p->setMessage(MessageType::Validation, {}, value);
}

ResultValue &ResultValue::setValidation(const ResultValue &lr)
{
    if(!lr){
        (*this) = lr;
        return *this;
    }
    return this->setValidation();
}

ResultValue &ResultValue::setResponse(const QVariant &value)
{
    switch (value.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    {
        auto vHash=value.toHash();
        //auto qt_status_code=vHash.value(QStringLiteral("qt_status_code")).toInt();
        auto status_code=vHash.value(QStringLiteral("status_code")).toInt();
        auto response_body=vHash.value(QStringLiteral("response_body"));
        auto reason_phrase=vHash.value(QStringLiteral("reason_phrase")).toString().trimmed();
        if(!statusCodeSuccessList->contains(status_code)){
            QStm::ResultInfo resultInfo;
            QVariant vResultInfo;
            switch (response_body.typeId()) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            {
                auto vHash=response_body.toHash();
                vResultInfo=vHash.value(QStringLiteral("resultInfo"));
                break;
            }
            default:
                vResultInfo=vHash.value(QStringLiteral("resultInfo"));
                break;
            }

            resultInfo.fromVar(vResultInfo);
            if(resultInfo.errors().isEmpty()){
                if(reason_phrase.isEmpty())
                    resultInfo.errors().append(reason_phrase);
            }
            if(!resultInfo.errors().isEmpty())
                reason_phrase=resultInfo.errors().first().toString().trimmed();
            this->setCode(status_code, reason_phrase);
            p->resultItem.resultVariant=response_body;
            p->resultInfo.fromResultInfo(resultInfo);
        }
        break;
    }
    default:
        break;
    }
    return *this;
}

ResultValue &ResultValue::setUnauthorized()
{
    static auto msg=tr("Unauthorized");
    return this->setUnauthorized(msg);
}

ResultValue &ResultValue::setUnauthorized(const QVariant &value)
{
    p->clear();
    if (value.isValid()) {
        p->resultItem.returnType = Unauthorized;
        p->resultItem.returnCode = p->variantConvertToCode(value);
        p->resultItem.returnText = p->variantConvertToText(value);
        p->makeResult();
    }
    return *this;
}

ResultValue &ResultValue::setUnauthorized(const ResultValue &lr)
{
    if (!lr){
        (*this) = lr;
        return *this;
    }
    return this->setUnauthorized();
}

ResultValue &ResultValue::setNotImplemented()
{
    static auto msg=tr("Not implemented");
    return this->setNotImplemented(msg);
}

ResultValue &ResultValue::setNotImplemented(const QVariant &value)
{
    p->clear();
    if (value.isValid()) {
        p->resultItem.returnType = NotImplemented;
        p->resultItem.returnCode = p->variantConvertToCode(value);
        p->resultItem.returnText = p->variantConvertToText(value);
        p->makeResult();
    }
    return *this;
}

ResultValue &ResultValue::setNotImplemented(const ResultValue &lr)
{
    if (!lr){
        (*this) = lr;
        return *this;
    }
    return this->setNotImplemented();
}

ResultValue &ResultValue::setBadRequest()
{
    static auto msg=tr("Bad format");
    return this->setBadRequest(msg);
}

ResultValue &ResultValue::setBadRequest(const QVariant &value)
{
    p->clear();
    if (value.isValid()) {
        p->resultItem.returnType = BadRequest;
        p->resultItem.returnCode = p->variantConvertToCode(value);
        p->resultItem.returnText = p->variantConvertToText(value);
        p->makeResult();
    }
    return *this;
}

ResultValue &ResultValue::setBadRequest(const ResultValue &lr)
{
    if (!lr){
        (*this) = lr;
        return *this;
    }
    return this->setBadRequest();
}

ResultValue &ResultValue::setNoContent()
{
    static auto msg=tr("No content");
    return this->setNoContent(msg);
}

ResultValue &ResultValue::setNoContent(const QVariant &value)
{
    p->clear();
    if (value.isValid()) {
        p->resultItem.returnType = NoContent;
        p->resultItem.returnCode = p->variantConvertToCode(value);
        p->resultItem.returnText = p->variantConvertToText(value);
        p->makeResult();
    }
    return *this;
}

ResultValue &ResultValue::setNotFound()
{
    static auto msg=tr("Not found");
    return this->setNotFound(msg);
}

ResultValue &ResultValue::setNotFound(const QVariant &value)
{
    p->clear();
    if (value.isValid()) {
        p->resultItem.returnType = Notfound;
        p->resultItem.returnCode = p->variantConvertToCode(value);
        p->resultItem.returnText = p->variantConvertToText(value);
        p->makeResult();
    }
    return *this;
}

ResultValue &ResultValue::setNotFound(const ResultValue &lr)
{
    if (!lr){
        (*this) = lr;
        return *this;
    }
    return this->setNotFound();
}

ResultValue &ResultValue::setUnsupportedMediaType()
{
    static auto msg=tr("Unsupported Media Type");
    return this->setCritical(msg);
}

ResultValue &ResultValue::setUnsupportedMediaType(const QVariant &value)
{
    p->clear();
    if (value.isValid()) {
        p->resultItem.returnType = UnsupportedMediaType;
        p->resultItem.returnCode = p->variantConvertToCode(value);
        p->resultItem.returnText = p->variantConvertToText(value);
        p->makeResult();
    }
    return *this;
}

ResultValue &ResultValue::setUnsupportedMediaType(const ResultValue &lr)
{
    if (!lr){
        (*this) = lr;
        return *this;
    }
    return this->setUnsupportedMediaType();
}

ResultValue &ResultValue::setCritical()
{
    if (!this->isCritical()) {
        static auto msg=tr("Critical error").toUtf8();
        auto message = p->resultItem.returnText.trimmed();
        message = message.isEmpty() ? msg : message;
        this->setCritical(message);
    }
    return *this;
}

ResultValue &ResultValue::setCritical(const QString &value)
{
    return p->setCritical(QVariant{}, value);
}

ResultValue &ResultValue::setCritical(const QVariantHash &value)
{
    return p->setCritical(QVariant{}, value);
}

ResultValue &ResultValue::setCritical(const QVariantList &value)
{
    return p->setCritical(QVariant{}, value);
}

ResultValue &ResultValue::setCritical(const ResultValue &lr)
{
    if (!lr){
        (*this) = lr;
        return *this;
    }
    return this->setCritical();
}

ResultValue &ResultValue::setCritical(const QVariant &code, const QVariant &message)
{
    return p->setCritical(code, message);
}

bool ResultValue::isOk() const
{
    return p->isOk();
}

bool ResultValue::isNotOk() const
{
    return !this->isOk();
}

bool ResultValue::isInformation() const
{
    return p->resultItem.returnType == Information;
}

bool ResultValue::isUnauthorized() const
{
    return p->resultItem.returnType == Unauthorized;
}

bool ResultValue::isNotfound() const
{
    return p->resultItem.returnType == Notfound;
}

bool ResultValue::isBadRequest() const
{
    return p->resultItem.returnType;
}

bool ResultValue::isWarning() const
{
    return p->resultItem.returnCode.isValid();
}

bool ResultValue::isValidation() const
{
    return p->resultItem.returnCode.isValid();
}

bool ResultValue::isCritical() const
{
    return p->resultItem.returnType == Critical;
}

bool ResultValue::isNoContent() const
{
    return p->resultItem.returnType == NoContent;
}

QVariantHash ResultValue::data() const
{
    return p->data;
}

void ResultValue::setData(const QVariantHash &data)
{
    p->data = data;
}

QVariant ResultValue::stateCode() const
{
    QVariantHash v;
    QVariant code;
    QVariant phrase;
    if (QThread::currentThread()->isInterruptionRequested())
        code = 500; //InternalServerError
    else if (staticMakeArStats->contains(this->returnType()))
        code = staticMakeArStats->value(this->returnType());
    else
        code = 500; //InternalServerError

    phrase = this->returnText().isEmpty() ? QVariant{} : this->returnText();

    if (code.isValid())
        v.insert(QByteArrayLiteral("code"), code);
    if (phrase.isValid())
        v.insert(QByteArrayLiteral("phrase"), phrase);

    return v.isEmpty() ? QVariant{} : v;
}

QVariant ResultValue::sc() const
{
    return this->stateCode();
}

ResultValue &ResultValue::setCode(const QVariant &statusCode, const QVariant &reasonPhrase)
{
    if (statusCodeSuccessList->contains(statusCode.toInt())){
        this->setInformation(QVariant{});
        return*this;
    }

    const auto &returnType = staticMakeArStats->key(statusCode.toInt());
    if (staticMakeArStats->contains(returnType)) {

        p->resultItem.returnType = returnType;
        p->resultItem.returnCode = p->variantConvertToCode(returnType);
        p->resultItem.returnText = reasonPhrase.toByteArray();
        if (!p->resultItem.returnCode.isValid())
            p->resultItem.returnCode = p->toMd5(p->resultItem.returnText);
        p->makeResult();
    }
    return *this;
}

QString ResultValue::toString() const
{
    return this->returnText();
}
