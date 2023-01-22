#include "./qstm_result.h"
#include "./qstm_macro.h"
#include "./qstm_object.h"
#include "./qstm_meta_enum.h"
#include "./qstm_util_meta_object.h"
#include "./qstm_util_variant.h"
#include "./private/p_qstm_result_info.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QMutex>
#include <QThread>

using namespace QStm;

class ResultValuePvt;

static const auto __returnBool = "returnBool";
static const auto __returnType = "returnType";
static const auto __returnCode = "returnCode";
static const auto __returnHash = "returnHash";
static const auto __returnText = "returnText";
static const auto __resultVariant = "resultVariant";
static const auto __resultInfo = "resultInfo";
//static const auto __point = ".";
static const auto __2point = ":";
//static const auto __code = "code";
//static const auto __phrase = "phrase";
static const auto __status_code="status_code";
static const auto __response_body="response_body";
static const auto __reason_phrase="reason_phrase";
static const auto __enter="\n";
static const auto __arg2="%1==%2";

static QByteArray toMd5(const QVariant &v)
{
    return QCryptographicHash::hash(v.toByteArray(), QCryptographicHash::Md5).toHex();
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
        return QString{__arg2}.arg(QObject::tr("Error value"), value.toString());
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
                list.append(QString{__arg2}.arg(i.key(), i.value().toByteArray()));
            }
            break;
        }
    default:
        list.append(value.toString().trimmed());
        break;
    }
    return list.join(__enter).toUtf8();
}


class QStmThreadReturnItem{
public:
    QUuid uuid = QUuid::createUuid();
    QVariantHash dataHash;
    QVariantMap dataMap;
    bool returnBool = true;
    QByteArray returnCode;
    QByteArray returnHash;
    QString returnText;
    QVariant resultVariant;
    QStm::ResultInfo resultInfo;

    explicit QStmThreadReturnItem()
    {
    }

    virtual ~QStmThreadReturnItem()
    {
    }

    void operator=(const QStmThreadReturnItem *item)
    {
        this->read(item);
    }

    void read(const QStmThreadReturnItem *item)
    {
        this->uuid=item->uuid;
        this->dataHash=item->dataHash;
        this->dataMap=item->dataMap;
        this->returnBool=item->returnBool;
        this->returnCode=item->returnCode;
        this->returnHash=item->returnHash;
        this->returnText=item->returnText;
        this->resultVariant=item->resultVariant;
        this->resultInfo=&item->resultInfo;
    }


    void clear()
    {
        this->uuid=QUuid::createUuid();
        this->dataHash.clear();
        this->dataMap.clear();
        this->returnBool = true;
        this->returnHash = {};
        this->returnCode = {};
        this->returnText.clear();
        this->resultVariant = {};
        this->resultInfo.clear();
        this->resultInfo.setSuccess(true);
    }

    bool makeResult()
    {
        this->returnBool=true;
        this->resultVariant=QVariantHash{{__resultInfo, this->resultInfo.toHash()}};
        if (this->isOkCodes()) {
            this->clear();
            this->resultInfo.messages().append(this->returnText);
            return true;
        }
        this->returnHash = toMd5(this->toHash());
        if(this->returnCode.isEmpty())
            this->returnCode=this->returnHash;

        if(!this->returnText.trimmed().isEmpty()){
            this->resultInfo.errors().clear();
            this->resultInfo.errors().append(this->returnText);
        }
        this->resultVariant=QVariantHash{{__resultInfo, this->resultInfo.toHash()}};
        this->returnBool=false;
        this->resultInfo.setSuccess(false);
        return {};
    }

    bool isOkCodes()const
    {
        if(!this->resultInfo.p->messageType.equal(ResultValue::Information) && !this->resultInfo.p->messageType.equal(ResultValue::None)){
            if (this->returnCode.isEmpty() || !this->returnText.trimmed().isEmpty())
                return {};
        }
        return true;
    }

    bool isOkResult()const
    {
        if (QThread::currentThread()->isInterruptionRequested())
            return {};

        if(!this->resultInfo.p->messageType.equal(ResultValue::Information) && !this->resultInfo.p->messageType.equal(ResultValue::None))
            return {};

        if (!this->returnCode.isEmpty() || !this->returnText.trimmed().isEmpty())
            return {};

        if(!this->returnBool)
            return {};

        return true;
    }

    QString toString() const
    {
        if (this->returnCode.isEmpty()) {
            if (this->returnCode == this->returnText)
                return this->returnText;
            return this->returnCode + __2point + this->returnText;
        }
        return this->returnText;
    }

