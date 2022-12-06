#pragma once

#include "./qstm_global.h"
#include <QMetaEnum>
#include <QVariant>

namespace QStm {

template <typename ENUM>
class Q_STM_EXPORT MetaEnum
{
public:
    MetaEnum()
    {
        this->_type={};
    }

    MetaEnum(const ENUM &value)
    {
        this->_type=value;
    }

    MetaEnum(const QVariant &value)
    {
        this->_type=this->type(value);
    }

    ENUM operator = (const ENUM &value)
    {
        this->_type=value;
        return this->_type;
    }

    ENUM operator = (const QVariant &value)
    {
        this->_type=this->type(value);
        return this->_type;
    }

    bool operator == (const QVariant &value)
    {
        return this->_type==this->type(value);
    }

    bool operator != (const QVariant &value)
    {
        return this->_type!=this->type(value);
    }

    ENUM type()const
    {
        return this->_type;
    }

    bool equal(const QVariant &v)const
    {
        ENUM type=this->type(v);
        return this->_type==type;
    }

    QByteArray name()const
    {
        return QByteArray{metaEnum.valueToKey(this->_type)};
    }

    ENUM type(const QVariant &value)const
    {
        if(metaEnum.keyCount()==0)
            return ENUM(-1);

        switch (value.typeId()) {
        case QMetaType::Long:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Double:
        {
            auto index=value.toInt();
            auto t =metaEnum.valueToKey(index);
            if(t)
                return ENUM(metaEnum.keyToValue(t));

            for(int i=0; i< metaEnum.keyCount(); ++i){
                auto v=metaEnum.value(i);
                if(v==index)
                    return ENUM(v);
            }
            break;
        }
        case QMetaType::QString:
        case QMetaType::QByteArray:
        {
            auto uName=value.toByteArray().trimmed();
            auto t =metaEnum.keyToValue(uName);
            if(t>=0)
                return ENUM(t);
            for(int i=0; i< metaEnum.keyCount(); ++i){
                auto v=QByteArray{metaEnum.key(i)};
                if(v.toLower()==uName){
                    return ENUM(metaEnum.keyToValue(v));
                }
            }
            break;
        }
        default:
            break;
        }
        return ENUM(this->metaEnum.value(0));
    }

    bool contains(const QVariant &value)const
    {
        return this->type(value)>=0;
    }

private:
    QMetaEnum metaEnum=QMetaEnum::fromType<ENUM>();
    ENUM _type=ENUM(this->metaEnum.value(0));;
};

} // namespace QMFE
