#ifndef ACTIONENCODER_H
#define ACTIONENCODER_H

#include "action.h"
#include <cstdint>
#include <vector>

/*-----------------------------------------------------------------------------
 * Protocol constants (must match device firmware)
 *-----------------------------------------------------------------------------
 * SOF0/SOF1           : Start-of-frame bytes for UART framing (0xAA, 0x55).
 * MSG_ID_*            : Application message identifiers.
 * MAX_PAYLOAD_SIZE    : Maximum bytes of payload per packet (excluding SOF/CRC).
 * TYPE_*              : On-device action type tags used in the encoded blob.
 *---------------------------------------------------------------------------*/

#define SOF0 0xAA
#define SOF1 0x55

#define MSG_ID_EXECUTE_ACTIONS  0x10   // Send & execute an actions blob immediately

#define MSG_ID_CSPI_BEGIN       0x50   // Begin CSPI session (header)
#define MSG_ID_CSPI_DATA        0x52   // Stream CSPI TX data (512B chunks typically)
#define MSG_ID_CSPI_END         0x54   // Graceful CSPI end; finish remaining work
#define MSG_ID_CSPI_REQ         0x59   // Device → host: please send next data chunk
#define MSG_ID_CSPI_TERMINATE   0x5B   // Abort CSPI session immediately

#define MSG_ID_WRITE_FLASH      0x90   // Store a frame in user flash (no auto-exec)
#define MSG_ID_WRITE_FLASH_BOOT 0x91   // Store & mark as boot-executable frame

#define MSG_ID_RESET            0xFF   // Device software reset

#define MSG_ID_CLEAR_FLASH      0xCC   // Erase user flash region

#define MAX_PAYLOAD_SIZE        512    // Matches device-side parser limit

// Device-side action type tags (wire format)
static constexpr std::uint8_t TYPE_START       = 0x01;
static constexpr std::uint8_t TYPE_DELAY       = 0x02;
static constexpr std::uint8_t TYPE_PIN_READ    = 0x03;
static constexpr std::uint8_t TYPE_PIN_WRITE   = 0x04;
static constexpr std::uint8_t TYPE_PIN_TRIGGER = 0x05;

/*-----------------------------------------------------------------------------
 * Small helpers for byte packing (big-endian where applicable)
 *---------------------------------------------------------------------------*/

/**
 * @brief Convert Level enum to wire format (0=LOW, 1=HIGH, 0xFF=UNDEFINED).
 */
std::uint8_t levelToU8(Level l);

/**
 * @brief Append a single byte to an output buffer.
 */
void appendU8(std::vector<std::uint8_t>& out, std::uint8_t v);

/**
 * @brief Append a 16-bit unsigned integer in big-endian order.
 */
void appendU16BE(std::vector<std::uint8_t>& out, std::uint16_t v);

/**
 * @brief Append a 32-bit unsigned integer in big-endian order.
 */
void appendU32BE(std::vector<std::uint8_t>& out, std::uint32_t v);

/**
 * @brief Clamp an integer ID to 0..255 for wire encoding.
 *
 * The device expects dense 8-bit IDs; editor-side IDs are int.
 */
std::uint8_t clampIdToU8(int id);

/**
 * @brief Append a target list: count byte followed by IDs.
 *
 * Layout: [count:1][id0:1][id1:1]...
 */
void appendTargets(std::vector<std::uint8_t>& out, const std::vector<int>& ids);

/*-----------------------------------------------------------------------------
 * Action-to-bytes packers (host → device wire format)
 * Each function appends a single action record into @out.
 *---------------------------------------------------------------------------*/

/**
 * @brief Encode a StartAction as TYPE_START.
 * Layout: [TYPE][ID][TCOUNT][TIDS...]
 */
void packStart   (std::vector<std::uint8_t>& out, const StartAction& a);

/**
 * @brief Encode a DelayAction as TYPE_DELAY.
 * Layout: [TYPE][ID][DUR_MS:4][DUR_US:2][TCOUNT][TIDS...]
 */
void packDelay   (std::vector<std::uint8_t>& out, const DelayAction& a);

/**
 * @brief Encode a PinReadAction as TYPE_PIN_READ.
 * Layout: [TYPE][ID][PORT][PIN][INIT][TARGET][FINAL][MS:4][US:2][TCOUNT][TIDS...]
 */
void packPinRead (std::vector<std::uint8_t>& out, const PinReadAction& a);

/**
 * @brief Encode a PinWriteAction as TYPE_PIN_WRITE.
 * Layout: [TYPE][ID][PORT][PIN][INIT][TARGET][FINAL][MS:4][US:2][TCOUNT][TIDS...]
 */
void packPinWrite(std::vector<std::uint8_t>& out, const PinWriteAction& a);

/**
 * @brief Encode a PinTriggerAction as TYPE_PIN_TRIGGER.
 * Layout: [TYPE][ID][PORT][PIN][INIT][TARGET][TIMEOUT_MS:4][TIMEOUT_US:2][TCOUNT][TIDS...]
 */
void packPinTrigger(std::vector<std::uint8_t>& out, const PinTriggerAction& a);

/*-----------------------------------------------------------------------------
 * UART framing helpers (CRC + packet builder)
 *---------------------------------------------------------------------------*/

/**
 * @brief Compute CRC-CCITT (0xFFFF seed, poly 0x1021) over @data.
 * Matches device-side crc16_step loop behavior.
 */
quint16     crc16_ccitt(const QByteArray &data);

/**
 * @brief Build a framed UART packet: SOF, MSG, LEN, PAYLOAD, CRC.
 *
 * Frame:
 *  SOF0 SOF1  MSG  LEN_H LEN_L  [PAYLOAD...]  CRC_H CRC_L
 */
QByteArray  buildPacket(quint8 msgId, const QByteArray &payload);

/*-----------------------------------------------------------------------------
 * High-level encoders
 *---------------------------------------------------------------------------*/

/**
 * @brief Encode a set of Action* into the device "actions blob" (payload only).
 *
 * - The caller is responsible for wrapping with a MSG_ID_EXECUTE_ACTIONS packet.
 * - Actions must have unique IDs and consistent runAfterMe references.
 */
std::vector<std::uint8_t> encodeActionPayload(const std::vector<Action*>& actions);

/**
 * @brief Encode CSPI header + separate TX data payload.
 *
 * Return value:
 *  - For header: call encodeCSPIPayload(CSPIAction) and take the first 13 bytes
 *    as the MSG_ID_CSPI_BEGIN payload (mode, wordSize, readSize, transfer_size,
 *    threshold (BE32), port, pin). The remaining bytes (if any) are ignored here.
 *  - Actual stream data (a.txData) should be sent in 512B chunks with MSG_ID_CSPI_DATA.
 */
std::vector<std::uint8_t> encodeCSPIPayload(const CSPIAction& a);

#endif // ACTIONENCODER_H
