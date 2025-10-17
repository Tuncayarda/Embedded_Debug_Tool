#include "../../../serialmonitor.h"
#include "../../../ui_serialmonitor.h"
#include "../../../actionEncoder.h"
#include <QDateTime>

void SerialMonitor::logTx(const QByteArray &packet, const ActionSet* set)
{
    const QString ts = m_timestamp
                           ? QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ")
                           : QString();

    if (packet.size() < 7) {
        if (ui->txCheckBox->isChecked())
            ui->textEdit->append(QString("%1TX len=%2 %3").arg(ts).arg(packet.size()).arg(bytesToHex(packet)));
        return;
    }

    const int total = packet.size();
    const int payloadLen = total - 2 /*SOF*/ - 1 /*msg*/ - 2 /*len*/ - 2 /*crc*/;
    if (payloadLen < 0) {
        if (ui->txCheckBox->isChecked())
            ui->textEdit->append(QString("%1TX len=%2 %3").arg(ts).arg(packet.size()).arg(bytesToHex(packet)));
        return;
    }

    const quint8 msgId = (quint8)packet[2];
    const QByteArray payload = packet.mid(5, payloadLen);

    if (set == nullptr) {
        QString html = QString("%1TX len=%2 ").arg(ts).arg(packet.size())
        + coloredHeader(packet) + " ";

        if (msgId == MSG_ID_CSPI_BEGIN) {
            html += coloredBulkBeginCSPI(payload) + " ";
        }
        else if (msgId == MSG_ID_CSPI_DATA) {
            html += coloredBulkData(payload) + " ";
        }
        else if (msgId == MSG_ID_CSPI_END) {
            html += coloredBulkEnd() + " ";
        }
        else {
            html += bytesToHex(payload) + " ";
        }
        html += coloredCrc(packet);
        if (ui->txCheckBox->isChecked())
            ui->textEdit->append(html);
        return;
    }

    const std::vector<Action*> actions = idAscending(set);

    const QString html =
        QString("%1TX len=%2 ").arg(ts).arg(packet.size())
        + coloredHeader(packet) + " "
        + coloredActions(payload, actions) + " "
        + coloredCrc(packet);

    if (ui->txCheckBox->isChecked())
        ui->textEdit->append(html);
}
