#include "./qstm_log.h"
#include "./qstm_envs.h"
#include <QDateTime>
#include <QCoreApplication>

namespace QStm {

typedef QHash<int, QString> MsgTypeHash;

Q_GLOBAL_STATIC_WITH_ARGS(MsgTypeHash, msgTypeMap,({{QtDebugMsg, "D"},{QtWarningMsg, "W"},{QtCriticalMsg, "C"},{QtFatalMsg, "F"},{QtInfoMsg, "I"},{QtSystemMsg, "S"}}))
Q_GLOBAL_STATIC(Envs, staticEnvs)
static const QtMessageHandler qtMessageHandlerDefault = qInstallMessageHandler(0);

#ifdef QT_DEBUG
static bool staticQT_LOG_ENABLED=true;
static bool staticQT_LOG_DEBUG=true;
static bool staticQT_LOG_INFO=true;
static bool staticQT_LOG_WARNING=true;
static bool staticQT_LOG_CRITICAL=true;
static bool staticQT_LOG_SSL_WARNING=true;
#else
static bool staticQT_LOG_ENABLED=false;
static bool staticQT_LOG_DEBUG=false;
static bool staticQT_LOG_INFO=false;
static bool staticQT_LOG_WARNING=false;
static bool staticQT_LOG_CRITICAL=false;
static bool staticQT_LOG_SSL_WARNING=false;
#endif

//Q_GLOBAL_STATIC(QString, static_log_dir);


static void initQtFlags()
{
    auto toBool=[](bool v){
        return v?"true":"false";
    };

    QStringList format={
        QString("qt.network.ssl.warning=%1").arg(toBool(staticQT_LOG_SSL_WARNING)),
        QString("*.debug=true;qml.debug=%1").arg(toBool(staticQT_LOG_DEBUG)),
        QString("*.warning=%1").arg(toBool(staticQT_LOG_INFO)),
        QString("*.critical=%1").arg(toBool(staticQT_LOG_WARNING)),
        QString("*.info=%1").arg(toBool(staticQT_LOG_CRITICAL)),
    };

    staticEnvs->systemEnvs("QT_LOGGING_RULES", format.join(','));
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

static void initEnvs()
{
    staticQT_LOG_ENABLED=staticEnvs->value("QT_LOG_ENABLED").toBool();
    if(staticQT_LOG_ENABLED){
        staticQT_LOG_SSL_WARNING=staticEnvs->value("QT_LOG_SSL_WARNING").toBool();
        staticQT_LOG_DEBUG=staticEnvs->value("LOG_DEBUG").toBool();
        staticQT_LOG_INFO=staticEnvs->value("LOG_INFO").toBool();
        staticQT_LOG_WARNING=staticEnvs->value("LOG_WARNING").toBool();
        staticQT_LOG_CRITICAL=staticEnvs->value("LOG_CRITICAL").toBool();
    }
}

static void init()
{
    initEnvs();
    initQtFlags();
    QStm::Log::enabled();
}

Q_COREAPP_STARTUP_FUNCTION(init)

Log::Log(QObject *parent) : QObject{parent}
{

}

bool Log::qLogEnabled()
{
    return staticQT_LOG_ENABLED;
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

