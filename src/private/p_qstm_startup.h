#pragma once

#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include "../qstm_startup.h"

namespace QStm{

typedef QList<StartUp*> StartUpList;

class StartUpRunner: public QThread
{
    Q_OBJECT
public:

    explicit StartUpRunner();

    void run();

public slots:
    void runnerStart();
signals:
    void runnerTerminate();
};

class StartUpPvt:public QObject
{
    Q_OBJECT
    friend class StartUpRunner;
public:
    QMutex locker;
    int number=0;
    int interval=1000;
    bool enabled=true;
    StartUpRunner runner;
    QVector<StartUpFunction> list;
    explicit StartUpPvt(StartUp *parent);

    static StartUp &i();
private slots:
    void runnerTerminate();
signals:
    void runnerStart();
    void runnerQuit();
};


}
