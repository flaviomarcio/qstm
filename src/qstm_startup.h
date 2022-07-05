#pragma once

#include <QObject>
#include "./qstm_global.h"

#define Q_STM_STARTUP_APPEND(MODULE, FUNC)\
static void qstm_startup_init_##FUNC(){\
    MODULE::StartUp::i().append(FUNC);\
}\
Q_COREAPP_STARTUP_FUNCTION(qstm_startup_init_##FUNC)


#define Q_STM_STARTUP_FUNCTION(FUNC) Q_STM_STARTUP_APPEND(QStm, FUNC)

#define Q_STM_STARTUP_NUMBER(NUMBER) public: virtual int number() const { return NUMBER;}
#define Q_STM_STARTUP_INSTANCE(NUMBER) static StartUp &i(){ static StartUp _i; return _i;}
#define Q_STM_STARTUP_CONSTUCTOR(NUMBER) \
public: \
Q_STM_STARTUP_NUMBER(NUMBER) \
Q_STM_STARTUP_INSTANCE(1) \
Q_INVOKABLE explicit StartUp(QObject *parent = nullptr):QStm::StartUp{parent}{}

namespace QStm {

class StartUpPvt;
class StartUpRunner;
typedef std::function<void()> StartUpFunction;

//!
//! \brief The StartUp class
//!
class Q_STM_EXPORT StartUp : public QObject
{
    Q_OBJECT
    friend class StartUpRunner;
    friend class StartUpPvt;
public:
    //!
    //! \brief StartUp
    //! \param parent
    //!
    explicit StartUp(QObject *parent = nullptr);
    ~StartUp();

    bool operator<(const StartUp *object) const;

    bool operator>(const StartUp *object) const;

    //!
    //! \brief i
    //! \return
    //!
    static StartUp &i();

    //!
    //! \brief append
    //! \param func
    //! \return
    //!
    bool append(StartUpFunction func);

    //!
    //! \brief clear
    //!
    void clear();

    //!
    //! \brief count
    //! \return
    //!
    int count()const;

    //!
    //! \brief run
    //!
    void run();

    //!
    //! \brief at
    //! \param index
    //! \return
    //!
    StartUpFunction at(int index);

    //!
    //! \brief methods
    //! \return
    //!
    QVector<StartUpFunction> &methods()const;

    virtual int number() const;
    void setNumber(int newNumber);
    void resetNumber();

    int interval() const;
    void setInterval(int newInterval);
    void resetInterval();

    bool enabled() const;
    void setEnabled(bool newEnabled);
    void resetEnabled();

private:
    StartUpPvt *p=nullptr;
signals:
    void numberChanged();
    void intervalChanged();
    void enabledChanged();
};

} // namespace QStm
