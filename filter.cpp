#include "filter.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Filter::Filter(QObject* parent)
    : QObject(parent)
{
    // Load the regexes from a JSON file
    /*
    if (!loadRegexesFromJson("regexes.json")) {
        // Handle error
    }
    */
}

bool Filter::loadRegexesFromJson(const QString& fileName)
{
    // Read the JSON file
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    // Parse the JSON data
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return false;
    }
    QJsonObject obj = doc.object();

    // Iterate over the "regexes" array
    QJsonArray regexesArray = obj["regexes"].toArray();
    for (const auto& regexObject : regexesArray) {
        // Get the "name" and "patterns" properties
        QString name = regexObject.toObject()["name"].toString();
        QJsonArray patternsArray = regexObject.toObject()["patterns"].toArray();
        // Create a list of QRegularExpression objects
        QList<QRegularExpression> regexList;
        for (const auto& pattern : patternsArray) {
            QRegularExpression regex(pattern.toString());
            regexList.append(regex);
        }

        // Add the name and list of regexes to the map
        m_regexes[name] = regexList;
    }
    return true;
}

void Filter::onJournalEntryMatched(const QString& sysid, const QString& message)
{
    // qDebug() << "Filter:" << sysid;
    // Iterate over the map of regexes
    for (const auto& name : m_regexes.keys()) {
        if( name == sysid ) {
            const auto& regexList = m_regexes[name];    // Check if any of the regexes in the list match the message
            for (const auto& regex : regexList) {
                // qDebug() << "Trying:" << regex;
                const auto& gyufa = regex.match(message);
                if (gyufa.hasMatch()) {
                    // Extract the IP address from the message
                    QString ip = gyufa.captured(1);
                    qWarning() << "BanJackHammering" << ip << "Reason:" << regex;;
                    // Emit the signal to add the IP address to the set
                    emit addIpToSet(ip);
                    break;
                } /* else {
                    qDebug() << "No match.";
                } */
            }
        }
    }
    // qDebug() << "---";
}
