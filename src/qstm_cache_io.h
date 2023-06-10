#pragma once

#include "./qstm_global.h"
#include <QUrl>
#include <QUuid>
#include <QFile>

namespace QStm {

class CacheIOPvt;

//!
//! \brief The CacheIO class
//!
class Q_STM_EXPORT CacheIO : public QObject
{
    Q_OBJECT
public:
    explicit CacheIO(QObject *parent = nullptr);
    explicit CacheIO(const QString &root, QObject *parent = nullptr);

    //!
    //! \brief lastError
    //! \return
    //!
    QString &lastError()const;

    //!
    //! \brief backup
    //! \return
    //!
    virtual bool backup();

    //!
    //! \brief root
    //! \return
    //!
    virtual QString &rootName();
    virtual CacheIO &rootName(const QString &newRootName);

    //!
    //! \brief storagePath
    //! \return
    //!
    virtual QVariant &storage();
    virtual CacheIO &storage(const QVariant &newStorage);

    //!
    //! \brief storageBackup
    //! \return
    //!
    virtual const QString &storageBackup();

    //!
    //! \brief storageTree
    //! \return
    //!
    virtual const QString &storageTree();

    //!
    //! \brief exists
    //! \param mdhFile
    //! \return
    //!
    virtual bool exists(const QUuid &md5File);

    //!
    //! \brief save
    //! \param bytes
    //! \return
    //!
    virtual bool put(QUuid &outMD5, const QByteArray &bytes);
    virtual bool put(QUuid &outMD5, QFile &srcFile);

    //!
    //! \brief get
    //! \param mdhFile
    //! \return
    //!

    virtual bool get(const QUuid &md5File, QByteArray &outBytes);
    //!
    //! \brief rm
    //! \param srcFile
    //! \return
    //!
    virtual bool rm(const QUuid &md5File);

    //!
    //! \brief cp
    //! \param md5File
    //! \param destine
    //! \return
    //!
    virtual bool cp(const QUuid &md5File, const QString &destine);

    //!
    //! \brief url
    //! \param md5File
    //! \return
    //!
    virtual QUrl url(const QUuid &md5File);

private:
    CacheIOPvt *p=nullptr;
signals:
    void afterPrepared();
    void afterPut(const QUuid &md5File, const QUrl &urlFile);
    void beforeGet(const QUuid &md5File, const QUrl &urlFile);
    void beforeRm(const QUuid &md5File, const QUrl &urlFile);
    void beforeCp(const QUuid &md5File, const QUrl &urlFile);
    void beforeBackup(const QUrl &backupFile, QUrl &destineFile);
};

} // namespace ServerService
