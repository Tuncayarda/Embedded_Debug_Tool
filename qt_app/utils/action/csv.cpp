#include "../../actionwindow.h"
#include "../../ui_actionwindow.h"

QString AddActionWindow::idsToCsv(const std::vector<int>& v) {
    QStringList parts; parts.reserve((int)v.size());
    for (int x : v) parts << QString::number(x);
    return parts.join(',');
}
std::vector<int> AddActionWindow::csvToIds(const QString& s) {
    std::vector<int> out;
    for (const QString& t : s.split(',', Qt::SkipEmptyParts)) {
        bool ok=false; int v=t.trimmed().toInt(&ok);
        if (ok) out.push_back(v);
    }
    return out;
}

QString AddActionWindow::depsCsv() const {
    switch (currentKind()) {
    case Kind::DELAY:       return ui->lineEdit_dependency_d->text();
    case Kind::PIN_READ:    return ui->lineEdit_dependency_r->text();
    case Kind::PIN_WRITE:   return ui->lineEdit_dependency_w->text();
    case Kind::PIN_TRIGGER: return ui->lineEdit_dependency_t->text();
    default: return {};
    }
}
void AddActionWindow::depsSetCsv(const QString& s) {
    switch (currentKind()) {
    case Kind::DELAY:       ui->lineEdit_dependency_d->setText(s); break;
    case Kind::PIN_READ:    ui->lineEdit_dependency_r->setText(s); break;
    case Kind::PIN_WRITE:   ui->lineEdit_dependency_w->setText(s); break;
    case Kind::PIN_TRIGGER: ui->lineEdit_dependency_t->setText(s); break;
    default: break;
    }
}

std::vector<int> AddActionWindow::deps() const {
    return csvToIds(depsCsv());
}
