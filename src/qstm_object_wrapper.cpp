#include "./qstm_object_wrapper.h"
#include "./qstm_util_variant.h"
#ifdef QT_GUI_LIB
#include "./qstm_meta_enum.h"
#endif
#include <QMetaProperty>
#include <QJsonDocument>
#include <QVariantHash>
#include <QMetaType>
#include <QFile>
#ifdef QT_GUI_LIB
#include <QFont>
#endif

namespace QStm {

#ifdef QT_GUI_LIB
static const auto __family="family";
static const auto __bold="bold";
static const auto __italic="italic";
static const auto __weight="weight";
static const auto __strikeOut="strikeOut";
static const auto __stretch="stretch";
static const auto __pixelSize="pixelSize";
static const auto __pointSize="pointSize";
static const auto __pointSizeF="pointSizeF";
#endif
typedef QVector<QByteArray> PropertyNames;
Q_GLOBAL_STATIC_WITH_ARGS(PropertyNames, staticIgnoreMethods,({"objectName","values","measures","asJson", "measures", "baseValues", "clearOnSetFail"}))

ObjectWrapper::ObjectWrapper(QObject *parent)
    : QObject{parent}
{
    QObject::connect(this, &ObjectWrapper::changed, this, [this](){this->_values.clear();});
}

void ObjectWrapper::printProperties()
{
    auto propList=this->toPropList();
    qInfo()<<"Properties of "<<this->metaObject()->className();
    for(auto &p:propList){
        qInfo()<<"  "<<p.name()<<"=="<<p.read(this);
    }
}

bool ObjectWrapper::isValid() const
{
    return true;
}

bool ObjectWrapper::readFrom(const QVariant &values)
{
    QVariantHash v=ObjectWrapper::parserVariant(values).toHash();
    return this->setValues(v);
}

void ObjectWrapper::readFrom(const QObject *object)
{
    this->setValues(QVariant::fromValue(object));
}

bool ObjectWrapper::mergeFrom(const QVariant &values)
{
    auto vHash=this->toHash();
    QVariantHash v=ObjectWrapper::parserVariant(values).toHash();
    QHashIterator<QString, QVariant> i(v);
    while(i.hasNext()){
        i.next();
        vHash.insert(i.key(), i.value());
    }
    return this->setValues(vHash);
}

bool ObjectWrapper::mergeFrom(const QObject *object)
{
    return this->mergeFrom(QVariant::fromValue(object));
}

void ObjectWrapper::clear()
{
    auto &metaObject = *this->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);

        if(!property.isWritable())
            continue;

        if(!property.isResettable())
            continue;

        auto pName=QByteArray{property.name()};

        if(staticIgnoreMethods->contains(pName))
            continue;

        auto obj=property.read(this).value<QObject *>();
        if(obj){
            obj->setProperty(QByteArrayLiteral("values"), {});
            continue;
        }

        property.reset(this);
    }
    emit cleared();
}

const QString ObjectWrapper::toJson()
{
    auto hash=this->toHash();
    if(hash.isEmpty())
        return {};
    return QJsonDocument::fromVariant(hash).toJson(QJsonDocument::Compact);
}

const QVariantHash ObjectWrapper::toHash()const
{
    auto __return=extractHash(this);
    return __return;
}

const QByteArray QStm::ObjectWrapper::toMd5()
{
    auto vHash=this->extractHash(this, this->__md5IgnoreProperties().toStringList());
    Q_DECLARE_VU;
    return vu.toMd5(vHash);
}

QVector<QMetaProperty> ObjectWrapper::toPropList() const
{
    return this->extractProperty(this);
}

QVariant ObjectWrapper::values() const
{
    return this->extractHash(this);
}

