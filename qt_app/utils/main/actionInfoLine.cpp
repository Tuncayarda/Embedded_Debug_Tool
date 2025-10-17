#include "../../mainwindow.h"
#include <QString>

static const char* kindName(Kind k) {
    switch (k) {
    case Kind::START:       return "START";
    case Kind::DELAY:       return "DELAY";
    case Kind::PIN_READ:    return "PIN_READ";
    case Kind::PIN_WRITE:   return "PIN_WRITE";
    case Kind::PIN_TRIGGER: return "PIN_TRIGGER";
    default:                return "UNKNOWN";
    }
}

static inline QString lvl(Level x) {
    switch (x) {
    case Level::LOW:  return "L";
    case Level::HIGH: return "H";
    default:          return "?";
    }
}

static inline QString portLetter(int port) {
    if (port < 0) return "?";
    int idx = port;
    if (idx >= 0 && idx < 26) return QString(QChar('A' + idx));
    return "?";
}

static inline QString joinIds(const std::vector<int>& v) {
    QString out; out += '[';
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) out += ',';
        out += QString::number(v[i]);
    }
    out += ']';
    return out;
}

QString MainWindow::actionInfoLine(const Action* a) const {
    if (!a) return {};

    const std::vector<int> deps   = actionSet.parentsOf(a->id);
    const std::vector<int>& after = a->runAfterMe;

    QString s;
    s.reserve(256);

    s += QString("ID:%1  %2").arg(a->id).arg(kindName(a->kind));

    switch (a->kind) {
    case Kind::START: {
    } break;

    case Kind::DELAY: {
        if (auto d = dynamic_cast<const DelayAction*>(a)) {
            if (d->durationUs > 0)
                s += QString("  duration=%1.%2ms").arg(d->durationMs).arg(d->durationUs, 3, 10, QLatin1Char('0'));
            else
                s += QString("  duration=%1ms").arg(d->durationMs);
        } else {
            s += "  duration=?ms";
        }
    } break;

    case Kind::PIN_READ: {
        if (auto r = dynamic_cast<const PinReadAction*>(a)) {
            QString dur = (r->durationUs > 0)
            ? QString("%1.%2ms").arg(r->durationMs).arg(r->durationUs, 3, 10, QLatin1Char('0'))
            : QString("%1ms").arg(r->durationMs);
            s += QString("  port=%1  pin=%2  init=%3  target=%4  final=%5  duration=%6")
                     .arg(portLetter(r->port))
                     .arg(r->pin >= 0 ? QString::number(r->pin) : "?")
                     .arg(lvl(r->initial))
                     .arg(lvl(r->target))
                     .arg(lvl(r->final))
                     .arg(dur);
        } else {
            s += "  port=?  pin=?  init=?  target=?  final=?  duration=?ms";
        }
    } break;

    case Kind::PIN_WRITE: {
        if (auto w = dynamic_cast<const PinWriteAction*>(a)) {
            QString dur = (w->durationUs > 0)
            ? QString("%1.%2ms").arg(w->durationMs).arg(w->durationUs, 3, 10, QLatin1Char('0'))
            : QString("%1ms").arg(w->durationMs);
            s += QString("  port=%1  pin=%2  init=%3  target=%4  final=%5  duration=%6")
                     .arg(portLetter(w->port))
                     .arg(w->pin >= 0 ? QString::number(w->pin) : "?")
                     .arg(lvl(w->initial))
                     .arg(lvl(w->target))
                     .arg(lvl(w->final))
                     .arg(dur);
        } else {
            s += "  port=?  pin=?  init=?  target=?  final=?  duration=?ms";
        }
    } break;

    case Kind::PIN_TRIGGER: {
        if (auto t = dynamic_cast<const PinTriggerAction*>(a)) {
            QString tout = (t->timeoutUs > 0)
            ? QString("%1.%2ms").arg(t->timeoutMs).arg(t->timeoutUs, 3, 10, QLatin1Char('0'))
            : QString("%1ms").arg(t->timeoutMs);
            s += QString("  port=%1  pin=%2  init=%3  target=%4  timeout=%5")
                     .arg(portLetter(t->port))
                     .arg(t->pin >= 0 ? QString::number(t->pin) : "?")
                     .arg(lvl(t->initial))
                     .arg(lvl(t->target))
                     .arg(tout);
        } else {
            s += "  port=?  pin=?  init=?  target=?  timeout=?ms";
        }
    } break;

    default:
        break;
    }

    s += QString("  deps=%1  after=%2")
             .arg(joinIds(deps))
             .arg(joinIds(after));

    return s;
}
