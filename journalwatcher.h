#ifndef JOURNALWATCHER_H
#define JOURNALWATCHER_H

#include <QObject>
#include <QSocketNotifier>
#include <systemd/sd-journal.h>

class JournalWatcher : public QObject
{
    Q_OBJECT

public:
    JournalWatcher();
    ~JournalWatcher();

    void addMatch(const QString& field, const QString& value);
    bool loadIdsFromJson(const QString& fileName);
    void startWatching();

signals:
    void journalEntryMatched(const QString& sysid, const QString& message);

private slots:
    void onJournalReady();

private:
    void processJournal();
    sd_journal* m_journal;
    QSocketNotifier *m_notifier;
};

#endif // JOURNALWATCHER_H