    const QVariantHash &toHash()
    {
        dataHash.clear();
        dataHash.insert(__returnBool, this->returnBool);
        dataHash.insert(__returnType, this->resultInfo.messageType());
        dataHash.insert(__returnCode, this->returnCode);
        dataHash.insert(__returnHash, this->returnHash);
        dataHash.insert(__returnText, this->returnText);
        dataHash.insert(__resultVariant, this->resultVariant);
        dataHash.insert(__resultInfo, this->resultInfo.toHash());
        return dataHash;
    }

    const QVariantMap &toMap()
    {
        return (dataMap=QVariant{this->toHash()}.toMap());
    }
};

class QStmResults:public QObject{
public:
    QMutex mutex;
    QHash<qlonglong, QStmThreadReturnItem*> results;

    explicit QStmResults(QObject *parent=nullptr):QObject{parent}{}

    ~QStmResults()
    {
        auto aux=results.values();
        results.clear();
        qDeleteAll(aux);
    }

    bool insert(const QStmThreadReturnItem &newItem)
    {
        QMutexLocker<QMutex> locker(&mutex);
        if (newItem.returnHash.trimmed().isEmpty())
            return {};

        auto thread=QThread::currentThread();
        if(!thread)
            return {};

        auto tId=qlonglong(QThread::currentThreadId());

        auto item = results.value(tId);
        if(item && (item->returnHash==newItem.returnHash)){
            *item=&newItem;
            return false;
        }

        if(!item){
            item=new QStmThreadReturnItem{};
            results.insert(tId, item);
        }
        *item=&newItem;
        QObject::connect(thread, &QThread::finished, this, &QStmResults::threadFinished);
        return true;
    }

    QStmThreadReturnItem &returnItem()
    {
        QMutexLocker<QMutex> locker(&mutex);
        return *results[qlonglong(QThread::currentThreadId())];
    }

    bool equal(const QStmThreadReturnItem &returnItem)
    {
        const auto &item=this->returnItem();
        if(item.returnHash==returnItem.returnHash)
            return true;
        return {};
    }

    void clear()
    {
        QMutexLocker<QMutex> locker(&mutex);
        auto id=qlonglong(QThread::currentThreadId());
        if(results.contains(id))
            results.remove(id);
    }

