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
    QUuid md5Counter;
    bool enabled=false;
    MetaEnum<ResultInfo::MessageType> messageType=ResultInfo::None;
    int code=0;
    QStringList messages;
    QString messagesText;
    bool success=true;
    int page=0;
    int perPage=0;
    int count=0;
    int totalCount=0;
    int totalPages=0;

    explicit ResultInfoPvt(ResultInfo *parent):QObject{parent}, parent{parent}
    {
    }

};

ResultInfo::ResultInfo(QObject *parent):ObjectWrapper{parent}, p{new ResultInfoPvt{this}}
{
}

bool ResultInfo::enabled()const
{
    if(p->perPage<=0)
        return false;
    return p->enabled;
}

ResultInfo &ResultInfo::enabled(const QVariant &value)
{
    p->enabled=value.toBool();
    return *this;
}

ResultInfo &ResultInfo::resetEnabled()
{
    return this->enabled({});
}

QUuid ResultInfo::md5Counter()
{
    return p->md5Counter;
}

ResultInfo &ResultInfo::md5Counter(const QVariant &value)
{
    Q_DECLARE_VU;
    p->md5Counter=vu.toMd5Uuid(value);
    emit md5CounterChanged();
    return *this;
}

ResultInfo &ResultInfo::resetMd5Counter()
{
    return this->md5Counter({});
}

int ResultInfo::code()
{
    return p->code;
}

ResultInfo &ResultInfo::code(const QVariant &newCode)
{
    if (p->messageType == newCode)
        return *this;
    p->messageType = newCode;
    emit messageTypeChanged();
    return *this;
}

ResultInfo &ResultInfo::resetCode()
{
    return code(ResultInfo::None);
}

ResultInfo::MessageType ResultInfo::messageType() const
{
    return p->messageType.type();
}

ResultInfo &ResultInfo::messageType(const QVariant &value)
{
    if (p->messageType == value)
        return *this;
    p->messageType = value;
    emit messageTypeChanged();
    return *this;
}

ResultInfo &ResultInfo::resetMessageType()
{
    return messageType(None);
}

QStringList &ResultInfo::messages()
{
    return p->messages;
}

ResultInfo &ResultInfo::messages(const QVariant &value)
{
    Q_DECLARE_VU;
    p->messages=vu.toStringList(value);
    return *this;
}

ResultInfo &ResultInfo::resetMessages()
{
    return this->messages({});
}

QString &ResultInfo::messagesText()
{
    QStringList list;
    for(auto &v:p->messages)
        list.append(v.trimmed());
    auto msg=list.join(',').trimmed();
    static const auto __format1=QString("%1-%2");
    static const auto __format2=QString("Error %1");
    if(p->code!=0 && !msg.isEmpty())
        p->messagesText=__format1.arg(p->code).arg(msg);
    else if(p->code!=0)
        p->messagesText=__format2.arg(p->code);
    else if(!msg.isEmpty())
        p->messagesText=msg;
    else
        p->messagesText.clear();
    return p->messagesText;
}

bool ResultInfo::success() const
{
    return p->success;
}

ResultInfo &ResultInfo::success(const QVariant &value)
{
    p->success=value.toBool();
    return *this;
}

ResultInfo &ResultInfo::resetSuccess()
{
    return this->success(true);
}

int ResultInfo::page() const
{
    if(this->totalPages()<=0)
        return 0;

    if(p->page<=0)
        return 1;

    return p->page;
}

ResultInfo &ResultInfo::page(const QVariant &value)
{
    p->page = value.toInt();
    emit pageChanged();
    return *this;
}

ResultInfo &ResultInfo::resetPage()
{
    return this->page(0);
}

int ResultInfo::perPage() const
{
    return p->perPage;
}

ResultInfo &ResultInfo::perPage(const QVariant &value)
{
    p->perPage = value.toInt();
    emit perPageChanged();
    emit totalPagesChanged();
    return *this;
}

ResultInfo &ResultInfo::resetPerPage()
{
    return this->perPage(0);
}

int ResultInfo::count() const
{
    return p->count;
}

ResultInfo &ResultInfo::count(const QVariant &value)
{
    p->count = value.toInt();
    emit countChanged();
    return *this;
}

ResultInfo &ResultInfo::resetCount()
{
    return this->count(0);
}

int ResultInfo::totalCount() const
{
    return p->totalCount;
}

ResultInfo &ResultInfo::totalCount(const QVariant &value)
{
    p->totalCount = value.toInt();
    emit totalCountChanged();
    emit totalPagesChanged();
    return *this;
}

ResultInfo &ResultInfo::resetTotalCount()
{
    return this->totalCount(0);
}

int ResultInfo::totalPages() const
{
    if(p->totalCount<=0 || p->perPage<=0)
        return 0;
    double pages=double(p->totalCount)/double(p->perPage);
    return pages>0 && pages<1?1:int(pages);
}

int ResultInfo::offSetRecords() const
{
    auto page=this->page()-1;
    page=(page<=0)?0:page;
    return this->perPage()*page;
}

}
