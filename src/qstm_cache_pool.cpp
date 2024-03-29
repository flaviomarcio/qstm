#include "./qstm_cache_pool.h"
#include <QHash>
#include <QPair>
#include <QMutex>
#include <QDateTime>
#include <QVariant>
#include <QCryptographicHash>
#include <QJsonDocument>

namespace QStm {

typedef QMap <QThread*,QStm::CachePool*> ThreadCachePool;
typedef QPair<QDateTime,QVariant> CacheItem;

Q_GLOBAL_STATIC(ThreadCachePool, instancesMap)

class CachePoolPvt:public QObject{
public:
    QMutex cacheMutex;
    QHash<QByteArray, CacheItem> cache;
    explicit CachePoolPvt(QObject *parent):QObject{parent}
    {
    }

    QByteArray toMd5(const QVariant &value)const
    {
        QByteArray bytes;
        switch (value.typeId()) {
        case QMetaType::QVariantMap:
        case QMetaType::QVariantHash:
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            bytes = QJsonDocument::fromVariant(value).toJson(QJsonDocument::Compact);
            break;
        default:
            bytes = value.toByteArray();
        }
        return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
    }

    void clear()
    {
        QMutexLocker<QMutex> locker(&cacheMutex);
        this->cache.clear();
    }
};


CachePool::CachePool(QObject *parent) : QObject{parent},p{new CachePoolPvt{parent}}
{
}

CachePool &CachePool::i()
{
    return CachePool::instance(QThread::currentThread());
}

CachePool &CachePool::instance()
{
    return CachePool::instance(QThread::currentThread());
}

CachePool &CachePool::instance(QThread*currentThread)
{
    static QMutex instanceMutex;
    QMutexLocker<QMutex> locker(&instanceMutex);//enfileira e apenas 1 vai entrar
    auto &i=(*instancesMap)[currentThread];
    if(i==nullptr)
        i=new CachePool{};
    return *i;
}

CachePool &CachePool::clear()
{
    p->clear();
    return *this;
}

bool CachePool::get(const QVariant &key, QVariant &value) const
{
    value=this->get(key);
    return value.isValid();
}

QVariant CachePool::get(const QVariant &key) const
{
    if(!key.isValid())
        return {};

    QMutexLocker<QMutex> locker(&p->cacheMutex);
    auto hshMd5 = p->toMd5(key);
    return p->cache.value(hshMd5).second;
}

QByteArray CachePool::set(QVariant &value) const
{
    if(!value.isValid())
        return {};

    QMutexLocker<QMutex> locker(&p->cacheMutex);
    auto hshMd5 = p->toMd5(value);
    if(this->set(hshMd5, value))
        return hshMd5;
    return {};
}

bool CachePool::set(const QVariant &key, const QVariant &value) const
{
    if(!key.isValid() && !value.isValid())
        return false;


    auto hshMd5 = p->toMd5(key);
    QMutexLocker<QMutex> locker(&p->cacheMutex);
    auto pair=CacheItem(QDateTime::currentDateTime(), value);
    p->cache.insert(hshMd5, pair);
    return true;
}

QVariant CachePool::take(const QVariant &key) const
{
    if(!key.isValid())
        return {};

    QMutexLocker<QMutex> locker(&p->cacheMutex);
    auto hshMd5 = p->toMd5(key);
    if(p->cache.contains(hshMd5))
        return p->cache.take(hshMd5).second;
    return {};
}

QVariant CachePool::remove(const QVariant &key) const
{
    if(!key.isValid())
        return {};

    QMutexLocker<QMutex> locker(&p->cacheMutex);
    auto hshMd5 = p->toMd5(key);
    if(p->cache.contains(hshMd5))
        return p->cache.take(hshMd5).second;
    return {};
}

}