    void clear(const QStmThreadReturnItem &returnItem)
    {
        if(!this->equal(returnItem))
            return;
        this->clear();
    }
private slots:
    void threadFinished()
    {
        this->clear();
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

class ResultValuePvt:public QObject
{
public:
    ResultValue *parent = nullptr;
    QStmThreadReturnItem returnItem;

    explicit ResultValuePvt(ResultValue *parent):QObject{parent}
    {
        this->parent = parent;
    }

    ResultValue &setResult(const ResultValue &value)
    {
        this->readFull(value.p);
        return *this->parent;
    }

    ResultValue &setResult(const QVariant &value)
    {
        switch (value.typeId()) {
        case QMetaType::Bool:
            this->returnItem.returnBool = value.toBool();
            break;
        case QMetaType::QObjectStar:
        {
            {
                auto *object=value.value<ResultValue*>();
                if(object){
                    this->readFull(object->p);
                    break;
                }
            }

            {
                auto *object=value.value<QObject*>();
                if(object){
                    this->clearReturn();
                    MetaObjectUtil util;
                    returnItem.resultVariant=util.toHash(object);
                    break;
                }
            }
            break;
        }
        default:
            this->clearReturn();
            returnItem.resultVariant=value;
        };
        return *this->parent;
    }

    ResultValue &setMessage(ResultValue::MessageType msgType, const QVariant &code, const QVariant &message)
    {
        Q_DECLARE_VU;
        this->clear();
        if (message.isValid()) {
            returnItem.returnCode = vu.toByteArray(code);
            returnItem.returnText = variantConvertToText(message);
            returnItem.resultInfo.setMessageType(msgType);
            if(!returnItem.makeResult()){
                if(!staticResults->insert(returnItem))
                    return *this->parent;
            }
            this->makeParentResult();
        }
        return *this->parent;
    }

    void makeParentResult()
    {
        if(this->returnItem.isOkResult())
            return;
        {//set results parent's
            QList<ResultValue *> listParentResultValue;
            auto pp = dynamic_cast<QStm::Object *>(this->parent->parent());

            if(pp && pp==this->parent->parent())
                pp=dynamic_cast<QStm::Object *>(pp->parent());

            while (pp) {
                const auto &lr = &pp->lr();
                listParentResultValue.insert(0, lr);
                pp = dynamic_cast<QStm::Object *>(pp->parent());
            }
            sWarning() << this->returnItem.toString();//TODO INCLUIR DIRECTIVA PARA OCULTAR O ERRO
            for(int i=listParentResultValue.count()-1; i>=0; i--){
                auto parent=listParentResultValue.at(i);
                if(parent->parent())
                    sWarning() << parent->parent()->metaObject()->className();
                parent->p->read(this);
            }
        }
    }

    void clear()
    {
        this->returnItem.clear();
        staticResults->clear();
    }

    void clearReturn()
    {
        this->returnItem.clear();
    }

    void read(ResultValuePvt *pvt)
    {
        if (pvt == nullptr) {
            this->clear();
            return;
        }
        this->returnItem = &pvt->returnItem;
    }

    void readFull(const ResultValuePvt *pvt)
    {
        if (pvt == nullptr) {
            this->clear();
            return;
        }
        this->returnItem.read(&pvt->returnItem);
    }

    static QByteArray toMd5(const QVariant &v)
    {
        return QCryptographicHash::hash(v.toByteArray(), QCryptographicHash::Md5).toHex();
    }

};

ResultValue::ResultValue(QObject *parent) : QObject{parent}
{
    this->p = new ResultValuePvt{this};
}

ResultValue::operator bool() const
{
    return p->returnItem.isOkResult();
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
    auto hashA = this->returnHash();
    auto hashB = value.returnHash();
    return (hashA == hashB);
}

bool ResultValue::operator!=(const ResultValue &value)
{
    auto uuidA = this->returnHash();
    auto uuidB = value.returnHash();
    return (uuidA != uuidB);
}

QStm::ResultInfo &ResultValue::resultInfo()const
{
    return p->returnItem.resultInfo;
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
    return p->returnItem.resultVariant;
}

QVariant ResultValue::resultVariantInfo() const
{
    if(p->returnItem.resultVariant.isNull() || !p->returnItem.resultVariant.isValid()){
        auto vHash=p->returnItem.resultVariant.toHash();
        vHash.insert(__resultInfo, p->returnItem.resultInfo.toHash());
        return vHash;
    }

    switch (p->returnItem.resultVariant.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:{
        auto vHash=p->returnItem.resultVariant.toHash();
        vHash.insert(__resultInfo, p->returnItem.resultInfo.toHash());
        return vHash;
    }
    default:
        return p->returnItem.resultVariant;
    }
}

QVariantMap ResultValue::resultMap() const
{
    return p->returnItem.resultVariant.toMap();
}

QVariantHash ResultValue::resultHash() const
{
    return p->returnItem.resultVariant.toHash();
}

QVariantList ResultValue::resultList() const
{

    auto &v=p->returnItem.resultVariant;
    switch (v.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return QVariantList{v};
    default:
        return p->returnItem.resultVariant.toList();
    }
}

QStringList ResultValue::resultStringList() const
{
    auto &v=p->returnItem.resultVariant;
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
    auto &v=p->returnItem.resultVariant;
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
    return p->returnItem.resultVariant.toString();
}

QByteArray ResultValue::resultByteArray() const
{
    return p->returnItem.resultVariant.toByteArray();
}

QUuid ResultValue::resultUuid() const
{
    return p->returnItem.resultVariant.toUuid();
}

qlonglong ResultValue::resultInt() const
{
    return p->returnItem.resultVariant.toLongLong();
}

qlonglong ResultValue::resultLongLong() const
{
    return p->returnItem.resultVariant.toLongLong();
}

double ResultValue::resultDouble() const
{
    return p->returnItem.resultVariant.toDouble();
}

QDate ResultValue::resultDate() const
{
    return p->returnItem.resultVariant.toDate();
}

QDateTime ResultValue::resultDateTime() const
{
    return p->returnItem.resultVariant.toDateTime();
}

QTime ResultValue::resultTime() const
{
    return p->returnItem.resultVariant.toTime();
}

QUrl ResultValue::resultUrl() const
{
    return p->returnItem.resultVariant.toUrl();
}

ResultValue &ResultValue::clear()
{
    p->clear();
    return *this;
}

ResultValue &ResultValue::clearScope()
{
    staticResults->clear();
    return *this;
}

const QVariantMap &ResultValue::toMap()
{
    return p->returnItem.toMap();
}

const QVariantHash &ResultValue::toHash()
{
    return p->returnItem.toHash();
}

QByteArray &ResultValue::returnHash() const
{
    return p->returnItem.returnHash;
}

ResultValue::MessageType ResultValue::returnType() const
{
    int m=p->returnItem.resultInfo.messageType();
    return ResultValue::MessageType(m);
}

QByteArray &ResultValue::returnCode() const
{
    return p->returnItem.returnCode;
}

QString &ResultValue::returnText() const
{
    return p->returnItem.returnText;
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
        auto status_code=vHash.value(__status_code).toInt();
        auto response_body=vHash.value(__response_body);
        auto reason_phrase=vHash.value(__reason_phrase).toString().trimmed();
        if(!statusCodeSuccessList->contains(status_code)){
            QStm::ResultInfo resultInfo;
            QVariant vResultInfo;
            switch (response_body.typeId()) {
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            {
                auto vHash=response_body.toHash();
                vResultInfo=vHash.value(__resultInfo);
                break;
            }
            default:
                vResultInfo=vHash.value(__resultInfo);
                break;
            }

            resultInfo.fromVar(vResultInfo);
            if(resultInfo.errors().isEmpty()){
                if(reason_phrase.isEmpty())
                    resultInfo.errors().append(reason_phrase);
            }
            if(!resultInfo.errors().isEmpty())
                reason_phrase=resultInfo.errors().first().trimmed();
            this->setCode(status_code, reason_phrase);
            p->returnItem.resultVariant=response_body;
            p->returnItem.resultInfo=&resultInfo;
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
    if (value.isValid())
        p->setMessage(Unauthorized, {}, value);
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
    if (value.isValid())
        p->setMessage(NotImplemented, {}, value);
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
    if (value.isValid())
        p->setMessage(BadRequest, {}, value);
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
    if (value.isValid())
        p->setMessage(NoContent, {}, value);
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
    if (value.isValid())
        p->setMessage(Notfound, {}, value);
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
    if (value.isValid())
        p->setMessage(UnsupportedMediaType, {}, value);
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
        auto message = p->returnItem.returnText.trimmed();
        message = message.isEmpty() ? msg : message;
        this->setCritical(message);
    }
    return *this;
}

ResultValue &ResultValue::setCritical(const QString &value)
{
    return p->setMessage(Critical, {}, value);
}

ResultValue &ResultValue::setCritical(const QVariantHash &value)
{
    return p->setMessage(Critical, {}, value);
}

ResultValue &ResultValue::setCritical(const QVariantList &value)
{
    return p->setMessage(Critical, {}, value);
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
    return p->setMessage(Critical, code, message);
}

bool ResultValue::isOk() const
{
    return p->returnItem.isOkCodes();
}

bool ResultValue::isNotOk() const
{
    return !p->returnItem.isOkCodes();
}

bool ResultValue::isInformation() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m==Information;
}

bool ResultValue::isUnauthorized() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m==Unauthorized;
}

