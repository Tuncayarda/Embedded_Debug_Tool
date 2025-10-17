#include "../../actionEncoder.h"
#include <stdexcept>

/*
 * Encoding utilities for the Qt-side protocol builder.
 * ----------------------------------------------------
 * - Converts high-level Action objects into a compact byte stream understood
 *   by the device firmware.
 * - Provides helpers for big-endian integer appends and small validation.
 * - Also builds framed UART packets (SOF + CORE + CRC16).
 */

/* Map UI-level logic level to protocol encoding:
 *   LOW  -> 0x00
 *   HIGH -> 0x01
 *   UNDEFINED -> 0xFF
 */
std::uint8_t levelToU8(Level l) {
    switch (l) {
    case Level::LOW:  return 0x00;
    case Level::HIGH: return 0x01;
    case Level::UNDEFINED: default: return 0xFF;
    }
}

/* Append a raw 8-bit value. */
void appendU8(std::vector<std::uint8_t>& out, std::uint8_t v) { out.push_back(v); }

/* Append a 16-bit integer in big-endian order. */
void appendU16BE(std::vector<std::uint8_t>& out, std::uint16_t v) {
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
    out.push_back(static_cast<std::uint8_t>(v & 0xFF));
}

/* Append a 32-bit integer in big-endian order. */
void appendU32BE(std::vector<std::uint8_t>& out, std::uint32_t v) {
    out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFF));
    out.push_back(static_cast<std::uint8_t>((v >>  8) & 0xFF));
    out.push_back(static_cast<std::uint8_t>( v        & 0xFF));
}

/* Validate an Action ID is within protocol range [0..255]; return as u8. */
std::uint8_t clampIdToU8(int id) {
    if (id < 0 || id > 255) throw std::runtime_error("Action.id out of 0..255");
    return static_cast<std::uint8_t>(id);
}

/* Append a target list (fan-out/dependencies) as:
 *   [count:u8][id0:u8][id1:u8]...
 */
void appendTargets(std::vector<std::uint8_t>& out, const std::vector<int>& ids) {
    if (ids.size() > 255) throw std::runtime_error("TARGET_COUNT > 255");
    appendU8(out, static_cast<std::uint8_t>(ids.size()));
    for (int id : ids) {
        if (id < 0 || id > 255) throw std::runtime_error("Target id out of 0..255");
        appendU8(out, static_cast<std::uint8_t>(id));
    }
}

/* START action encoding:
 *   [TYPE=0x01][ID][TCOUNT][TIDS...]
 */
void packStart(std::vector<std::uint8_t>& out, const StartAction& a) {
    appendU8(out, TYPE_START);
    appendU8(out, clampIdToU8(a.id));
    appendTargets(out, a.runAfterMe);
}

/* DELAY action encoding:
 *   [TYPE=0x02][ID][DUR_MS:4BE][DUR_US:2BE][TCOUNT][TIDS...]
 */
void packDelay(std::vector<std::uint8_t>& out, const DelayAction& a) {
    appendU8(out, TYPE_DELAY);
    appendU8(out, clampIdToU8(a.id));
    appendU32BE(out, static_cast<std::uint32_t>(a.durationMs));
    appendU16BE(out, static_cast<std::uint16_t>(a.durationUs));
    appendTargets(out, a.runAfterMe);
}

/* PIN_READ action encoding:
 *   [TYPE=0x03][ID][PORT][PIN][INIT][TARGET][FINAL][DUR_MS:4BE][DUR_US:2BE][TCOUNT][TIDS...]
 */
void packPinRead(std::vector<std::uint8_t>& out, const PinReadAction& a) {
    if (a.pin < 0 || a.pin > 255) throw std::runtime_error("PinReadAction.pin out of 0..255");

    appendU8(out, TYPE_PIN_READ);
    appendU8(out, clampIdToU8(a.id));
    appendU8(out, static_cast<std::uint8_t>(a.port));
    appendU8(out, static_cast<std::uint8_t>(a.pin));
    appendU8(out, levelToU8(a.initial));
    appendU8(out, levelToU8(a.target));
    appendU8(out, levelToU8(a.final));
    appendU32BE(out, static_cast<std::uint32_t>(a.durationMs));
    appendU16BE(out, static_cast<std::uint16_t>(a.durationUs));
    appendTargets(out, a.runAfterMe);
}

/* PIN_WRITE action encoding:
 *   [TYPE=0x04][ID][PORT][PIN][INIT][TARGET][FINAL][DUR_MS:4BE][DUR_US:2BE][TCOUNT][TIDS...]
 */
void packPinWrite(std::vector<std::uint8_t>& out, const PinWriteAction& a) {
    if (a.pin < 0 || a.pin > 255) throw std::runtime_error("PinWriteAction.pin out of 0..255");

    appendU8(out, TYPE_PIN_WRITE);
    appendU8(out, clampIdToU8(a.id));
    appendU8(out, static_cast<std::uint8_t>(a.port));
    appendU8(out, static_cast<std::uint8_t>(a.pin));
    appendU8(out, levelToU8(a.initial));
    appendU8(out, levelToU8(a.target));
    appendU8(out, levelToU8(a.final));
    appendU32BE(out, static_cast<std::uint32_t>(a.durationMs));
    appendU16BE(out, static_cast<std::uint16_t>(a.durationUs));
    appendTargets(out, a.runAfterMe);
}

