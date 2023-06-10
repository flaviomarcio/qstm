#include "./qstm_cache_io.h"
#include "./qstm_macro.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTimer>
#include <QCryptographicHash>

namespace QStm {

static const auto __cache_io="cache-io";
static const auto __backup="backup";
static const auto __tree_md5="/tree-md5";

class CacheIOPvt:public QObject{
public:
    CacheIO *parent=nullptr;
    QString lastError, root, storageDir, storageTree, storageBackup;
    QVariant storage;
    explicit CacheIOPvt(CacheIO*parent):QObject{parent}
    {
        this->parent=parent;
        this->cachePrepare();
    }

    static QByteArray toMd5(const QByteArray &bytes)
    {
        return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
    }

    static QUuid toMd5Uuid(const QByteArray &bytes)
    {
        auto md5=toMd5(bytes);
        return QUuid::fromString(QStringLiteral("{")+md5+QStringLiteral("}"));
    }

    void cachePrepare()
    {
        auto storage=this->storage.toString().trimmed();
        this->storage=!storage.isEmpty()
                            ?storage:
                            QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        emit parent->afterPrepared();
    }

    const QString &getStorageDir()
    {
        auto storage=this->storage.toString().trimmed();
        auto root="/"+(this->root.isEmpty()?__cache_io:this->root);
        this->storageDir=storage+root;
        return this->storageDir;
    }

    const QString &getStorageBackup()
    {
        return (this->storageBackup=this->getStorageDir()+'/'+__backup);
    }

    const QString &getStorageTree()
    {
        return (this->storageTree=this->getStorageDir()+'/'+__tree_md5);
    }

    QString localPathFinal(const QUuid &md5File)
    {
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
        QString base=this->getStorageTree();
        base+=QStringLiteral("/")+md5.mid(0,1);
        base+=QStringLiteral("/")+md5.mid(1,1);
        base+=QStringLiteral("/")+md5.mid(2,2);
        base+=QStringLiteral("/")+md5.mid(4,2);
        return base;
    }

    QString storagePath(const QUuid &md5File)
    {
        if(md5File.isNull())
            return {};

        QDir dir(localPathFinal(md5File));
        if(!dir.exists())
            dir.mkpath(dir.path());

        static const auto __format=QStringLiteral("%1/%2");
        return __format.arg(dir.path(), md5File.toString());
    }

};

CacheIO::CacheIO(QObject *parent) : QObject{parent}
{
    this->p=new CacheIOPvt{this};
}

CacheIO::CacheIO(const QString &root, QObject *parent) : QObject{parent}
{
    this->p=new CacheIOPvt{this};
    this->p->root=root.trimmed();
}

QString &CacheIO::lastError() const
{
    return p->lastError;
}

bool CacheIO::backup()
{
    return true;
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

QVariant &CacheIO::storage()
{
    return p->storage;
}

CacheIO &CacheIO::storage(const QVariant &newStorage)
{
    p->storage=newStorage;
    p->cachePrepare();
    return *this;
}

const QString &CacheIO::storageBackup()
{
    return p->getStorageBackup();
}

const QString &CacheIO::storageTree()
{
    return p->getStorageTree();
}

bool CacheIO::exists(const QUuid &md5File)
{
    QFile file(p->storagePath(md5File));
    if(file.exists())
        return false;
    return true;
}

bool CacheIO::put(QUuid &outMD5, const QByteArray &bytes)
{
    outMD5={};
    auto md5File=p->toMd5Uuid(bytes);
    if(md5File.isNull()){
        sWarning()<<QStringLiteral("Invalid bytes");
        return false;
    }

    QFile file(p->storagePath(md5File));
    emit this->afterPut(md5File, QUrl::fromLocalFile(file.fileName()));

    if(!file.exists()){
        if(!file.open(QFile::Truncate | QFile::Unbuffered | QFile::WriteOnly)){
            sWarning()<<file.errorString();
            return false;
        }
        file.write(bytes);
        file.flush();
        file.close();
    }
    outMD5=md5File;
    return !outMD5.isNull();
}

bool CacheIO::put(QUuid &outMD5, QFile &srcFile)
{
    if(!srcFile.isOpen()){
        if(srcFile.open(QFile::Unbuffered | QFile::ReadOnly)){
            sWarning()<<srcFile.errorString();
            return false;
        }
    }
    auto bytes=srcFile.readAll();
    return this->put(outMD5, bytes);
}

bool CacheIO::get(const QUuid &md5File, QByteArray &outBytes)
{
    QFile file(p->storagePath(md5File));
    emit this->beforeGet(md5File, QUrl::fromLocalFile(file.fileName()));

    if(!file.exists()){
        sWarning()<<QStringLiteral("cache not found: %1").arg(file.fileName());
        return false;
    }

    if(!file.open(QFile::Unbuffered | QFile::ReadOnly)){
        sWarning()<<file.errorString();
        return false;
    }

    outBytes=file.readAll();
    file.close();
    return true;
}

bool CacheIO::rm(const QUuid &md5File)
{
    QFile file(p->storagePath(md5File));
    emit this->beforeRm(md5File, QUrl::fromLocalFile(file.fileName()));

    if(!file.exists()){
        sWarning()<<QStringLiteral("File not exists: %1").arg(file.fileName());
        return false;
    }

    file.remove();
    emit beforeRm(md5File, QUrl::fromLocalFile(file.fileName()));
    return true;
}

bool CacheIO::cp(const QUuid &md5File, const QString &destine)
{
    QFile file(p->storagePath(md5File));
    if(!file.exists()){
        sWarning()<<QStringLiteral("Invalid source: %1").arg(file.fileName());
        return false;
    }

    QDir dir(destine);
    if(!dir.exists()){
        dir.mkpath(destine);
        if(!dir.exists()){
            sWarning()<<QStringLiteral("Invalid destine: %1").arg(destine);
            return false;
        }
    }

    auto newFileName=QStringLiteral("%1/%2").arg(destine, md5File.toString());
    if(!file.copy(newFileName)){
        sWarning()<<file.errorString();
        return false;
    }

    emit beforeCp(md5File, QUrl::fromLocalFile(file.fileName()));
    return true;
}

QUrl CacheIO::url(const QUuid &md5File)
{
    auto fileName=p->storagePath(md5File);
    return QUrl::fromLocalFile(fileName);
}

} // namespace ServerService