bool ObjectWrapper::setValues(const QVariant &v)
{
    QVariantHash vParser=ObjectWrapper::parserVariant(v).toHash();

    if(vParser.isEmpty()){
        this->clear();
        emit valuesChanged();
    }

    bool __return=false;
    auto propList=this->toPropList();

    auto getValue=[&vParser](const QByteArray &propertyName){
        if(vParser.contains(propertyName))
            return vParser.value(propertyName);

        auto pName=propertyName.toLower().trimmed();

        QHashIterator<QString, QVariant> i(vParser);
        while(i.hasNext()){
            i.next();
            auto key=i.key().toLower().trimmed();
            if(key==pName)
                return i.value();
        }
        return QVariant{};
    };


    for(auto &property:propList) {

        if(!property.isWritable())
            continue;

        auto value=getValue(property.name());

        if(!beforeSetProperty(property, value))
            continue;

        auto isObject=[&property](){
            const auto &metaType=property.metaType().metaObject();
            if(!metaType)
                return false;
            if(metaType->inherits(&ObjectWrapper::staticMetaObject))
                return true;
            return false;
        };


        if(!isObject()){
#ifdef QT_GUI_LIB
            switch(property.typeId()){
            case QMetaType::QFont:{
                auto vHash=value.toHash();
                QFont font;

                if(vHash.contains(__family))
                    font.setFamily(vHash.value(__family).toString());

                if(vHash.contains(__bold))
                    font.setBold(vHash.value(__bold).toBool());

                if(vHash.contains(__weight)){
                    MetaEnum<QFont::Weight> weight=vHash.value(__weight);
                    font.setWeight(weight.type());
                }

                if(vHash.contains(__strikeOut))
                    font.setStrikeOut(vHash.value(__strikeOut).toBool());

                if(vHash.contains(__stretch))
                    font.setStretch(vHash.value(__stretch).toReal());

                if(vHash.contains(__italic))
                    font.setItalic(vHash.value(__italic).toInt());

                if(vHash.contains(__pixelSize))
                    font.setPixelSize(vHash.value(__pixelSize).toInt());

                if(vHash.contains(__pixelSize))
                    font.setPixelSize(vHash.value(__pixelSize).toInt());

                if(vHash.contains(__pointSize))
                    font.setPointSize(vHash.value(__pointSize).toInt());

                if(vHash.contains(__pointSizeF))
                    font.setPointSizeF(vHash.value(__pointSizeF).toReal());

                value=QVariant::fromValue(font);
                break;
            }
            default:
                break;
            }
#endif
            switch(value.typeId()){
            case QMetaType::UnknownType:{
//                if(mergeValues)
//                    break;
                property.reset(this);
                break;
            }
            default:
                if(property.write(this, value))
                    __return=true;
                break;
            }
        }
        else{
            auto objReady=property.read(this).value<QObject *>();
            switch(value.typeId()){
            case QMetaType::UnknownType:
            {
                auto obj=dynamic_cast<ObjectWrapper*>(objReady);
                if(obj && this->_clearOnSetFail){
                    obj->clear();
                    break;
                }

                property.reset(this);
                break;
            }
            case QMetaType::QVariantHash:
            case QMetaType::QVariantMap:
            case QMetaType::QVariantList:
            case QMetaType::QStringList:
            {
                auto obj=dynamic_cast<ObjectWrapper*>(objReady);
                if(obj){
                    obj->setValues(value);
                    break;
                }
                break;
            }
            default:
                if(property.write(this, value))
                    __return=true;
            }
        }
        afterSetProperty(property, value);
    }

    if(__return)
        emit changed();
    emit valuesChanged();
    return __return;
}

bool ObjectWrapper::setValues(const QObject *v)
{
    if(!v){
        this->clear();
        return {};
    }
    return this->setValues(extractHash(v));
}

bool ObjectWrapper::beforeSetProperty(const QMetaProperty &property, QVariant &value)
{
    Q_UNUSED(property)
    Q_UNUSED(value)
    return true;
}

bool ObjectWrapper::afterSetProperty(const QMetaProperty &property, const QVariant &value)
{
    Q_UNUSED(property)
    Q_UNUSED(value)
    return true;
}

const QVariant ObjectWrapper::parserVariant(const QVariant &v)
{
    const auto o=v.value<QObject *>();
    if(o){
        auto v=ObjectWrapper::extractHash(o);
        return v;
    }

    switch (v.typeId()) {
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return v.toHash();
    case QMetaType::QVariantList:
    case QMetaType::QStringList:
        return v.toList();
    default:
    {
        {
            auto bytes=v.toByteArray().trimmed();
            if(!bytes.isEmpty()){
                {
                    if(bytes.at(0)=='{' || bytes.at(0)=='['){
                        return QJsonDocument::fromJson(bytes).toVariant();
                    }
                }
                QFile file(bytes);
                if(file.exists() && file.open(file.ReadOnly)){
                    auto bytes=file.readAll();
                    file.close();
                    return QJsonDocument::fromJson(bytes).toVariant();
                }
            }
        }

        {
            auto vOut=v.value<QObject *>();
            if(vOut)
                return QVariant::fromValue<QObject *>(vOut);
        }

        {
            auto vOut=v.toHash();
            if(!vOut.isEmpty())
                return vOut;
        }

        {
            auto vOut=v.toList();
            if(!vOut.isEmpty())
                return vOut;
        }
        return v;
    }
    }
}

const QVector<QMetaProperty> ObjectWrapper::extractProperty(const QObject *object)
{
    QVector<QMetaProperty> __return;

    auto &metaObject = *object->metaObject();
    for(int col = 0; col < metaObject.propertyCount(); ++col) {
        auto property = metaObject.property(col);
        auto name=QByteArray{property.name()};

        if(staticIgnoreMethods->contains(name))
            continue;

        __return.append(property);
    }
    return __return;
}

const QVariantHash ObjectWrapper::extractHash(const QObject *object, const QStringList &ignoreProperties)
{
    QVariantHash __return;
    auto metaObject = object->metaObject();

    for(int col = 0; col < metaObject->propertyCount(); ++col) {

        auto property = metaObject->property(col);

        auto name=QByteArray{property.name()};
        if(ignoreProperties.contains(name))
            continue;

        if(!property.isReadable())
            continue;

        if(staticIgnoreMethods->contains(name))
            continue;

        auto value=property.read(object);

        auto obj=value.value<QObject *>();
        if(obj)
            value = extractHash(obj);

        switch (value.typeId()) {
        case QMetaType::QUuid:
            value=value.toUuid().toString();
            break;
        case QMetaType::QUrl:
            value=value.toUrl().toString();
            break;
        default:
            break;
        }
        __return.insert(property.name(), value);
    }
    return __return;
}

} // namespace QStm
