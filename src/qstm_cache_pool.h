#pragma once

#include <QObject>
#include <QThread>
#include <QVariant>
#include "./qstm_global.h"

namespace QStm {

class Q_STM_EXPORT CachePool : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit CachePool(QObject *parent = nullptr);
    Q_INVOKABLE ~CachePool();

    static CachePool&i();
    static CachePool&instance();
    static CachePool&instance(QThread *currentThread);

    /**
     * @brief clear
     * @return
     */
    virtual CachePool &clear();

    /**
     * @brief get
     * @param key
     * @return
     */
    virtual bool get(const QVariant &key, QVariant &value)const;
    virtual QVariant get(const QVariant &key)const;

    /**
     * @brief set
     * @param value
     * @return
     */
    virtual QByteArray set(QVariant&value)const;
    virtual bool set(const QVariant &key, const QVariant&value)const;

    /**
     * @brief take
     * @param key
     * @return
     */
    virtual QVariant take(const QVariant &key)const;

    /**
     * @brief remove
     * @param key
     * @return
     */
    virtual QVariant remove(const QVariant &key)const;

private:
    void*p=nullptr;
};

}