#pragma once

#include "./qstm_global.h"

namespace QStm {

class CryptoUtilPvt;
class Q_STM_EXPORT CryptoUtil : public QObject
{
    Q_OBJECT
public:
    explicit CryptoUtil(QObject *parent = nullptr);

    //!
    //! \brief encrypt
    //! \param plain
    //! \return
    //!
    QByteArray encrypt(const QByteArray &plain);
    QByteArray encryptToHex(const QByteArray &plain);

    //!
    //! \brief decrypt
    //! \param cipher
    //! \return
    //!
    QByteArray decrypt(const QByteArray &cipher);
    QByteArray decryptFromHex(const QByteArray &cipherHex);
private:
    CryptoUtilPvt *p=nullptr;
};

} // namespace QStm

