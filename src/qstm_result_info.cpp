#include "./qstm_result_info.h"
#include <QMetaProperty>
#include <QJsonDocument>
//#include "./qstm_meta_types.h"
//#include "./qstm_macro.h"

namespace QStm {

class ResultInfoPvt{
public :
    ResultInfo *parent=nullptr;
    bool enabled=false;
    QVariantList errors;
    QVariantList messages;
    bool success=true;
    int page=0;
    int per_page=9999999;
    int count=0;
    int total_count=0;
    int total_pages=0;

    explicit ResultInfoPvt(ResultInfo *parent)
    {
        this->parent=parent;
    }
    virtual ~ResultInfoPvt()
    {
    }

    void clear()
    {
        success=false;
        page=0;
        per_page=9999999;
        count=0;
        total_count=0;
        total_pages=0;
        this->messages.clear();
        this->clearErrors();
    }

    void clearErrors()
    {
        this->errors.clear();
        this->success=true;
    }
};

ResultInfo::ResultInfo(QObject *parent):ObjectWrapper{parent}
{
    this->p=new ResultInfoPvt{this};
}

ResultInfo::~ResultInfo()
{
    delete p;
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

QVariantList &ResultInfo::errors()
{
    return p->errors;
}

void ResultInfo::setErrors(const QVariantList &value)
{
    p->errors=value;
}

QVariantList &ResultInfo::messages()
{
    return p->messages;
}

void ResultInfo::setMessages(const QVariantList &value)
{
    p->messages=value;
}

int ResultInfo::success() const
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
    QVariantHash v;
    v[QT_STRINGIFY(page)]=this->page();
    v[QT_STRINGIFY(per_page)]=this->perPage();
    return v;
}

//QVariantMap ResultInfo::toMap()const
//{
//    QVariantMap __return;
//    auto &metaObject = *this->metaObject();
//    for(int col = 0; col < metaObject.propertyCount(); ++col) {
//        auto property = metaObject.property(col);
//        __return.insert(property.name(), property.read(this));
//    }
//    return __return;
//}

//QVariantHash ResultInfo::toHash() const
//{
//    QVariantHash __return;
//    auto &metaObject = *this->metaObject();
//    for(int col = 0; col < metaObject.propertyCount(); ++col) {
//        auto property = metaObject.property(col);
//        __return.insert(property.name(), property.read(this));
//    }
//    return __return;
//}

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
