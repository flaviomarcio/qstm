#include "./p_qstm_startup.h"
#include "../qstm_log.h"
#include <algorithm>

namespace QStm{

Q_GLOBAL_STATIC(QMutex, startUpMutex)
Q_GLOBAL_STATIC(StartUpList, startUpList)
Q_GLOBAL_STATIC(StartUp, staticStartUp)

static void init()
{
    Log::enabled();
    staticStartUp->run();
}

Q_COREAPP_STARTUP_FUNCTION(init)

StartUpPvt::StartUpPvt(StartUp *parent):QObject{parent}
{
    QObject::connect(this, &StartUpPvt::runnerStart, &this->runner, &StartUpRunner::runnerStart);
    startUpMutex->lock();
    startUpList->append(parent);
    startUpMutex->unlock();
}

StartUp &StartUpPvt::i()
{
    return *staticStartUp;
}

StartUpRunner::StartUpRunner():QThread{nullptr}
{

}

void StartUpRunner::run()
{
    this->exec();
}

void StartUpRunner::runnerStart()
{
    int max=0;
    QHash<int, StartUp*> hash;
    for(auto &v:*startUpList){
        max=(v->number()>max)?v->number():max;
        hash.insert(v->number(), v);
    }
    for(int i=0; i<=max; i++){
        auto v=hash.value(i);
        qInfo()<<QT_STRINGIFY2(QStm::StartUp)<<QStringLiteral(": module[%1]==%2").arg(i).arg(v->metaObject()->className());
        if(v){
            for(auto &func:v->methods())
                func();
        }
    }
    //this->quit();
}

}
