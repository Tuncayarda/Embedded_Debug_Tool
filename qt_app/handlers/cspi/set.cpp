#include "../../cspiwindow.h"
#include "../../ui_cspiwindow.h"
#include "../../action.h"
#include "../../actionEncoder.h"
#include <QScrollBar>

/*
 * Helper: parseHexString
 * ----------------------
 * Converts a multi-line hex-text into a raw byte array according to a fixed
 * transfer size per line.
 *
 * Expected input format:
 *   - Each non-empty line must end with a semicolon ';'.
 *   - Between the line start and the semicolon there must be exactly
 *     `transferSize` 2-hex-digit tokens separated by whitespace.
 *   - Example for transferSize = 3:
 *       "AA BB CC;"
 *
 * Validation:
 *   - Missing semicolon, wrong token count, or non-hex tokens => failure.
 *
 * @param s             Source string (possibly multi-line).
 * @param transferSize  Number of bytes expected per line.
 * @param ok            Out flag set true on success, false on any parse error.
 *
 * @return Concatenated bytes of all valid lines (empty on error).
 */
static QByteArray parseHexString(const QString& s, int transferSize, bool* ok) {
    if (ok) *ok = false;

    QByteArray out;
    QStringList lines = s.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);

    for (int ln = 0; ln < lines.size(); ++ln) {
        QString line = lines[ln].trimmed();
        if (line.isEmpty()) continue;

        // Every data line must terminate with ';'
        if (!line.endsWith(';')) {
            return {};
        }
        line.chop(1); // remove trailing ';'

        // Split into whitespace-separated tokens
        QStringList tokens = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (tokens.size() != transferSize) {
            return {};
        }

        // Parse each token as a 2-digit hex byte
        for (const QString& tok : tokens) {
            if (tok.size() != 2) return {};
            bool byteOk = false;
            ushort v = tok.toUShort(&byteOk, 16);
            if (!byteOk) return {};
            out.append(char(v & 0xFF));
        }
    }

    if (ok) *ok = true;
    return out;
}

/*
 * Helper: parseThresholdText
 * --------------------------
 * Parses a threshold value expressed as 1..4 space-separated hex bytes
 * (each token exactly two hex digits), e.g. "AA BB CC DD".
 *
 * Semantics:
 *   - Empty string => success with value 0.
 *   - >4 bytes or any malformed token => error (returns 0, ok=false).
 *   - Value is big-endian assembled: b0 b1 b2 b3 -> (((b0<<8)+b1)<<8)+...
 *
 * @param s   Threshold text.
 * @param ok  Out flag set true on success, false on error.
 *
 * @return Parsed value as quint64 (0..0xFFFFFFFF).
 */
static quint64 parseThresholdText(const QString& s, bool* ok)
{
    if (ok) *ok = false;

    QString t = s.trimmed();
    if (t.isEmpty()) {
        if (ok) *ok = true;
        return 0;
    }

    QStringList tokens = t.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    QByteArray bytes;
    for (const QString& tok : tokens) {
        if (tok.size() != 2) {
            return 0;
        }
        bool byteOk = false;
        ushort v = tok.toUShort(&byteOk, 16);
        if (!byteOk) return 0;
        bytes.append(char(v & 0xFF));
    }
    if (bytes.size() > 4) {
        return 0;
    }

    quint64 value = 0;
    for (unsigned char b : bytes) {
        value = (value << 8) | quint64(b);
    }

    if (ok) *ok = true;
    return value;
}

/*
 * Helper: escapeHtml
 * ------------------
 * Minimal HTML escaping for &, <, > so plain text can be embedded in HTML.
 */
static QString escapeHtml(QString s) {
    s.replace('&',"&amp;").replace('<',"&lt;").replace('>',"&gt;");
    return s;
}

/*
 * Helper: spanTok
 * ---------------
 * Wraps a token in an HTML <span> with optional color, and uppercases it.
 * If colorHex is empty, returns the token HTML-escaped without a span.
 */
static QString spanTok(const QString& tok, const QString& colorHex)
{
    if (colorHex.isEmpty())
        return tok.toHtmlEscaped().toUpper();
    return "<span style='color:" + colorHex + "'>" +
           tok.toHtmlEscaped().toUpper() + "</span>";
}

