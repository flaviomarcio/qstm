#include "./qstm_crypto_util.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#ifdef QTREFORCE_QTINIAES
#include "./qtinyaes.h"
#endif

namespace QStm {

#ifdef QTREFORCE_QTINIAES
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, key_256bit, ("t6w9z$C&F)J@NcRfUjXn2r5u7x!A%D*G"))
//Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, hash_key_256bit, ())
//Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, key_128bit, ("Xp2s5v8y/B?E(G+K"))
//Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, hash_key_128bit, ())
#endif

class CryptoUtilPvt:public QObject
{
public:
#ifdef QTREFORCE_QTINIAES
    QTinyAes aes;
#endif
    explicit CryptoUtilPvt(QObject *parent):QObject{parent}
    {
#ifdef QTREFORCE_QTINIAES
        aes.setMode(QTinyAes::CTR);
        aes.setKey(*key_256bit);// QTinyAes::KeySize (256 bit key by default)
        //aes.setIv(*key_128bit);// QTinyAes::BlockSize (128 iv vector)
#endif
    }
};



CryptoUtil::CryptoUtil(QObject *parent)
    : QObject{parent}, p{new CryptoUtilPvt{this}}
{
}

QByteArray CryptoUtil::encrypt(const QByteArray &plain)
{
    if(plain.trimmed().isEmpty())
        return {};
#ifdef QTREFORCE_QTINIAES
    return p->aes.encrypt(plain);
#else
    return plain;
#endif
}

QByteArray CryptoUtil::encryptToHex(const QByteArray &plain)
{
    if(plain.trimmed().isEmpty())
        return {};
#ifdef QTREFORCE_QTINIAES
    return p->aes.encrypt(plain).toHex();
#else
    return plain;
#endif
}

QByteArray CryptoUtil::decrypt(const QByteArray &cipher)
{
    if(cipher.trimmed().isEmpty())
        return {};
#ifdef QTREFORCE_QTINIAES
    return p->aes.decrypt(cipher);
#else
    return cipher;
#endif
}

QByteArray CryptoUtil::decryptFromHex(const QByteArray &cipherHex)
{
    auto cipher=QByteArray::fromHex(cipherHex);
    return decrypt(cipher);
}


} // namespace QStm
