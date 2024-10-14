#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include <QRegularExpression>

class Filter : public QObject
{
    Q_OBJECT

public:
    Filter(QObject* parent = nullptr);

    bool loadRegexesFromJson(const QString& fileName);

public slots:
    void onJournalEntryMatched(const QString& sysid, const QString& message);

signals:
    void addIpToSet(const QString& ip);

private:
    QMap<QString, QList<QRegularExpression>> m_regexes;
};

#endif // FILTER_H
