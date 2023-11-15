#include "./qstm_url.h"
#include "./qstm_result.h"
#include "./qstm_util_variant.h"
#include <QFile>

namespace QStm {

static const auto __QStm__Url="__QStm__Url";
static const auto __http="http";
static const auto __name="name";
static const auto __outPutName="outPutName";
static const auto __outputname="outputname";
static const auto __url="url";
static const auto __headers="__headers";

class UrlPvt{
public:
    Url*parent=nullptr;
    Q_DECLARE_VU;
    QString name;
    QString outPutName;
    QVariantHash body;
    QVariantHash headers;
    explicit UrlPvt(Url*parent):parent{parent}
    {
    }

    virtual ~UrlPvt(){
    }

    QUrl url()
    {
        if(!this->body.contains(__url))
            return {};
        return this->body.value(__url).toUrl();
    }

    Url &setData(){
        auto &vBody=this->body;
        vBody.clear();
        if(!this->name.isEmpty())
            vBody.insert(__name,this->name);
        if(!this->outPutName.isEmpty())
            vBody.insert(__outPutName,this->name);
        if(this->url().isValid() && !this->url().isEmpty())
            vBody.insert(__url,this->url());
        if(!this->headers.isEmpty())
            vBody.insert(__headers, this->headers);
        this->parent->setValue(vBody.isEmpty()?vBody:QVariant{});
        return *this->parent;
    }

    void setVar(const QVariant &v){
        if(!v.isValid() || v.isNull()){
            this->body.clear();
            this->headers.clear();
            this->setData();
            return;
        }
        if(v.typeId()==QMetaType::QUrl){
            this->body.insert(__url,v.toUrl());
            this->setData();
            return;
        }
        this->body={};
        auto vMap=this->vu.toHash(v);
        if(!vMap.contains(__QStm__Url))
            return;

        this->name=vMap.value(__name).toString().trimmed();
        this->outPutName=vMap.value(__outPutName).toString().trimmed();
        if(this->outPutName.isEmpty())
            this->outPutName=vMap.value(__outputname).toString().trimmed();
        if(vMap.contains(__url)){
            this->body.insert(__url,this->vu.toUrl(__url));
            if(vMap.contains(__headers))
                this->headers=this->vu.toHash(vMap.value(__headers));
        }
        else if(v.toString().toLower().startsWith(__http))
            this->body.insert(__url, this->vu.toUrl(v));
        this->setData();
    }

};

Url::Url(const QVariant &v):QVariant{},p{new UrlPvt{this}}
{
    p->setVar(v);
}

Url::Url(const ResultValue *v):QVariant{},p{new UrlPvt{this}}
{
    if(v)
        p->setVar(v->resultHash());
}

Url::~Url()
{
    delete p;
}

Url &Url::operator=(const QVariant &v)
{

    p->setVar(v);
    return *this;
}

Url Url::from(const QVariant &v)
{
    Url url(v);
    return url;
}

Url &Url::read(const QVariant &v)
{

    p->setVar(v);
    return *this;
}

QVariantHash &Url::headers() const
{

    return p->headers;
}

Url &Url::header(const QVariant &value)
{

    p->headers=p->vu.toHash(value);
    return p->setData();
}

Url &Url::setHeader(const QVariant &value)
{

    p->headers=p->vu.toHash(value);
    return p->setData();
}

QString &Url::name() const
{

    return p->name;
}

Url &Url::name(const QVariant &value)
{

    p->name=value.toString();
    return p->setData();
}

Url &Url::setName(const QVariant &value)
{

    p->name=value.toString();
    return p->setData();
}

QString &Url::outPutName() const
{

    return p->outPutName;
}

Url &Url::outPutName(const QVariant &value)
{

    p->outPutName=value.toString();
    return p->setData();
}

Url &Url::setOutPutName(const QVariant &value)
{

    p->outPutName=value.toString();
    return p->setData();
}

QUrl Url::url() const
{
    return p->url();
}

Url &Url::url(const QVariant &value)
{
    p->body.insert(__url, p->vu.toUrl(value));
    return p->setData();
}

Url&Url::setUrl(const QVariant &value)
{
    return this->url(value);
}

bool Url::isValid() const
{
    return p->url().isValid() && p->url().isEmpty();
}

bool Url::isLocalFile() const
{
    return p->url().isLocalFile();
}

QString Url::toLocalFile() const
{
    return p->url().toLocalFile();
}

Url Url::fromLocalFile(const QString &localfile)
{
    return Url::from(QUrl::fromLocalFile(localfile));
}

QVariantHash Url::toHash() const
{
    auto vHash=p->body;
    vHash.insert(__QStm__Url,true);
    return vHash;
}

QByteArray Url::readBody() const
{
    if(!this->isLocalFile())
        return {};
    QByteArray __return;
    QFile file(this->url().toLocalFile());
    if(!file.open(file.ReadOnly))
        return {};
    __return=file.readAll();
    file.close();
    return __return;
}

QString Url::toString() const
{
    return p->url().toString();
}

}
