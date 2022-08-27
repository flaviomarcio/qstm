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
    per_page=9999999;
    count=0;
    total_count=0;
    total_pages=0;
    this->messages.clear();
    this->messageType=ResultInfo::MessageType::None;
    this->clearErrors();
}

void QStm::ResultInfoPvt::clearErrors()
{
    this->errors.clear();
    this->success=true;
}



}
