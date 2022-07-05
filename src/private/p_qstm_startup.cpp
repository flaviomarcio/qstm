#include "./p_qstm_startup.h"

namespace QStm{

Q_GLOBAL_STATIC(QMutex, startUpMutex)
Q_GLOBAL_STATIC(StartUpList, startUpList)
Q_GLOBAL_STATIC(StartUp, staticStartUp)

static void init()
{
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

StartUpPvt::~StartUpPvt()
{

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
//    StartUpList list=*startUpList;
//    staticStartUp->setNumber(0);
//    struct EntryCompare {
//        bool operator()(const StartUp *a, const StartUp *b) {return(a < b);}
//    };
//    std::sort(list.begin(), list.end(), EntryCompare());

    int max=0;
    QHash<int, StartUp*> hash;
    for(auto&v:*startUpList){
        max=(v->number()>max)?v->number():max;
        hash.insert(v->number(), v);
    }
    for(int i=0; i<=max; i++){
        auto v=hash.value(i);
        if(v){
            for(auto&func:v->methods())
                func();
        }
    }
    //this->quit();
}

}
