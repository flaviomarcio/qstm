#include "./qstm_cache_io.h"
#include "./qstm_util_variant.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTimer>
//#include <Lib7z>

namespace QStm {

class CacheIOPvt:public QObject{
public:
    CacheIO *parent=nullptr;
    QString root, path;
    explicit CacheIOPvt(CacheIO*parent):QObject{parent}
    {
        this->parent=parent;
        parent->beforePrepare();
        this->cachePrepare();
        parent->afterPrepare();
    }

    ResultValue &backup()
    {
        static const auto __format=QStringLiteral("%1/backup-%2.zip");
        auto fileName=__format.arg(this->storagePath(), QDateTime::currentDateTime().toString(Qt::ISODateWithMs));

        return this->parent->lr(QUrl::fromLocalFile(fileName));
    }

    void cachePrepare()
    {
        this->path=!this->path.isEmpty()?path:QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        this->path+=(this->root.isEmpty()?"":"/"+root);
        this->path=(this->path+QStringLiteral("/tree-md5"));
    }

    QString &storagePath()
    {
        return this->path;
    }

    QString storagePath(const QUuid &md5File)
    {
        if(md5File.isNull())
            return {};

        Q_DECLARE_VU;
        QDir dir(localPathFinal(md5File));
        if(!dir.exists())
            dir.mkpath(dir.path());

        static const auto __format=QStringLiteral("%1/%2");
        return __format.arg(dir.path(), md5File.toString());
    }

