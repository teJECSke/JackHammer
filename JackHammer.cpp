#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>

#include "journalwatcher.h"
#include "nftablesinterface.h"
#include "filter.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    // Create the JournalWatcher, Filter, and NftablesInterface objects
    JournalWatcher journalWatcher;
    Filter filter;
    NftablesInterface nftablesInterface;

    // Set up the command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("JackHammer");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add the "rules" option
    QCommandLineOption rulesOption(QStringList() << "rules", "JSON file containing rules.", "file");
    parser.addOption(rulesOption);

    // Add the "ban" option
    QCommandLineOption banOption(QStringList() << "ban", "Ban the specified IP address.", "ip");
    parser.addOption(banOption);

    // Add the "unban" option
    QCommandLineOption unbanOption(QStringList() << "unban", "Unban the specified IP address.", "ip");
    parser.addOption(unbanOption);

    // Parse the command line arguments
    parser.process(app);

    // Check if the "ban" option was specified
    if (parser.isSet(banOption)) {
        QString ip = parser.value(banOption);
        nftablesInterface.addIpToSet(ip);
        return 0;
    }

    // Check if the "unban" option was specified
    if (parser.isSet(unbanOption)) {
        QString ip = parser.value(unbanOption);
        nftablesInterface.deleteIpFromSet(ip);
        return 0;
    }

    // Check if the "rules" option was specified
    if (parser.isSet(rulesOption)) {
        QString rules = parser.value(rulesOption);
        journalWatcher.loadIdsFromJson(rules);
        filter.loadRegexesFromJson(rules);
    }

    // Connect the signals and slots
    QObject::connect(&journalWatcher, &JournalWatcher::journalEntryMatched,
                     &filter, &Filter::onJournalEntryMatched);
    QObject::connect(&filter, &Filter::addIpToSet,
                     &nftablesInterface, &NftablesInterface::addIpToSet);

    // Start the JournalWatcher
    journalWatcher.startWatching();
    qWarning() << "Starting main event loop.";
    int vege = app.exec();
    qWarning() << "Vége:" << vege;
    return vege;
}