/*
 * buildHighlightedHtml
 * --------------------
 * Produces a syntax-highlighted HTML view of the CSPI TX text area content.
 *
 * Rules:
 *   - Each line is expected to be: "<B0> <B1> ... <Bn>;" where n+1 == tsize.
 *   - Invalid lines (wrong format or counts) are emitted as plain escaped text.
 *   - For valid lines, the byte group is combined (big-endian) into an integer
 *     and compared against `thr`. If value > thr, the entire line's tokens are
 *     colored (default red #e74c3c).
 *
 * Counters:
 *   - countTotal: number of valid data lines seen.
 *   - countOver:  number of valid data lines whose value exceeded the threshold.
 *
 * @param plain       Original plain-text.
 * @param tsize       Transfer size (bytes per line).
 * @param thr         Threshold value.
 * @param countOver   Optional out count of “over threshold” lines.
 * @param countTotal  Optional out count of valid data lines.
 *
 * @return HTML string suitable for QTextEdit::setHtml().
 */
QString buildHighlightedHtml(const QString& plain,
                             int tsize,
                             quint64 thr,
                             int* countOver,
                             int* countTotal)
{
    if (countOver)  *countOver  = 0;
    if (countTotal) *countTotal = 0;

    // Small lambda to parse a 2-hex-digit token
    auto parseTokByte = [](const QString& tok, bool* ok)->int {
        if (ok) *ok = false;
        if (tok.size() != 2) return 0;
        bool okLocal=false;
        int v = tok.toInt(&okLocal, 16);
        if (ok) *ok = okLocal;
        return okLocal ? v : 0;
    };

    QString out;
    out += "<div style='font-family:Menlo,monospace; font-size:12px; white-space:pre-wrap;'>";

    const QStringList lines = plain.split(QRegularExpression("[\r\n]"), Qt::KeepEmptyParts);
    for (int i = 0; i < lines.size(); ++i)
    {
        const QString origLine = lines[i];

        // Preserve empty lines
        if (origLine.isEmpty()) {
            if (i+1 < lines.size()) out += "<br/>";
            continue;
        }

        QString t = origLine.trimmed();

        const bool hasSemi = t.endsWith(';');
        if (!hasSemi || tsize <= 0) {
            // Not a data line, render as-is
            out += escapeHtml(origLine);
            if (i+1 < lines.size()) out += "<br/>";
            continue;
        }
        t.chop(1); // remove ';'

        QStringList toks = t.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (toks.size() != tsize) {
            // Wrong count: render original line escaped
            out += escapeHtml(origLine);
            if (i+1 < lines.size()) out += "<br/>";
            continue;
        }

        // Parse bytes
        bool allOk = true;
        quint64 val = 0;
        QList<int> bytes; bytes.reserve(tsize);
        for (const QString& tok : toks) {
            bool ok=false;
            int b = parseTokByte(tok, &ok);
            if (!ok) { allOk = false; break; }
            bytes.append(b & 0xFF);
        }
        if (!allOk) {
            out += escapeHtml(origLine);
            if (i+1 < lines.size()) out += "<br/>";
            continue;
        }

        if (countTotal) (*countTotal)++;

        // Assemble big-endian value
        for (int b : bytes)
            val = (val << 8) | quint64(b);

        const bool over = (val > thr);
        if (over && countOver) (*countOver)++;

        // Highlight tokens if over-threshold
        const QString color = over ? "#e74c3c" : QString();

        QString lineHtml;
        for (int k = 0; k < toks.size(); ++k) {
            if (k) lineHtml += ' ';
            lineHtml += spanTok(toks[k], color);
        }
        lineHtml += ";";

        out += lineHtml;

        if (i+1 < lines.size()) out += "<br/>";
    }

    out += "</div>";
    return out;
}

/*
 * CSPIWindow::updateTxHighlight
 * -----------------------------
 * Re-renders the TX data text edit with HTML highlighting based on:
 *   - transfer size (bytes per line)
 *   - threshold (parsed from line edit)
 *
 * Keeps user experience smooth:
 *   - Avoids redundant setHtml() if content didn't change.
 *   - Preserves selection (anchor/position) and scroll position.
 *   - Updates a small label with "<valid>/<over>" counts.
 */
