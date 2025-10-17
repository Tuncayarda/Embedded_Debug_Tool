#ifndef ACTION_H
#define ACTION_H

#include <QtCore/qstringview.h>
#include <cstdint>
#include <vector>

/**
 * @brief Logical level used by GPIO-related actions.
 *
 * - UNDEFINED: Not specified (used as a sentinel during authoring).
 * - LOW/HIGH : Concrete logic levels.
 */
enum class Level  : std::int8_t { UNDEFINED, LOW, HIGH };

/**
 * @brief High-level action kind selector for serialization / dispatch.
 *
 * START       : Synthetic root; triggers its targets immediately.
 * DELAY       : Waits for a time duration, then completes.
 * PIN_READ    : (Reserved) Read/monitor a pin for a duration (host-side modeling).
 * PIN_WRITE   : Drive a pin to a level for a duration, then optionally to a final level.
 * PIN_TRIGGER : Wait until a pin reaches a target level or times out.
 * CSPI        : Configure and run a SPI-slave (CSPI) exchange (header + stream).
 */
enum class Kind   : std::int8_t { START, DELAY, PIN_READ, PIN_WRITE, PIN_TRIGGER, CSPI };

/**
 * @brief Base action node for the editor / encoder.
 *
 * - id           : Unique, dense identifier used as array index on device.
 * - kind         : Concrete type discriminator (mirrors union on device).
 * - runAfterMe   : List of action IDs that should start after this completes.
 *
 * Notes:
 * - Derived classes add type-specific parameters.
 * - Kept polymorphic (virtual dtor) for safe downcasting via kind.
 */
class Action {
public:
    int  id{0};
    Kind kind{Kind::START};

    std::vector<int> runAfterMe;

    virtual ~Action() = default;
};

/**
 * @brief Add a dependency edge: when @a completes, start @b.
 *
 * @param a Producer action.
 * @param b Consumer action that should run after @a.
 *
 * Implementation detail:
 * - Stores only the target's ID; encoder will resolve to the device graph.
 */
inline void linkRunAfter(Action& a, Action& b) {
    a.runAfterMe.push_back(b.id);
}

/**
 * @brief Synthetic start node. Triggers its children immediately.
 *
 * Device mapping:
 * - Encoded as TYPE_START with target list = runAfterMe.
 */
class StartAction : public Action {
public:
    StartAction() { kind = Kind::START; }
};

/**
 * @brief Time delay action.
 *
 * Parameters:
 * - durationMs / durationUs : Delay interval (composed to device ticks).
 *
 * Device mapping:
 * - Encoded as TYPE_DELAY, followed by target list.
 */
class DelayAction : public Action {
public:
    uint32_t durationMs{0};
    uint16_t durationUs{0};
    DelayAction() { kind = Kind::DELAY; }
};

/**
 * @brief (Reserved/host-side) Pin read/monitor action.
 *
 * Parameters:
 * - port, pin      : GPIO selector.
 * - initial        : Expected initial level (for validation/logging).
 * - target         : Desired/observed level during the window.
 * - final          : Expected level at the end of the window.
 * - durationMs/Us  : Observation window.
 *
 * Device mapping:
 * - Mirrors TYPE_PIN_READ structure (may be used for validation/preview).
 */
class PinReadAction : public Action {
public:
    int   port{-1};
    int   pin{-1};
    Level initial{Level::UNDEFINED};
    Level final{Level::UNDEFINED};
    Level target{Level::UNDEFINED};
    uint32_t durationMs{0};
    uint16_t durationUs{0};
    PinReadAction() { kind = Kind::PIN_READ; }
};

/**
 * @brief Drive a GPIO pin for a duration.
 *
 * Parameters:
 * - port, pin      : GPIO selector.
 * - initial        : Level expected before starting (for authoring checks).
 * - target         : Level to drive during the active window.
 * - final          : Level to set when the window ends.
 * - durationMs/Us  : Active window length.
 *
 * Device mapping:
 * - Encoded as TYPE_PIN_WRITE with timing and level fields.
 */
class PinWriteAction : public Action {
public:
    int   port{-1};
    int   pin{-1};
    Level initial{Level::UNDEFINED};
    Level final{Level::UNDEFINED};
    Level target{Level::UNDEFINED};
    uint32_t durationMs{0};
    uint16_t durationUs{0};
    PinWriteAction() { kind = Kind::PIN_WRITE; }
};

/**
 * @brief Wait for a pin to reach a target level within a timeout.
 *
 * Parameters:
 * - port, pin      : GPIO selector.
 * - initial        : Expected initial level (optional).
 * - target         : Level to wait for (LOW/HIGH).
 * - timeoutMs/Us   : Maximum wait duration.
 *
 * Device mapping:
 * - Encoded as TYPE_PIN_TRIGGER; completes early on level match or errors on timeout.
 */
class PinTriggerAction : public Action {
public:
    int   port{-1};
    int   pin{-1};
    Level initial{Level::UNDEFINED};
    Level target{Level::UNDEFINED};
    uint32_t   timeoutMs{0};
    uint16_t   timeoutUs{0};
    PinTriggerAction() { kind = Kind::PIN_TRIGGER; }
};

/**
 * @brief Configure and run a CSPI (SPI slave) transaction sequence.
 *
 * Parameters:
 * - mode           : SPI mode (0..3).
 * - wordSize       : Bits per frame; device currently supports 8.
 * - readSize       : Number of bytes to capture from MISO (if any).
 * - transfer_size  : Bytes per "round" (1..4 used for threshold compare logic).
 * - port, pin      : GPIO used as an indicator/output (e.g., LED).
 * - threshold      : 32-bit compare value against last @transfer_size TX bytes.
 * - txData         : Streamed payload that will be fed to the device ring.
 *
 * Device mapping:
 * - Header sent via MSG_ID_CSPI_BEGIN.
 * - Payload streamed with MSG_ID_CSPI_DATA (multiple 512-B chunks).
 * - Session closed with MSG_ID_CSPI_END or MSG_ID_CSPI_TERMINATE.
 */
class CSPIAction : public Action {
public:
    int mode{-1};
    int wordSize{-1};
    int readSize{-1};
    int transfer_size{-1};
    int port{-1};
    int pin{-1};
    quint64 threshold{0};
    QByteArray txData;

    CSPIAction() { kind = Kind::CSPI; }
};

#endif // ACTION_H
