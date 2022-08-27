#include "../qstm_result_info.h"
#include <QMetaProperty>
#include "../qstm_meta_enum.h"

namespace QStm {

class ResultInfoPvt:public QObject
{
    Q_OBJECT
public :
    ResultInfo *parent=nullptr;
    bool enabled=false;
    QStringList errors;
    QStringList messages;
    bool success=true;
    int page=0;
    int per_page=9999999;
    int count=0;
    int total_count=0;
    int total_pages=0;

    MetaEnum<ResultInfo::MessageType> messageType=ResultInfo::MessageType::None;

    explicit ResultInfoPvt(ResultInfo *parent);

    void clear();

    void clearErrors();
};


}