bool ResultValue::isNotfound() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m==Notfound;
}

bool ResultValue::isBadRequest() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m==BadRequest;
}

bool ResultValue::isWarning() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m==Warning;
}

bool ResultValue::isValidation() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m==Validation;
}

bool ResultValue::isCritical() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m==Critical;
}

bool ResultValue::isNoContent() const
{
    int m=p->returnItem.resultInfo.messageType();
    return m == NoContent;
}

QVariantHash ResultValue::data() const
{
    return p->returnItem.dataHash;
}

QVariant ResultValue::stateCode() const
{

    QVariant code;

    if (QThread::currentThread()->isInterruptionRequested())
        code = 500; //InternalServerError
    else if (staticMakeArStats->contains(this->returnType()))
        code = staticMakeArStats->value(this->returnType());
    else
        code = 500; //InternalServerError

    return code;
}

QVariant ResultValue::sc() const
{
    return this->stateCode();
}

ResultValue &ResultValue::setCode(const QVariant &statusCode, const QVariant &reasonPhrase)
{
    if (statusCodeSuccessList->contains(statusCode.toInt())){
        this->setInformation(QVariant{});
        return *this;
    }

    const auto &returnType = staticMakeArStats->key(statusCode.toInt());
    if (staticMakeArStats->contains(returnType)) {
        p->setMessage(returnType,statusCode,reasonPhrase);
    }
    return *this;
}

QString ResultValue::toString() const
{
    return this->returnText();
}
