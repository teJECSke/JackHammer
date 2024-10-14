#ifndef NFTABLESINTERFACE_H
#define NFTABLESINTERFACE_H

#include <QObject>

class NftablesInterface : public QObject
{
    Q_OBJECT

public:
    explicit NftablesInterface(QObject* parent = nullptr);
    ~NftablesInterface();

public Q_SLOTS:
    void addIpToSet(const QString& ip);
    void deleteIpFromSet(const QString& ip);
};
#endif // NFTABLESINTERFACE_H
