#include "../../../serialmonitor.h"
#include "../../../actionEncoder.h"
#include <QDateTime>

QString SerialMonitor::bytesToHex(const QByteArray &data) const {
    QString s; s.reserve(data.size() * 3);
    for (unsigned char b : data)
        s += QString("%1 ").arg(b, 2, 16, QLatin1Char('0')).toUpper();
    return s.trimmed();
}

QString SerialMonitor::coloredHeader(const QByteArray& packet) const {
    if (packet.size() < 5) return bytesToHex(packet);

    auto hx = [](unsigned char b){
        return QString("%1").arg(b,2,16,QLatin1Char('0')).toUpper();
    };

    const unsigned char sof0 = (unsigned char)packet[0];
    const unsigned char sof1 = (unsigned char)packet[1];
    const unsigned char msg  = (unsigned char)packet[2];
    const unsigned char lenH = (unsigned char)packet[3];
    const unsigned char lenL = (unsigned char)packet[4];

    return QString(
               "<span style='color:#e74c3c'>%1 %2</span> "
               "<span style='color:#2980b9'>%3</span> "
               "<span style='color:#f39c12'>%4 %5</span>"
               )
        .arg(hx(sof0)).arg(hx(sof1))
        .arg(hx(msg))
        .arg(hx(lenH)).arg(hx(lenL));
}

QString SerialMonitor::coloredCrc(const QByteArray& packet) const {
    if (packet.size() < 2) return {};
    const int n = packet.size();
    auto hx = [](unsigned char b){
        return QString("%1").arg(b,2,16,QLatin1Char('0')).toUpper();
    };
    return QString("<span style='color:#7f8c8d'>%1 %2</span>")
        .arg(hx((unsigned char)packet[n-2]))
        .arg(hx((unsigned char)packet[n-1]));
}

std::vector<Action*> SerialMonitor::idAscending(const ActionSet* set) const {
    std::vector<Action*> v;
    if (!set) return v;
    v.reserve(set->nodes().size());
    for (auto &up : set->nodes()) v.push_back(up.get());
    std::sort(v.begin(), v.end(), [](const Action* a, const Action* b){
        return a->id < b->id;
    });
    return v;
}

QString SerialMonitor::coloredActions(const QByteArray& payload,
                                      const std::vector<Action*>& actions) const
{
    if (payload.isEmpty() || actions.empty())
        return {};

    std::vector<int> sizes; sizes.reserve(actions.size());
    std::vector<Action*> prefix; prefix.reserve(actions.size());

    int prev = 0;
    for (size_t i=0; i<actions.size(); ++i) {
        prefix.push_back(actions[i]);
        std::vector<std::uint8_t> enc = encodeActionPayload(prefix);
        const int curr = (int)enc.size();
        sizes.push_back(curr - prev);
        prev = curr;
    }
    int sum = 0; for (int s : sizes) sum += s;
    if (sum != payload.size()) {
        return bytesToHex(payload);
    }

    static const char* COLORS[] = {
        "#27ae60", "#8e44ad", "#d35400", "#16a085",
        "#2c3e50", "#c0392b", "#7d3cff", "#00a8ff"
    };
    const int C = (int)(sizeof(COLORS)/sizeof(COLORS[0]));

    QString out;
    out.reserve(payload.size() * 12);

    int off = 0;
    for (size_t i=0; i<sizes.size(); ++i) {
        const int len = sizes[i];
        const QByteArray chunk = payload.mid(off, len);
        off += len;

        const QString hex = bytesToHex(chunk);
        out += QString("<span style='color:%1'>%2</span> ")
                   .arg(COLORS[i % C], hex);
    }
    return out.trimmed();
}

