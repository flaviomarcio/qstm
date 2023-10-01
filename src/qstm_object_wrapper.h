#pragma once

#include <QObject>
#include <QVariant>
#include "./qstm_global.h"

namespace QStm {

#define Q_STM_OBJECT_WRAPPER_MD5_IGNORE(IGNORE)\
public:\
virtual QVariant __md5IgnoreProperties(){return QVariant{IGNORE};}


#define Q_STM_OBJECT_WRAPPER(CLASS)\
public:\
void operator=(const CLASS *object)\
{\
    ObjectWrapper::setValues(object);\
}\
Q_INVOKABLE void operator=(const QVariant &object)\
{\
    ObjectWrapper::setValues(object);\
}\
static CLASS *from(const QVariant &v, QObject *parent)\
{\
    auto item = new CLASS{parent};\
    (*item)=v;\
    if(!item->isValid()){\
        delete item;\
        return nullptr;\
    }\
    return item;\
}

class Q_STM_EXPORT ObjectWrapper : public QObject
{
    Q_OBJECT
    Q_STM_OBJECT_WRAPPER(ObjectWrapper)
    Q_STM_OBJECT_WRAPPER_MD5_IGNORE()
public:
    //!
    //! \brief ObjectWrapper
    //! \param parent
    //!
    Q_INVOKABLE explicit ObjectWrapper(QObject *parent = nullptr);

    //!
    //! \brief printProperties
    //!
    Q_INVOKABLE void printProperties();

    //!
    //! \brief isValid
    //! \return
    //!
    virtual bool isValid()const;

    //!
    //! \brief readFom
    //! \return
    //!
    Q_INVOKABLE virtual bool readFrom(const QVariant &values);

    //!
    //! \brief readFom
    //! \return
    //!
    virtual void readFrom(const QObject *object);

    //!
    //! \brief mergeFrom
    //! \return
    //!
    Q_INVOKABLE virtual bool mergeFrom(const QVariant &values);

    //!
    //! \brief mergeFrom
    //! \return
    //!
    virtual bool mergeFrom(const QObject *object);

    //!
    //! \brief clear
    //!
    Q_INVOKABLE virtual void clear();

    //!
    //! \brief toJson
    //! \return
    //!
    Q_INVOKABLE virtual const QString toJson();

    //!
    //! \brief toHash
    //! \return
    //!
    Q_INVOKABLE const virtual QVariantHash toHash()const;

    //!
    //! \brief toMd5
    //! \return
    //!
    Q_INVOKABLE const virtual QByteArray toMd5();

    //!
    //! \brief toPropList
    //! \return
    //!
    QVector<QMetaProperty> toPropList()const;

    //!
    //! \brief values
    //! \return
    //!
    QVariant values()const;
    virtual bool setValues(const QVariant &v);
    virtual bool setValues(const QObject *v);

    //!
    //! \brief baseValues
    //! \return
    //!
    QObject *baseValues() const;
    void setBaseValues(QObject *newBaseValues);
    void resetBaseValues();

    //!
    //! \brief parserVariant
    //! \param v
    //! \return
    //!
    static const QVariant parserVariant(const QVariant &v);

    //!
    //! \brief extractProperty
    //! \param object
    //! \return
    //!
    static const QVector<QMetaProperty> extractProperty(const QObject *object);

    //!
    //! \brief extractHash
    //! \param object
    //! \return
    //!
    static const QVariantHash extractHash(const QObject *object, const QStringList &ignoreProperties={});

protected:

    //!
    //! \brief beforeSetProperty
    //! \param property
    //! \param value
    //! \return
    //!
    virtual bool beforeSetProperty(const QMetaProperty &property, QVariant &value);

    //!
    //! \brief afterSetProperty
    //! \param property
    //! \param value
    //! \return
    //!
    virtual bool afterSetProperty(const QMetaProperty &property, const QVariant &value);


private:
    bool _clearOnSetFail=true;
    QVariant _values={};

signals:
    void changed();
    void cleared();
    void asJsonChanged();
    void valuesChanged();
    void clearOnSetFailChanged();
    void baseValuesChanged();
};

} // namespace QStm