/* PIN_TRIGGER action encoding:
 *   [TYPE=0x05][ID][PORT][PIN][INIT][TARGET][TIMEOUT_MS:4BE][TIMEOUT_US:2BE][TCOUNT][TIDS...]
 */
void packPinTrigger(std::vector<std::uint8_t>& out, const PinTriggerAction& a) {
    if (a.pin < 0 || a.pin > 255) throw std::runtime_error("PinTriggerAction.pin out of 0..255");

    appendU8(out, TYPE_PIN_TRIGGER);
    appendU8(out, clampIdToU8(a.id));
    appendU8(out, static_cast<std::uint8_t>(a.port));
    appendU8(out, static_cast<std::uint8_t>(a.pin));
    appendU8(out, levelToU8(a.initial));
    appendU8(out, levelToU8(a.target));
    appendU32BE(out, static_cast<std::uint32_t>(a.timeoutMs));
    appendU16BE(out, static_cast<std::uint16_t>(a.timeoutUs));
    appendTargets(out, a.runAfterMe);
}

/* CRC16-CCITT (poly 0x1021, init 0xFFFF) over a QByteArray. */
quint16 crc16_ccitt(const QByteArray &data) {
    quint16 crc = 0xFFFF;
    for (uchar b : data) {
        crc ^= quint16(b) << 8;
        for (int i = 0; i < 8; ++i)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
    }
    return crc;
}

/* Build a framed UART packet:
 *   SOF:      0xAA 0x55
 *   CORE:     [MSG_ID][LEN_H][LEN_L][PAYLOAD...]
 *   CRC16:    over CORE only (big-endian)
 */
QByteArray buildPacket(quint8 msgId, const QByteArray &payload) {
    QByteArray core; core.reserve(1 + 2 + payload.size());
    core.append(char(msgId));
    const quint16 len = quint16(payload.size());
    core.append(char((len >> 8) & 0xFF));
    core.append(char(len & 0xFF));
    core.append(payload);

    const quint16 crc = crc16_ccitt(core);

    QByteArray pkt; pkt.reserve(2 + core.size() + 2);
    pkt.append(char(SOF0));
    pkt.append(char(SOF1));
    pkt.append(core);
    pkt.append(char((crc >> 8) & 0xFF));
    pkt.append(char(crc & 0xFF));
    return pkt;
}

/* Serialize a vector of polymorphic Action* into the device wire format.
 * Each Action is dispatched by kind and appended to 'payload' back-to-back.
 * (CSPI is not included here; it has its own encoder.)
 */
std::vector<std::uint8_t> encodeActionPayload(const std::vector<Action*>& actions) {
    std::vector<std::uint8_t> payload;
    payload.reserve(actions.size() * 16);

    for (const Action* a : actions) {
        switch (a->kind) {
        case Kind::START:       packStart(payload, *static_cast<const StartAction*>(a)); break;
        case Kind::DELAY:       packDelay(payload, *static_cast<const DelayAction*>(a)); break;
        case Kind::PIN_READ:    packPinRead(payload, *static_cast<const PinReadAction*>(a)); break;
        case Kind::PIN_WRITE:   packPinWrite(payload, *static_cast<const PinWriteAction*>(a)); break;
        case Kind::PIN_TRIGGER: packPinTrigger(payload, *static_cast<const PinTriggerAction*>(a)); break;
        case Kind::CSPI:
            /* CSPI actions are sent via a different path (bulk header+data). */
            break;
        }
    }
    return payload;
}

/* Encode the CSPI session header + TX payload for MSG_ID_CSPI_BEGIN.
 * Layout (must match device side parse_cspi_begin + subsequent data stream):
 *   [mode:1][wordSize:1][readSize:2][transfer_size:1]
 *   [tx_len:2][threshold:4][port:1][pin:1][tx_bytes...]
 */
std::vector<std::uint8_t> encodeCSPIPayload(const CSPIAction& a)
{
    // Copy QByteArray into a std::vector<uint8_t> for contiguous access
    std::vector<uint8_t> txBytes(a.txData.begin(), a.txData.end());

    std::vector<std::uint8_t> payload;
    payload.reserve(
        1  /* mode */
        + 1  /* wordSize */
        + 2  /* readSize */
        + 1  /* transfer_size */
        + 2  /* txData size */
        + 4  /* threshold (32-bit, high bytes ignored if a.threshold > 32-bit) */
        + 1  /* port */
        + 1  /* pin */
        + txBytes.size()
        );

    appendU8    (payload, static_cast<std::uint8_t>(a.mode));
    appendU8    (payload, static_cast<std::uint8_t>(a.wordSize));
    appendU16BE (payload, static_cast<std::uint16_t>(a.readSize));
    appendU8    (payload, static_cast<std::uint8_t>(a.transfer_size));

    if (txBytes.size() > 0xFFFF)
        throw std::runtime_error("txData too large (>65535)");
    appendU16BE (payload, static_cast<std::uint16_t>(txBytes.size()));

    appendU32BE (payload, static_cast<std::uint32_t>(a.threshold)); // only low 32 bits used on device
    appendU8    (payload, static_cast<std::uint8_t>(a.port));
    appendU8    (payload, static_cast<std::uint8_t>(a.pin));

    // Append raw TX stream used by the CSPI producer on the device
    payload.insert(payload.end(), txBytes.begin(), txBytes.end());
    return payload;
}
