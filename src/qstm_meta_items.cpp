#include "qstm_meta_items.h"
#include "qstm_util_meta_object.h"
#include "qstm_util_variant.h"
#include <QMutex>

namespace PrivateQStm {

static const auto __propertyIdName="propertyIdName";

class MetaItemsPvt:public QObject
{
public:
    QMutex mutex;
    QString propertyId;
    QVariantList itemsList;
    const QMetaObject &itemMetaObject;
    QStm::MetaObjectUtil mObj;
    QHash<int, QObject*> itemsHashByIndex;
    QHash<QUuid, QObject*> itemsHashById;
    explicit MetaItemsPvt(const QMetaObject &itemMetaObject, QObject *parent=nullptr)
        :QObject{parent},
        itemMetaObject{itemMetaObject},
        mObj{itemMetaObject, this}
    {
        {
            QVariant returnValue(QByteArray{});
            if(mObj.invoke(__propertyIdName, returnValue))
                this->propertyId=returnValue.toString().trimmed().toLower();

        }
    }

    void clear(){
        this->propertyId.clear();
        this->clean();
    }

    void clean(){
        this->propertyId.clear();
        auto aux=itemsHashByIndex;
        itemsHashByIndex.clear();
        itemsHashById.clear();
        qDeleteAll(aux);
    }

    QVariant getPropertyIdValue(const QVariantHash &v){
        Q_DECLARE_VU;
        if(v.contains(this->propertyId))
            return v.value(this->propertyId);

        QHashIterator<QString, QVariant> i(v);
        while(i.hasNext()){
            i.next();
            if(i.key().trimmed().toLower()==this->propertyId)
                return i.value();
        }
        return {};
    }

    QUuid getPropertyId(QObject *o){
        Q_DECLARE_VU;
        if(this->propertyId.trimmed().isEmpty()){
            auto property=this->mObj.property(this->propertyId.toUtf8());
            if(property.isValid() && property.isReadable())
                return vu.toMd5Uuid(property.read(o));
        }
        return {};
    }



    QUuid getPropertyId(const QVariantHash &v){
        auto itemIdValue=getPropertyIdValue(v);
        if(itemIdValue.isNull() || !itemIdValue.isValid())
            return {};
        Q_DECLARE_VU;
        return vu.toMd5Uuid(itemIdValue);
    }

    QObject *itemAdd(int index){
        if(index<0 || index>=this->itemsList.count())
            return nullptr;
        auto itemValues=this->itemsList.at(index).toHash();
        const auto itemId=this->getPropertyId(itemValues);

        auto o=this->mObj.newInstance();
        if(o==nullptr)
            return nullptr;

        this->itemsHashByIndex.insert(index,o);

        Q_DECLARE_VU;
        this->mObj.writeHash(o,itemValues);
        if(!itemId.isNull())
            this->itemsHashById.insert(itemId, o);

        return o;
    }

