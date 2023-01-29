#include "./private/p_qstm_startup.h"
#include "./qstm_macro.h"

namespace QStm {

StartUp::StartUp(QObject *parent)
    : QObject{parent}
{
    this->p=new StartUpPvt{this};
}

bool StartUp::operator<(const StartUp *object) const
{
    return this->number()<object->number();
}

bool StartUp::operator>(const StartUp *object) const
{
    return this->number()>object->number();
}

StartUp &StartUp::i()
{
    return StartUpPvt::i();
}

bool StartUp::append(StartUpFunction func)
{
    p->list.append(func);
    return true;
}

void StartUp::clear()
{
    p->list.clear();
}

int StartUp::count() const
{
    return p->list.count();
}

void StartUp::run()
{

    if(this==&StartUpPvt::i()){
        this->p->runner.start();
        emit this->p->runnerStart();
        this->p->runner.wait(1000);
        return;
    }

    for(auto &func : p->list)
        func();
}

StartUpFunction StartUp::at(int index)
{
    if(index<0 || index>=p->list.count())
        return [](){sWarning()<<__PRETTY_FUNCTION__<<": invalid startup function";};
    return p->list.at(index);
}

QVector<StartUpFunction> &StartUp::methods() const
{
    return p->list;
}

int StartUp::number() const
{
    return p->number;
}

void StartUp::setNumber(int newNumber)
{
    if (p->number == newNumber)
        return;
    p->number = newNumber;
    emit numberChanged();
}

void StartUp::resetNumber()
{
    setNumber({});
}

int StartUp::interval() const
{
    return p->interval;
}

void StartUp::setInterval(int newInterval)
{
    if (p->interval == newInterval)
        return;
    p->interval = newInterval;
    emit intervalChanged();
}

void StartUp::resetInterval()
{
    setInterval({}); 
}

bool StartUp::enabled() const
{
    return p->enabled;
}

void StartUp::setEnabled(bool newEnabled)
{
    if (p->enabled == newEnabled)
        return;
    p->enabled = newEnabled;
    emit enabledChanged();
}

void StartUp::resetEnabled()
{
    setEnabled({}); 
}

} // namespace QStm
