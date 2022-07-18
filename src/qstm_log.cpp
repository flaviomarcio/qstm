#include "./qstm_log.h"
#include <QDateTime>
#include <QCoreApplication>

namespace QStm {


typedef QHash<int, QString> MsgTypeMap;

Q_GLOBAL_STATIC(MsgTypeMap, msgTypeMap)

static const QtMessageHandler qtMessageHandlerDefault = qInstallMessageHandler(0);

static void initMsg()
{
    auto &vHash=*msgTypeMap;
    vHash[QtDebugMsg]=QStringLiteral("D");
    vHash[QtWarningMsg]=QStringLiteral("W");
    vHash[QtCriticalMsg]=QStringLiteral("C");
    vHash[QtFatalMsg]=QStringLiteral("F");
    vHash[QtInfoMsg]=QStringLiteral("I");
    vHash[QtSystemMsg]=QStringLiteral("S");

    auto format=
#ifdef QTREFORCE_DEBUG_LOG
    QByteArrayLiteral("qt.network.ssl.warning=true;qml.debug=true;*.debug=true;*.warning=true;*.critical=true;*.info=true");
#else
    QByteArrayLiteral("qt.network.ssl.warning=true;qml.debug=true;*.debug=false;*.warning=true;*.critical=true;*.info=true");
#endif
    qputenv(QByteArrayLiteral("QT_LOGGING_RULES"), format);
}



static void qtMessageHandlerCustomized(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static auto timeFormat=QStringLiteral("hh:mm:ss");
    static auto logFormat=QStringLiteral("#%1-%2:%3");
    static auto printFormat=QByteArrayLiteral("%s\n");

    const static auto replaceText=QStringList{QT_STRINGIFY(ResultValue &),QT_STRINGIFY(ResultValue &),QT_STRINGIFY(ServerService::),QStringLiteral("\"")};
    auto line = context.line<=0?QTime::currentTime().toString(timeFormat):QString::number(context.line).rightJustified(5,'0');
    auto message=logFormat.arg(msgTypeMap->value(type),line,msg);
    for(auto &key:replaceText)
        message.replace(key, "");
    fprintf(stderr, printFormat, message.toUtf8().constData());
}

static void init()
{
    initMsg();
    QStm::Log::enabled();
}

Q_COREAPP_STARTUP_FUNCTION(init);

Log::Log(QObject *parent) : QObject{parent}
{

}

void Log::enabled()
{
    static auto format="[%{if-debug}"
                       "D%{endif}"
                       "%{if-info}"
                       "I%{endif}"
                       "%{if-warning}"
                       "W%{endif}"
                       "%{if-critical}"
                       "C%{endif}"
                       "%{if-fatal}"
                       "F%{endif} "
                       "%{time yyyy.MM.dd h:mm:ss.zzz t}] "
                       "| line: %{line} | func: %{function} | %{message}";
    qSetMessagePattern(format);
    qInstallMessageHandler(qtMessageHandlerCustomized); // Install the handler local
}

void Log::disabled()
{
    qInstallMessageHandler(qtMessageHandlerDefault); // Install the handler local
}

}