void CSPIWindow::updateTxHighlight()
{
    bool thrOk = false;
    const quint64 thr = parseThresholdText(ui->lineEdit_threshold->text(), &thrOk);
    const int tsize   = ui->spinBox_transfer_size->value();
    const QString src = ui->textEdit_tx_data->toPlainText();
    const quint64 effThr = thrOk ? thr : std::numeric_limits<quint64>::max();

    int countOver  = 0;
    int countTotal = 0;
    const QString html = buildHighlightedHtml(src, tsize, effThr, &countOver, &countTotal);

    // Skip repaint if the HTML would be identical (avoids flicker)
    static QString s_lastHtml;
    if (html == s_lastHtml)
        return;
    s_lastHtml = html;

    // Preserve cursor and scroll
    QTextCursor cur = ui->textEdit_tx_data->textCursor();
    const int pos    = cur.position();
    const int anchor = cur.anchor();
    QScrollBar *vbar = ui->textEdit_tx_data->verticalScrollBar();
    const int scroll = vbar ? vbar->value() : 0;

    const QSignalBlocker blocker(ui->textEdit_tx_data);
    ui->textEdit_tx_data->setHtml(html);

    // Restore selection
    const int docLen = ui->textEdit_tx_data->toPlainText().size();
    int newPos    = qMin(pos,    docLen);
    int newAnchor = qMin(anchor, docLen);

    QTextCursor nc = ui->textEdit_tx_data->textCursor();
    nc.setPosition(newAnchor);
    nc.setPosition(newPos, QTextCursor::KeepAnchor);
    ui->textEdit_tx_data->setTextCursor(nc);

    // Restore scroll
    if (vbar) vbar->setValue(scroll);

    // Update counts label (format: "<valid>/<over>")
    ui->label_th_vals->setText(QString("%1/%2").arg(countTotal).arg(countOver));
}

/*
 * Helper: wordSizeFromCombo
 * -------------------------
 * Maps combo index to SPI word size in bits.
 * (UI contract: indices 0..4 map to {4, 8, 16, 32, 64}.)
 */
static int wordSizeFromCombo(const QComboBox* cb) {
    switch (cb->currentIndex()) {
    case 0: return 4;
    case 1: return 8;
    case 2: return 16;
    case 3: return 32;
    case 4: return 64;
    default: return 8;
    }
}

/*
 * CSPIWindow::on_setButton_clicked
 * --------------------------------
 * Collects CSPI configuration from the UI, validates fields, encodes the
 * header + TX data into the device protocol payload, and emits `payloadReady`.
 *
 * Steps:
 *   1) Read mode (0..3), transfer size (bytes), word size (bits), and read size.
 *   2) Parse threshold text into a 1..4-byte big-endian value; show inline
 *      placeholder error on failure.
 *   3) Read port/pin selection.
 *   4) Parse TX data text via `parseHexString`, enforcing exact per-line token
 *      count (== transfer size) and trailing ';'. Show inline placeholder on
 *      error with an example format.
 *   5) Encode CSPI header + payload via `encodeCSPIPayload` and emit signal.
 */
void CSPIWindow::on_setButton_clicked()
{
    CSPIAction a;

    // SPI mode (0..3)
    if (ui->radioButton_mode_0->isChecked()) a.mode = 0;
    else if (ui->radioButton_mode_1->isChecked()) a.mode = 1;
    else if (ui->radioButton_mode_2->isChecked()) a.mode = 2;
    else if (ui->radioButton_mode_3->isChecked()) a.mode = 3;

    // Bytes per transfer (comparison window width)
    a.transfer_size = ui->spinBox_transfer_size->value();

    // Bits per frame
    a.wordSize = wordSizeFromCombo(ui->comboBox_word);

    // Optional RX size (if slave expects to capture N bytes)
    a.readSize = ui->spinBox_read_size->text().toInt();

    // Threshold (1..4 hex bytes, big-endian)
    bool thrOk = false;
    a.threshold = parseThresholdText(ui->lineEdit_threshold->text(), &thrOk);
    if (!thrOk) {
        ui->lineEdit_threshold->clear();
        ui->lineEdit_threshold->setPlaceholderText("Error!");
        return;
    }

    // Output pin for threshold indication (port/pin)
    a.port = ui->comboBox_pin_port->currentIndex();
    a.pin  = ui->spinBox_pin_no->value();

    // TX data (multi-line hex, each line exactly transfer_size tokens + ';')
    bool ok = false;
    const QString txStr = ui->textEdit_tx_data->toPlainText();
    a.txData = parseHexString(txStr, a.transfer_size, &ok);
    if (!ok) {
        ui->textEdit_tx_data->clear();
        ui->textEdit_tx_data->setPlaceholderText(
            QString("Error: Each line must have %1 bytes, example:\n")
                .arg(a.transfer_size) +
            (a.transfer_size == 1 ? "AA;" : "AA BB ...;")
            );
        return;
    }

    // Build device payload and notify the main window / transport layer
    std::vector<std::uint8_t> raw = encodeCSPIPayload(a);
    QByteArray payload(reinterpret_cast<const char*>(raw.data()), int(raw.size()));
    emit payloadReady(payload);
}
