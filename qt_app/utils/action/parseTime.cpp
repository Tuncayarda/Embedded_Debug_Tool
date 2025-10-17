#include "../../actionwindow.h"
#include <QString>
#include <QStringList>
#include <QRegularExpression>

void AddActionWindow::parseTime(const QString& text, uint32_t &ms, uint32_t &us)
{
    ms = 0;
    us = 0;

    QString t = text.trimmed();
    if (t.isEmpty())
        return;

    static const QRegularExpression nonDigits(QStringLiteral("[^0-9]"));
    const QStringList parts = t.split('.', Qt::SkipEmptyParts);

    bool okMs = false;
    if (parts.size() >= 1) {
        ms = parts[0].toUInt(&okMs);
        if (!okMs) ms = 0;
    }

    if (parts.size() >= 2) {
        QString rhs = parts[1];
        rhs.remove(nonDigits);
        if (!rhs.isEmpty()) {
            if (rhs.size() <= 3) {
                QString padded = rhs + QString(3 - rhs.size(), QChar('0'));
                us = padded.toUInt();
            } else {
                uint32_t tmp = rhs.left(9).toUInt();
                ms += tmp / 1000u;
                us  = tmp % 1000u;
            }
        }
    }
}