QString SerialMonitor::coloredCSPI(const QByteArray& p) const
{
    // yeni minimum: 1+1+2+1+2+4+1+1 = 13 byte
    if (p.size() < 13)
        return bytesToHex(p);

    auto hx = [](unsigned char b){
        return QString("%1").arg(b, 2, 16, QLatin1Char('0')).toUpper();
    };
    auto be16 = [](unsigned char hi, unsigned char lo) -> int {
        return (int(hi) << 8) | int(lo);
    };

    // [5..6] = txSize (BE)
    const int txSize = be16((unsigned char)p[5], (unsigned char)p[6]);
    const int headerLen = 13;
    if (p.size() < headerLen + txSize)
        return bytesToHex(p);

    const QByteArray txData = p.mid(headerLen, txSize);

    // renkler
    const QString cMode    = "#9b59b6"; // mode
    const QString cWord    = "#2c3e50"; // word size
    const QString cRead    = "#f39c12"; // read size (2 byte)
    const QString cXfer    = "#8e44ad"; // transfer_size (1 byte)
    const QString cTxLen   = "#27ae60"; // tx size (2 byte)
    const QString cThr     = "#d35400"; // threshold (4 byte)
    const QString cPortPin = "#16a085"; // port & pin
    const QString cTxData  = "#2980b9"; // tx data

    auto span = [&](unsigned char b, const QString& col){
        return QString("<span style='color:%1'>%2</span>").arg(col, hx(b));
    };

    QString out; out.reserve(p.size() * 16);

    // header
    out += span((unsigned char)p[0], cMode) + " ";                // mode
    out += span((unsigned char)p[1], cWord) + " ";                // word
    out += span((unsigned char)p[2], cRead) + " ";                // rx_hi
    out += span((unsigned char)p[3], cRead) + " ";                // rx_lo
    out += span((unsigned char)p[4], cXfer) + " ";                // transfer_size
    out += span((unsigned char)p[5], cTxLen) + " ";               // tx_hi
    out += span((unsigned char)p[6], cTxLen) + " ";               // tx_lo

    // threshold 4 byte (BE)
    out += span((unsigned char)p[7],  cThr) + " ";
    out += span((unsigned char)p[8],  cThr) + " ";
    out += span((unsigned char)p[9],  cThr) + " ";
    out += span((unsigned char)p[10], cThr) + " ";

    // port & pin
    out += span((unsigned char)p[11], cPortPin) + " ";
    out += span((unsigned char)p[12], cPortPin);

    // txData
    if (!txData.isEmpty()) {
        out += "  ";
        for (unsigned char b : txData)
            out += " " + span(b, cTxData);
    }

    return out.trimmed();
}

QString SerialMonitor::coloredBulkBeginCSPI(const QByteArray& hdr) const
{
    // CSPI header: 13B => mode(1), word(1), readSize(2BE), xfer(1), txLen(2BE),
    //               threshold(4BE), port(1), pin(1)
    if (hdr.size() < 13) return bytesToHex(hdr);

    auto hx = [](unsigned char b){
        return QString("%1").arg(b,2,16,QLatin1Char('0')).toUpper();
    };

    const QString cMode    = "#9b59b6"; // mode
    const QString cWord    = "#2c3e50"; // word size
    const QString cRead    = "#f39c12"; // read size (2B)
    const QString cXfer    = "#8e44ad"; // transfer_size (1B)
    const QString cTxLen   = "#27ae60"; // txLen (2B)
    const QString cThr     = "#d35400"; // threshold (4B)
    const QString cPortPin = "#16a085"; // port+pin

    auto span = [&](unsigned char b, const QString& col){
        return QString("<span style='color:%1'>%2</span>").arg(col, hx(b));
    };

    QString out;
    out.reserve(200);

    // mode, word
    out += span((unsigned char)hdr[0], cMode) + " ";
    out += span((unsigned char)hdr[1], cWord) + " ";

    // readSize (2B)
    out += span((unsigned char)hdr[2], cRead) + " ";
    out += span((unsigned char)hdr[3], cRead) + " ";

    // transfer_size (1B)
    out += span((unsigned char)hdr[4], cXfer) + " ";

    // txLen (2B)
    out += span((unsigned char)hdr[5], cTxLen) + " ";
    out += span((unsigned char)hdr[6], cTxLen) + " ";

    // threshold (4B)
    out += span((unsigned char)hdr[7],  cThr) + " ";
    out += span((unsigned char)hdr[8],  cThr) + " ";
    out += span((unsigned char)hdr[9],  cThr) + " ";
    out += span((unsigned char)hdr[10], cThr) + " ";

    // port, pin
    out += span((unsigned char)hdr[11], cPortPin) + " ";
    out += span((unsigned char)hdr[12], cPortPin);

    return out.trimmed();
}

QString SerialMonitor::coloredBulkData(const QByteArray& payload) const
{
    const QString cTxData = "#2980b9";

    auto hx = [](unsigned char b){
        return QString("%1").arg(b,2,16,QLatin1Char('0')).toUpper();
    };
    auto span = [&](unsigned char b){
        return QString("<span style='color:%1'>%2</span>").arg(cTxData, hx(b));
    };

    QString out; out.reserve(payload.size() * 12);
    for (unsigned char b : payload) {
        if (!out.isEmpty()) out += ' ';
        out += span(b);
    }
    return out;
}

QString SerialMonitor::coloredBulkEnd() const
{
    return QString();
}
