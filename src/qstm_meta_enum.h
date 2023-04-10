#pragma once

#include "./qstm_global.h"
#include <QMetaEnum>
#include <QVariant>

namespace QStm {

//!
//! \brief The MetaEnum class
//!
template <typename ENUM>
class Q_STM_EXPORT MetaEnum
{
public:
    //!
    //! \brief MetaEnum
    //!
    MetaEnum()
    {
        this->_type={};
    }

    //!
    //! \brief MetaEnum
    //! \param value
    //!
    MetaEnum(const ENUM &value)
    {
        this->_type=value;
    }

    //!
    //! \brief MetaEnum
    //! \param value
    //!
    MetaEnum(const QVariant &value)
    {
        this->_type=this->type(value);
    }

    //!
    //! \brief operator =
    //! \param value
    //! \return
    //!
    ENUM operator = (const QVariant &value)
    {
        this->_type=this->type(value);
        return this->_type;
    }

    ENUM operator = (const ENUM &value)
    {
        this->_type=value;
        return this->_type;
    }

    //!
    //! \brief operator ==
    //! \param value
    //! \return
    //!
    bool operator == (const QVariant &value)
    {
        return this->_type==this->type(value);
    }

    //!
    //! \brief operator ==
    //! \param value
    //! \return
    //!
    bool operator == (const ENUM &value)
    {
        return this->_type==value;
    }

    //!
    //! \brief operator !=
    //! \param value
    //! \return
    //!
    bool operator != (const QVariant &value)
    {
        return this->_type!=this->type(value);
    }

    //!
    //! \brief operator !=
    //! \param value
    //! \return
    //!
    bool operator != (const ENUM &value)
    {
        return this->_type!=this->type(value);
    }

    //!
    //! \brief type
    //! \return
    //!
    ENUM type()const
    {
        return this->_type;
    }

    //!
    //! \brief equal
    //! \param v
    //! \return
    //!
    bool equal(const QVariant &v)const
    {
        ENUM type=this->type(v);
        return this->_type==type;
    }

    //!
    //! \brief equal
    //! \param type
    //! \return
    //!
    bool equal(const ENUM &type)const
    {
        return this->_type==type;
    }

    //!
    //! \brief name
    //! \return
    //!
    QByteArray name()const
    {
        auto ivalue=static_cast<int>(this->_type);
        return QByteArray{metaEnum.valueToKey(ivalue)};
    }

    //!
    //! \brief type
    //! \param value
    //! \return
    //!
    ENUM type(const QVariant &value)const
    {
        if(metaEnum.keyCount()==0){
            bool ok;
            auto i=value.toInt(&ok);
            if(ok)
                return ENUM(i);
            return ENUM(-1);
        }

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
        return this->_typeDefault;
    }

    //!
    //! \brief canType
    //! \param v
    //! \return
    //!
    bool canType(const QVariant &v){

        bool enumNumberOK=false;
        int enumCastNumber=v.toInt(&enumNumberOK);
        if(enumNumberOK){
            for(int i=0; i< metaEnum.keyCount(); ++i){
                auto v=QByteArray{metaEnum.key(i)};
                auto enumValue=metaEnum.keyToValue(v);
                if(enumValue==enumCastNumber){
                    this->_type=ENUM(enumCastNumber);
                    return true;
                }
            }
        }
        else{
            auto uName=v.toByteArray().trimmed().toLower();
            for(int i=0; i< metaEnum.keyCount(); ++i){
                auto v=QByteArray{metaEnum.key(i)};
                if(v.toLower()==uName){
                    this->_type=ENUM(metaEnum.keyToValue(v));
                    return true;
                }
            }
        }
        this->_type=_typeDefault;
        return false;
    }

    //!
    //! \brief contains
    //! \param value
    //! \return
    //!
    bool contains(const QVariant &value)const
    {
        return this->type(value)>=this->_typeDefault;
    }

private:
    QMetaEnum metaEnum=QMetaEnum::fromType<ENUM>();
    const ENUM _typeDefault=ENUM(metaEnum.keyToValue(metaEnum.key(0)));
    ENUM _type=_typeDefault;
};

} // namespace QMFE
