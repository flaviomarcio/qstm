#include "./qstm_cache_io.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTimer>
#include <QCryptographicHash>

#include "./qstm_macro.h"
#include "./qstm_util_hash.h"

namespace QStm {

static const auto __cache_io="cache-io";
static const auto __backup="backup";
static const auto __tree_md5="/tree-md5";

class CacheIOPvt:public QObject{
public:
    CacheIO *parent=nullptr;
    QString lastError, rootName, scopeName, storageDir, storageTree, storageBackup;
    QString storage;
    explicit CacheIOPvt(CacheIO*parent):QObject{parent}, parent{parent}
    {
        this->readParent();
        this->cachePrepare();
    }

    void readParent()
    {
        if(!this->parent || !this->parent->parent())
            return;
        auto cacheParent=dynamic_cast<CacheIO*>(this->parent->parent());
        if(!cacheParent)
            return;

        this->rootName=cacheParent->rootName().trimmed();
        this->scopeName=cacheParent->scopeName().trimmed();
        this->storageDir=cacheParent->storage().trimmed();

    }

    void cachePrepare()
    {
        auto storage=this->storage;
        this->storage=!storage.isEmpty()
                            ?storage:
                            QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        emit parent->afterPrepared();
    }

    const QString &getStorageDir()
    {
        auto &storage=this->storageDir;
        storage=this->storage.trimmed();
        storage+='/'+(this->rootName.isEmpty()?__cache_io:this->rootName);
        storage+=(this->scopeName.isEmpty()?"":'/'+this->scopeName);
        return storage;
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

CacheIO::CacheIO(QObject *parent) : QObject{parent}, p{new CacheIOPvt{this}}
{
}

CacheIO::CacheIO(const QString &root, QObject *parent) : QObject{parent}, p{new CacheIOPvt{this}}
{
    this->p->rootName=root.trimmed();
}

CacheIO::CacheIO(CacheIO &parent) : QObject{&parent}, p{new CacheIOPvt{this}}
{

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
    return p->rootName;
}

CacheIO &CacheIO::rootName(const QString &newRootName)
{
    p->rootName=newRootName.trimmed();
    p->cachePrepare();
    return *this;
}

QString &CacheIO::scopeName()
{
    return p->scopeName;
}

CacheIO &CacheIO::scopeName(const QString &newScopeName)
{
    p->scopeName=newScopeName.trimmed();
    p->cachePrepare();
    return *this;
}

QString &CacheIO::storage()const
{
    return p->storage;
}

CacheIO &CacheIO::storage(const QVariant &newStorage)
{
    p->storage=newStorage.toString().trimmed();
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
    Q_DECLARE_HU;
    auto md5File=hu.toMd5Uuid(bytes);
    if(md5File.isNull()){
        sWarning()<<(p->lastError=QStringLiteral("Invalid bytes"));
        return false;
    }

    QFile file(p->storagePath(md5File));
    emit this->afterPut(md5File, QUrl::fromLocalFile(file.fileName()));

    if(!file.exists()){
        if(!file.open(QFile::Truncate | QFile::Unbuffered | QFile::WriteOnly)){
            sWarning()<<(p->lastError=file.errorString());
            return false;
        }
        file.write(bytes);
        file.flush();
        file.close();
    }
    outMD5=md5File;
    if(outMD5.isNull()){
        sWarning()<<(p->lastError=QStringLiteral("Invalid outMD5 md5"));
        return false;
    }
    return !outMD5.isNull();
}

bool CacheIO::put(QUuid &outMD5, QFile &srcFile)
{
    if(!srcFile.isOpen()){
        if(srcFile.open(QFile::Unbuffered | QFile::ReadOnly)){
            sWarning()<<(p->lastError=srcFile.errorString());
            return false;
        }
    }
    auto bytes=srcFile.readAll();
    return this->put(outMD5, bytes);
}

bool CacheIO::put(const QByteArray &bytes)
{
    QUuid outMD5;
    return this->put(outMD5, bytes);
}

bool CacheIO::put(QFile &srcFile)
{
    QUuid outMD5;
    return this->put(outMD5, srcFile);
}

bool CacheIO::get(const QUuid &md5File, QByteArray &outBytes)
{
    QFile file(p->storagePath(md5File));
    emit this->beforeGet(md5File, QUrl::fromLocalFile(file.fileName()));

    if(!file.exists()){
        sWarning()<<(p->lastError=QStringLiteral("cache not found: %1").arg(file.fileName()));
        return false;
    }

    if(!file.open(QFile::Unbuffered | QFile::ReadOnly)){
        sWarning()<<(p->lastError=file.errorString());
        return false;
    }

    outBytes=file.readAll();
    file.close();
    return true;
}

bool CacheIO::get(const QUuid &md5File, QUrl &outFile)
{
    QFile file(p->storagePath(md5File));
    emit this->beforeGet(md5File, QUrl::fromLocalFile(file.fileName()));

    if(!file.exists()){
        sWarning()<<(p->lastError=QStringLiteral("cache not found: %1").arg(file.fileName()));
        return false;
    }
    outFile=QUrl::fromLocalFile(file.fileName());
    return true;
}

bool CacheIO::rm(const QUuid &md5File)
{
    QFile file(p->storagePath(md5File));
    emit this->beforeRm(md5File, QUrl::fromLocalFile(file.fileName()));

    if(!file.exists()){
        sWarning()<<(p->lastError=QStringLiteral("File not exists: %1").arg(file.fileName()));
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
        sWarning()<<(p->lastError=QStringLiteral("Invalid source: %1").arg(file.fileName()));
        return false;
    }

    QDir dir(destine);
    if(!dir.exists()){
        dir.mkpath(destine);
        if(!dir.exists()){
            sWarning()<<(p->lastError=QStringLiteral("Invalid destine: %1").arg(destine));
            return false;
        }
    }

    auto newFileName=QStringLiteral("%1/%2").arg(destine, md5File.toString());
    if(!file.copy(newFileName)){
        sWarning()<<(p->lastError=file.errorString());
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
