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
    QStringList messages;
    bool success=true;
    int page=0;
    int perPage=9999999;
    int count=0;
    int totalCount=0;
    int totalPages=0;

    MetaEnum<ResultInfo::MessageType> messageType=ResultInfo::MessageType::None;

    explicit ResultInfoPvt(ResultInfo *parent);

    void clear();
};


}
