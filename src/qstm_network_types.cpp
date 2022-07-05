#include "./qstm_network_types.h"

namespace QStm{

Network::Network(QObject *parent):QObject{parent}
{

}

const QHash<int, QString> &Network::methodName(){
    static const QHash<int,QString> __return{
        {HEAD,"Head"},
        {HEAD,"head"},
        {GET,"Get"},
        {GET,"get"},
        {POST,"Post"},
        {POST,"post"},
        {PUT,"Put"},
        {PUT,"put"},
        {DELETE,"Delete"},
        {DELETE,"delete"}
    };
    return __return;
}

QString Network::urlParsert(const QString &protocol, const QString &hostName, int port, const QString &basePath, const QString &path)
{
    auto sport=port<=0?"":QStringLiteral(":%1").arg(port);
    auto url=QStringLiteral("%1%2/%3/%4").arg(hostName, sport, basePath, path);
    while(url.contains(QStringLiteral("//")))
        url.replace(QStringLiteral("//"), QStringLiteral("/"));
    return QStringLiteral("%1://%2").arg(protocol, url);
}

}