    void itemsUpdate(const QVariant &newValues){
        Q_DECLARE_VU;
        auto vList=vu.toList(newValues);
        for(auto&v: vList){
            auto itemValues=v.toHash();
            auto itemId=this->getPropertyId(itemValues);
            if(itemId.isNull()){
                this->itemsList.append(v);
                this->itemAdd(this->itemsList.count()-1);
            }
            else if(this->itemsHashById.contains(itemId)){
                auto item=this->itemsHashById.value(itemId);
                if(itemValues.isEmpty())
                    this->mObj.reset(item);
                else
                    this->mObj.writeHash(item, itemValues);
            }
        }
    }
};

MetaItems::MetaItems(const QMetaObject &itemMetaObject, QObject *parent)
    :QObject{parent},
    p{new MetaItemsPvt{itemMetaObject, this}}
{
}

QString &MetaItems::propertyId() const
{
    return p->propertyId;
}

MetaItems &MetaItems::propertyId(const QString &newPropertyId)
{
    QMutexLocker locker(&p->mutex);
    p->propertyId=newPropertyId;
    return *this;
}

MetaItems &MetaItems::clear()
{
    QMutexLocker locker(&p->mutex);
    p->clear();
    return *this;
}

MetaItems &MetaItems::clean()
{
    QMutexLocker locker(&p->mutex);
    p->clean();
    return *this;
}

int MetaItems::count()
{
    QMutexLocker locker(&p->mutex);
    return p->itemsList.count();
}

QObject *MetaItems::item(int index)
{
    QMutexLocker locker(&p->mutex);
    if(index<0 || index>=p->itemsList.count())
        return nullptr;
    auto itemId=p->getPropertyId(p->itemsList.at(index).toHash());
    if(!itemId.isNull()){
        auto item=p->itemsHashById.value(itemId);
        if (item!=nullptr)
            return item;
    }
    return p->itemAdd(index);
}

MetaItems &MetaItems::itemsUpdate(const QVariant &v)
{
    p->itemsUpdate(v);
    return *this;
}

QObject *MetaItems::itemById(const QVariant &id)
{
    if(p->propertyId.trimmed().isEmpty())
        return nullptr;
    QMutexLocker locker(&p->mutex);
    Q_DECLARE_VU;
    auto itemId=vu.toMd5Uuid(id);
    if(itemId.isNull())
        return nullptr;

    auto item=p->itemsHashById.value(itemId);
    if(item!=nullptr)
        return item;

    for (int index = 0; index < p->itemsList.count(); ++index) {
        const auto &v=p->itemsList[index];
        if(v.isNull() || !v.isValid())
            continue;
        auto itemValues=v.toHash();
        if(itemValues.isEmpty())
            continue;
        if(!itemValues.contains(p->propertyId.trimmed()))
            continue;
        auto itemId=itemValues.value(p->propertyId.trimmed());
        if(itemId.isNull() || !itemId.isValid())
            continue;

        return p->itemAdd(index);
    }
    return nullptr;

}

QString MetaItems::itemJson(int index)
{
    Q_DECLARE_VU;
    auto item=this->item(index);
    return vu.toStr(p->mObj.toHash(item));
}

QString MetaItems::itemJsonById(const QVariant &id)
{
    Q_DECLARE_VU;
    auto item=this->itemById(id);
    return vu.toStr(p->mObj.toHash(item));
}

QObject *MetaItems::take(int index)
{
    QMutexLocker locker(&p->mutex);
    if(index<0 || index>=p->itemsList.count())
        return nullptr;
    QObject *item=nullptr;
    if(p->itemsHashByIndex.contains(index)){
        item=p->itemsHashByIndex.value(index);
        auto itemKey=p->itemsHashById.key(item);
        p->itemsHashById.remove(itemKey);
    }
    p->itemsList.remove(index);
    return item;
}

QObject *MetaItems::takeById(const QVariant &id)
{
    if(p->propertyId.trimmed().isEmpty())
        return nullptr;
    QMutexLocker locker(&p->mutex);
    Q_DECLARE_VU;
    auto itemId=vu.toMd5Uuid(id);
    if(itemId.isNull())
        return nullptr;

    QObject *item=nullptr;
    if(p->itemsHashById.contains(itemId)){
        item=p->itemsHashById.take(itemId);
        if(item!=nullptr){
            auto index=p->itemsHashByIndex.key(item);
            if(index>=0 && index<p->itemsList.count())
                p->itemsHashByIndex.remove(index);
        }
    }
    return item;
}

QString MetaItems::toJsonList()
{
    QMutexLocker locker(&p->mutex);
    Q_DECLARE_VU;
    return vu.toStr(p->itemsList);
}

QString MetaItems::toJsonHash()
{
    if(p->propertyId.trimmed().isEmpty())
        return {};
    QMutexLocker locker(&p->mutex);
    Q_DECLARE_VU;
    QVariantHash __return;
    for(auto&v:p->itemsList){
        auto itemId=p->getPropertyIdValue(v.toHash());
        __return.insert(vu.toStr(itemId), v);
    }
    return vu.toStr(__return);
}

}

namespace QStm {


} // namespace QStm
