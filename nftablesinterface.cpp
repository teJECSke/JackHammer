#include "nftablesinterface.h"

#include <nftables/libnftables.h>

#include <QDebug>

NftablesInterface::NftablesInterface(QObject* parent)
    : QObject(parent)
{
}

NftablesInterface::~NftablesInterface()
{
}

void NftablesInterface::addIpToSet(const QString& ip)
{
    // Create a new command list
    struct nft_ctx* ctx = nft_ctx_new(NFT_CTX_DEFAULT);
    if (!ctx) {
        qWarning() << "Failed to create nftables context";
        return;
    }

    // Create a new "add" command to add the IP address to the set
    QString cmd = QString("add element sshguard attackers { %1 }").arg(ip);
    // cmd += ip;
    QByteArray buf = cmd.toUtf8();

    // qWarning() << "Execute the command " << buf;
    int r = nft_run_cmd_from_buffer(ctx, buf);
    if (r < 0) {
        qWarning() << "Failed to execute nftables command: " << strerror(-r);
    }

    // Clean up
    nft_ctx_free(ctx);
}

void NftablesInterface::deleteIpFromSet(const QString& ip)
{
    // Create a new command list
    struct nft_ctx* ctx = nft_ctx_new(NFT_CTX_DEFAULT);
    if (!ctx) {
        qWarning() << "Failed to create nftables context";
        return;
    }

    // Create a new "delete" command to delete the IP address from the set
    QString cmd = QString("delete element sshguard attackers { %1 }").arg(ip);
    // cmd += ip;
    QByteArray buf = cmd.toUtf8();

    // qWarning() << "Execute the command " << buf;
    int r = nft_run_cmd_from_buffer(ctx, buf);
    if (r < 0) {
        qWarning() << "Failed to execute nftables command: " << strerror(-r);
    }

    // Clean up
    nft_ctx_free(ctx);
}
