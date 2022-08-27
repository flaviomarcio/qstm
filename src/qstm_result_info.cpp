#include "./private/p_qstm_result_info.h"
#include <QMetaProperty>
#include <QJsonDocument>
#include "./qstm_util_variant.h"

namespace QStm {

ResultInfo::ResultInfo(QObject *parent):ObjectWrapper{parent}
{
    this->p=new ResultInfoPvt{this};
}
void ResultInfo::clear()
{
    p->clear();
}

void ResultInfo::clearErrors()
{
    p->clearErrors();
}

bool ResultInfo::enabled()
{
    return p->enabled;
}

void ResultInfo::setEnabled(bool value)
{
    p->enabled=value;
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

QStringList &ResultInfo::errors()
{
    return p->errors;
}

void ResultInfo::setErrors(const QVariant &value)
{
    Q_DECLARE_VU;
    p->errors=vu.toStringList(value);
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
    return p->per_page;
}

void ResultInfo::setPerPage(int value)
{
    p->per_page = value;
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
    return p->total_count;
}

void ResultInfo::setTotalCount(int value)
{
    p->total_count = value;
}

int ResultInfo::totalPages() const
{
    return p->total_pages;
}

void ResultInfo::setTotalPages(int value)
{
    p->total_pages = value;
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
