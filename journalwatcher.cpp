#include "journalwatcher.h"

#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

JournalWatcher::JournalWatcher()
{
    int r = sd_journal_open(&m_journal, SD_JOURNAL_SYSTEM);
    if (r < 0) {
        qWarning() << "Failed to open journal: " << strerror(-r);
    }
    // Load the IDs from a JSON file
    /*
    if (!loadIdsFromJson("regexes.json")) {
        // Handle error
        qWarning() << "Failed to loadIdsFromJson.";
    }
    */
}

JournalWatcher::~JournalWatcher()
{
    if (m_notifier) {
        delete m_notifier;
    }
    if (m_journal) {
        sd_journal_close(m_journal);
    }
}

void JournalWatcher::addMatch(const QString& field, const QString& value)
{
    int err = sd_journal_add_match(m_journal, qPrintable(field + "=" + value), 0);
    qWarning() << "sd_journal_add_match() =>" << err;
}

void JournalWatcher::startWatching()
{
    if (!m_journal) {
        qWarning() << "Journal not opened";
        return;
    }

    int err = sd_journal_seek_tail(m_journal);
    qWarning() << "sd_journal_seek_tail() =>" << err;

    // sd_journal_previous_skip(m_journal, 1);
    err = sd_journal_step_one(m_journal, 0);
    qWarning() << "sd_journal_step_one() =>" << err;

    int fd = sd_journal_get_fd(m_journal);
    if( fd < 0 ) {
        qWarning() << "sd_journal_get_fd() =>" << strerror(-fd);
    }
    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &JournalWatcher::onJournalReady);

    processJournal(); // Process any existing entries
}

void JournalWatcher::onJournalReady()
{
    int err = sd_journal_process(m_journal);
    if( err < 0 ) {
        qWarning() << "sd_journal_process() =>" << strerror(-err);
    } // else {
       // qWarning() << "sd_journal_process() =>" << err;
//    }
    processJournal();
}

void JournalWatcher::processJournal()
{
    while (true) {
        int r = sd_journal_next(m_journal);
        if (r < 0) {
            qWarning() << "Failed to iterate to next entry:" << strerror(-r);
            break;
        }
        if (r == 0) {
            // Reached the end
            break;
        }

        const void *d;
        size_t l;
        r = sd_journal_get_data(m_journal, "SYSLOG_IDENTIFIER", &d, &l);
        if (r < 0) {
            qWarning() << "Failed to read SYSLOG_IDENTIFIER field:" << strerror(-r);
            continue;
        }
        QString sysid = QString::fromUtf8((const char *)d + 18, l - 18);

        r = sd_journal_get_data(m_journal, "MESSAGE", &d, &l);
        if (r < 0) {
            qWarning() << "Failed to read MESSAGE field:" << strerror(-r);
            continue;
        }
        QString message = QString::fromUtf8((const char *)d + 8, l - 8);

        qDebug() << "Emitting journalEntryMatched:" << sysid << message;
        emit journalEntryMatched(sysid, message);
    }
}

bool JournalWatcher::loadIdsFromJson(const QString& fileName)
{
    qWarning() << "loadIdsFromJson()";
    // Read the JSON file
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "loadIdsFromJson file.open";
        return false;
    }
    qWarning() << fileName << "open.";
    QByteArray data = file.readAll();
    file.close();

    // Parse the JSON data
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Failed to parse the JSON data!";
        return false;
    }
    //QJsonObject obj = doc.object();

    // Iterate over the "regexes" array
    /*
    QJsonArray regexesArray = obj["regexes"].toArray();
    for (const auto& regexObject : regexesArray) {
        // Get the "name" property
        QString name = regexObject["name"].toString();
    }
    */
    QJsonObject json = doc.object();
    if (json.contains("regexes") && json["regexes"].isArray()) {
        QJsonArray regexes = json["regexes"].toArray();
        qWarning() << "regexes ==" << regexes;
        for (int regIndex = 0; regIndex < regexes.size(); ++regIndex) {
            QJsonObject regObject = regexes[regIndex].toObject();
            QString name = regObject["name"].toString();
            // Add the name and list of regexes to the map
            qWarning() << "addMatch(\"SYSLOG_IDENTIFIER\", " << name << ");";
            addMatch("SYSLOG_IDENTIFIER", name);
        }
    } else {
        qWarning() << "no regexes";
    }
    return true;
}
