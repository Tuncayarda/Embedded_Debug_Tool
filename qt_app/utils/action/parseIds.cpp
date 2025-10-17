#include "../../actionwindow.h"
#include <QStringList>
#include <QRadioButton>

std::vector<int> AddActionWindow::parseIds(const QString& s) {
    std::vector<int> out;
    const QStringList parts = s.split(',', Qt::SkipEmptyParts);
    out.reserve(parts.size());
    for (const QString &p : parts) {
        bool ok = false;
        int v = p.trimmed().toInt(&ok);
        if (ok) out.push_back(v);
    }
    return out;
}
