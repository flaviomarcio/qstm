#include "./p_qstm_result_info.h"
#include <QMetaProperty>
#include <QJsonDocument>

namespace QStm {

QStm::ResultInfoPvt::ResultInfoPvt(ResultInfo *parent):QObject{parent}
{
    this->parent=parent;
}

void QStm::ResultInfoPvt::clear()
{
    success=false;
    page=0;
    perPage=9999999;
    count=0;
    totalCount=0;
    totalPages=0;
    this->messages.clear();
    this->messageType=ResultInfo::MessageType::None;
     this->success=true;
}

}
