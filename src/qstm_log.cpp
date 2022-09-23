#include "./qstm_log.h"
#include <QDateTime>
#include <QCoreApplication>

namespace QStm {


typedef QHash<int, QString> MsgTypeMap;

Q_GLOBAL_STATIC_WITH_ARGS(MsgTypeMap, msgTypeMap,({{QtDebugMsg, "D"},{QtWarningMsg, "W"},{QtCriticalMsg, "C"},{QtFatalMsg, "F"},{QtInfoMsg, "I"},{QtSystemMsg, "S"}}))

static const QtMessageHandler qtMessageHandlerDefault = qInstallMessageHandler(0);

static bool staticQ_LOG_ENABLED=false;
static bool staticLogRegister=false;
//Q_GLOBAL_STATIC(QString, static_log_dir);


static void initMsg()
{
    auto format=
    QByteArrayLiteral("qt.network.ssl.warning=true;*.debug=true;qml.debug=true;*.warning=true;*.critical=true;*.info=true");
    qputenv(QByteArrayLiteral("QT_LOGGING_RULES"), format);
}

static void qtMessageHandlerCustomized(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static auto timeFormat=QStringLiteral("hh:mm:ss");
    static auto logFormat=QStringLiteral("#%1-%2:%3");
    static auto printFormat=QByteArrayLiteral("%s\n");

    const static auto replaceText=QStringList{QT_STRINGIFY(ResultValue &),QT_STRINGIFY(ResultValue &),QT_STRINGIFY(QApp::),QStringLiteral("\"")};
    auto line = context.line<=0?QTime::currentTime().toString(timeFormat):QString::number(context.line).rightJustified(5,'0');
    auto message=logFormat.arg(msgTypeMap->value(type),line,msg);
    for(auto &key:replaceText)
        message.replace(key, "");
    fprintf(stderr, printFormat, message.toUtf8().constData());
}

static void init()
{
#ifdef QT_DEBUG
    staticLogRegister = true;
#else
    staticQ_LOG_ENABLED = QVariant{QString{getenv(QByteArrayLiteral("Q_LOG_ENABLED"))}.trimmed()}.toBool();
    staticLogRegister = staticQ_LOG_ENABLED;
#endif
    if(!staticLogRegister)
        return;
    initMsg();
    QStm::Log::enabled();
}

Q_COREAPP_STARTUP_FUNCTION(init);

Log::Log(QObject *parent) : QObject{parent}
{

}

bool Log::qLogEnabled()
{
    return staticQ_LOG_ENABLED;
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

