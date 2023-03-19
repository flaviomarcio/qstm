#include "./qstm_result_info.h"
#include "./qstm_util_variant.h"
#include "./qstm_meta_enum.h"
#include <QMetaProperty>
#include <QJsonDocument>

namespace QStm {

class ResultInfoPvt:public QObject
{
public :
    ResultInfo *parent=nullptr;
    bool enabled=false;
    int code=0;
    QStringList messages;
    bool success=true;
    int page=0;
    int perPage=9999999;
    int count=0;
    int totalCount=0;
    int totalPages=0;

    QStm::MetaEnum<ResultInfo::MessageType> messageType=ResultInfo::MessageType::None;

    explicit ResultInfoPvt(ResultInfo *parent):QObject{parent}
    {
        this->parent=parent;
    }

    void clear()
    {
        this->success=false;
        this->page=0;
        this->perPage=9999999;
        this->count=0;
        this->totalCount=0;
        this->totalPages=0;
        this->code=0;
        this->messages.clear();
        this->messageType=ResultInfo::MessageType::None;
        this->success=true;
    }
};

ResultInfo::ResultInfo(QObject *parent):ObjectWrapper{parent}
{
    this->p=new ResultInfoPvt{this};
}
void ResultInfo::clear()
{
    p->clear();
}

bool ResultInfo::enabled()
{
    return p->enabled;
}

void ResultInfo::setEnabled(bool value)
{
    p->enabled=value;
}

int ResultInfo::code()
{
    return p->code;
}

ResultInfo &ResultInfo::setCode(const QVariant &newCode)
{
    if (p->messageType == newCode)
        return *this;
    p->messageType = newCode;
    emit messageTypeChanged();
    return *this;
}

ResultInfo &ResultInfo::resetCode()
{
    return setCode(ResultInfo::None);
}

ResultInfo::MessageType ResultInfo::messageType() const
{
    return p->messageType.type();
}

void ResultInfo::setMessageType(const QVariant &value)
{
    if (p->messageType == value)
        return;
    p->messageType = value;
    emit messageTypeChanged();
}

void ResultInfo::resetMessageType()
{
    setMessageType(None);
}

QStringList &ResultInfo::messages()
{
    return p->messages;
}

void ResultInfo::setMessages(const QVariant &value)
{
    Q_DECLARE_VU;
    p->messages=vu.toStringList(value);
}

bool ResultInfo::success() const
{
    return p->success;
}

void ResultInfo::setSuccess(bool value)
{
    p->success=value;
}

int ResultInfo::page() const
{
    return p->page;
}

void ResultInfo::setPage(int value)
{
    p->page = value;
}

int ResultInfo::perPage() const
{
    return p->perPage;
}

void ResultInfo::setPerPage(int value)
{
    p->perPage = value;
}

int ResultInfo::count() const
{
    return p->count;
}

void ResultInfo::setCount(int value)
{
    p->count = value;
}

int ResultInfo::totalCount() const
{
    return p->totalCount;
}

void ResultInfo::setTotalCount(int value)
{
    p->totalCount = value;
}

int ResultInfo::totalPages() const
{
    return p->totalPages;
}

void ResultInfo::setTotalPages(int value)
{
    p->totalPages = value;
}

const QVariantHash ResultInfo::toRequestHash() const
{
    return {{QT_STRINGIFY(page),this->page()},{QT_STRINGIFY(per_page), this->perPage()}};
}

QVariant ResultInfo::toVar()const
{
    return this->toHash();
}

bool ResultInfo::fromVar(const QVariant &v)
{
    switch (v.typeId()) {
    case QMetaType::QString:
    case QMetaType::QByteArray:
        return this->fromHash(QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash());
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return this->fromHash(v.toHash());
    default:
        return false;
    }
}

bool ResultInfo::fromMap(const QVariantMap&map)
{
    bool __return=false;
    auto &metaObject = *this->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        if(property.write(this, map.value(property.name()))){
            __return=true;
        }
    }
    return __return;
}

bool ResultInfo::fromHash(const QVariantHash &map)
{
    bool __return=false;
    auto &metaObject = *this->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        if(property.write(this, map.value(property.name()))){
            __return=true;
        }
    }
    return __return;
}

bool ResultInfo::fromResultInfo(const ResultInfo &resultInfo)
{
    bool __return=false;
    auto &metaObject = *this->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        if(property.write(this, property.read(&resultInfo)))
            __return=true;
    }
    return __return;
}


}
