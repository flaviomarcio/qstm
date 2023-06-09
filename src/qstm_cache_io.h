#pragma once

#include "./qstm_global.h"
#include "./qstm_object.h"
#include "./qstm_result.h"
#include <QUrl>
#include <QUuid>
#include <QFile>

namespace QStm {

class CacheIOPvt;

//!
//! \brief The CacheIO class
//!
class Q_STM_EXPORT CacheIO : public QStm::Object
{
    Q_OBJECT
public:
    explicit CacheIO(QObject *parent = nullptr);
    explicit CacheIO(const QString &root, QObject *parent = nullptr);

    //!
    //! \brief backup
    //! \return
    //!
    virtual ResultValue &backup();

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
    virtual QString &storagePath();
    virtual CacheIO &storagePath(const QString &newStoragePath);

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
    virtual ResultValue &put(const QByteArray &bytes);
    virtual ResultValue &put(QFile &srcFile);

    //!
    //! \brief get
    //! \param mdhFile
    //! \return
    //!
    virtual ResultValue &get(const QUuid &md5File);

    //!
    //! \brief rm
    //! \param srcFile
    //! \return
    //!
    virtual bool rm(const QUuid &md5File);

    //!
    //! \brief cp
    //! \param md5File
    //! \param destineDir
    //! \return
    //!
    virtual bool cp(const QUuid &md5File, const QString &destineDir);

    //!
    //! \brief urlFile
    //! \param md5File
    //! \return
    //!
    virtual QUrl url(const QUuid &md5File);

    //!
    //! \brief beforePrepare
    //! \return
    //!
    virtual void beforePrepare();

    //!
    //! \brief afterPrepare
    //! \return
    //!
    virtual void afterPrepare();

    //!
    //! \brief beforePut
    //! \param md5File
    //! \param urlFile
    //! \return
    //!
    virtual ResultValue &beforePut(const QUuid &md5File, const QUrl &urlFile);

    //!
    //! \brief afterPut
    //! \param md5File
    //! \param urlFile
    //! \return
    //!
    virtual ResultValue &afterPut(const QUuid &md5File, const QUrl &urlFile);

    //!
    //! \brief beforeGet
    //! \param md5File
    //! \param urlFile
    //! \return
    //!
    virtual ResultValue &beforeGet(const QUuid &md5File, const QUrl &urlFile);

    //!
    //! \brief beforeRm
    //! \param md5File
    //! \param urlFile
    //! \return
    //!
    virtual ResultValue &beforeRm(const QUuid &md5File, const QUrl &urlFile);

    //!
    //! \brief afterBackup
    //! \param backupFile
    //! \return
    //!
    virtual ResultValue &afterBackup(const QUrl &backupFile);

    //!
    //! \brief beforeBackup
    //! \param backupFile
    //! \return
    //!
    virtual ResultValue &beforeBackup(const QUrl &backupFile);
private:
    CacheIOPvt *p=nullptr;
signals:
    void cachePutted(const QUuid &md5File, const QUrl &urlFile);
    void cacheGetted(const QUuid &md5File, const QUrl &urlFile);
    void cacheRemoved(const QUuid &md5File, const QUrl &urlFile);
    void cacheCopied(const QUuid &md5File, const QUrl &urlFile);
};

} // namespace ServerService