    QString localPathFinal(const QUuid &md5File)
    {
        Q_DECLARE_VU;
        if(md5File.isNull())
            return {};
        QString md5=md5File.toString()
                          .replace("{","")
                          .replace("}","")
                          .replace("[","")
                          .replace("]","")
                          .replace(".","")
                          .replace(".","")
                          .replace("-","");
        QString base=this->storagePath();
        base+=QStringLiteral("/")+md5.mid(0,1);
        base+=QStringLiteral("/")+md5.mid(1,1);
        base+=QStringLiteral("/")+md5.mid(2,2);
        base+=QStringLiteral("/")+md5.mid(4,2);
        return base;
    }
};

CacheIO::CacheIO(QObject *parent) : QStm::Object{parent}
{
    this->p=new CacheIOPvt{this};
    p->cachePrepare();
}

CacheIO::CacheIO(const QString &root, QObject *parent) : QStm::Object{parent}
{
    this->p=new CacheIOPvt{this};
    this->p->root=root.trimmed();
    p->cachePrepare();
}

ResultValue &CacheIO::backup()
{
    return this->lr(p->backup());
}

QString &CacheIO::rootName()
{
    return p->root;
}

CacheIO &CacheIO::rootName(const QString &newRootName)
{
    p->root=newRootName.trimmed();
    p->cachePrepare();
    return *this;
}

QString &CacheIO::storagePath()
{
    return p->storagePath();
}

CacheIO &CacheIO::storagePath(const QString &newStoragePath)
{
    p->path=newStoragePath.trimmed();
    p->cachePrepare();
    return *this;
}

bool CacheIO::exists(const QUuid &md5File)
{
    QFile file(p->storagePath(md5File));
    if(file.exists())
        return false;
    return true;
}

ResultValue &CacheIO::put(const QByteArray &bytes)
{
    Q_DECLARE_VU;
    auto md5File=vu.toMd5Uuid(bytes);
    if(md5File.isNull())
        return this->lr().setBadRequest();

    QFile file(p->storagePath(md5File));

    if(!this->beforePut(md5File, QUrl::fromLocalFile(file.fileName()))){
        if(this->lr().isNotOk())
            return this->lr();
        return this->lr().setCritical(QStringLiteral("beforeGet fail: %1").arg(file.fileName()));
    }

    if(!file.exists()){
        if(!file.open(QFile::Truncate | QFile::Unbuffered | QFile::WriteOnly))
            return this->lr().setCritical(file.errorString());

        file.write(bytes);
        file.flush();
        file.close();
    }

    if(!this->afterPut(md5File, QUrl::fromLocalFile(file.fileName()))){
        if(this->lr().isNotOk())
            return this->lr();
        return this->lr().setCritical(QStringLiteral("beforeGet fail: %1").arg(file.fileName()));
    }
    emit this->cachePutted(md5File, QUrl::fromLocalFile(file.fileName()));
    return this->lr(md5File);
}

ResultValue &CacheIO::put(QFile &srcFile)
{
    if(!srcFile.isOpen()){
        if(srcFile.open(QFile::Unbuffered | QFile::ReadOnly))
            return this->lr().setCritical(srcFile.errorString());
    }

    auto bytes=srcFile.readAll();
    return this->put(bytes);
}

ResultValue &CacheIO::get(const QUuid &md5File)
{
    QFile file(p->storagePath(md5File));

    if(!this->beforeGet(md5File, QUrl::fromLocalFile(file.fileName()))){
        if(this->lr().isNotOk())
            return this->lr();
        return this->lr().setCritical(QStringLiteral("beforeGet fail: %1").arg(file.fileName()));
    }

    if(!file.exists())
        return this->lr().setNotFound();

    if(!file.open(QFile::Unbuffered | QFile::ReadOnly))
        return this->lr().setCritical(file.errorString());

    auto bytes=file.readAll();
    file.close();
    emit this->cacheGetted(md5File, QUrl::fromLocalFile(file.fileName()));
    return this->lr(bytes);
}

bool CacheIO::rm(const QUuid &md5File)
{
    QFile file(p->storagePath(md5File));
    if(!this->beforeRm(md5File, QUrl::fromLocalFile(file.fileName()))){
        if(this->lr().isNotOk())
            return this->lr();
        return this->lr().setCritical(QStringLiteral("beforeRm fail: %1").arg(file.fileName()));
    }

    if(!file.exists())
        return this->lr();

    file.remove();
    emit this->cacheRemoved(md5File, QUrl::fromLocalFile(file.fileName()));
    return this->lr();
}

bool CacheIO::cp(const QUuid &md5File, const QString &destineDir)
{
    Q_DECLARE_VU;
    QFile file(p->storagePath(md5File));
    if(!file.exists())
        return this->lr().setNotFound(QStringLiteral("Invalid source: %1").arg(file.fileName()));

    QDir dir(destineDir);
    if(!dir.exists()){
        dir.mkpath(destineDir);
        if(!dir.exists())
            return this->lr().setNotFound(QStringLiteral("Invalid destine: %1").arg(destineDir));
    }

    auto newFileName=QStringLiteral("%1/%2").arg(destineDir, md5File.toString());
    if(!file.copy(newFileName))
        return this->lr().setCritical(file.errorString());

    emit this->cacheCopied(md5File, QUrl::fromLocalFile(file.fileName()));
    return this->lr();
}

QUrl CacheIO::url(const QUuid &md5File)
{
    auto fileName=p->storagePath(md5File);
    return QUrl::fromLocalFile(fileName);
}

void CacheIO::beforePrepare()
{

}

void CacheIO::afterPrepare()
{
}

ResultValue &CacheIO::beforePut(const QUuid &md5File, const QUrl &urlFile)
{
    Q_UNUSED(md5File)
    Q_UNUSED(urlFile)
    return this->lr();
}

ResultValue &CacheIO::afterPut(const QUuid &md5File, const QUrl &urlFile)
{
    Q_UNUSED(md5File)
    Q_UNUSED(urlFile)
    return this->lr();
}

ResultValue &CacheIO::beforeGet(const QUuid &md5File, const QUrl &urlFile)
{
    Q_UNUSED(md5File)
    Q_UNUSED(urlFile)
    return this->lr();
}

ResultValue &CacheIO::beforeRm(const QUuid &md5File, const QUrl &urlFile)
{
    Q_UNUSED(md5File)
    Q_UNUSED(urlFile)
    return this->lr();
}

ResultValue &CacheIO::afterBackup(const QUrl &backupFile)
{
    Q_UNUSED(backupFile)
    return this->lr();
}

ResultValue &CacheIO::beforeBackup(const QUrl &backupFile)
{
    Q_UNUSED(backupFile)
    return this->lr();
}

} // namespace ServerService
