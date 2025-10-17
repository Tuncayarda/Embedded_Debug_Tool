---

```markdown
# Embedded Debug Tool

**Embedded Debug Tool** is a unified system that connects an embedded microcontroller firmware with a Qt-based desktop application.  
It allows real-time testing, debugging, and communication with hardware devices through UART, SPI, or other interfaces.

---

## 🔧 Overview

This project provides a complete framework for hardware–software interaction:

- **Qt Desktop Application** – used to manage, trigger, and monitor embedded actions  
- **Firmware** – runs on the microcontroller, executing timed and state-based actions  
- **Common Layer (Planned)** – shared message definitions and CRC validation logic

Both sides communicate through a custom binary protocol with CRC16 integrity checking, supporting sequenced task execution and feedback.

---

## 📁 Project Structure
```

embedded-debug-tool/
│
├── qt_app/           # Qt-based desktop interface (C++/Qt6)
├── firmware/         # MCU firmware source code
├── common/           # Shared protocol headers (planned)
├── docs/             # Diagrams and protocol documentation
└── README.md         # This file

````

---

## ⚙️ Features

- Modular **Action–Set System** for structured test sequences  
- Multi-state execution logic (IDLE → PENDING → RUNNING → DONE)  
- Deadline management and timer-based scheduling  
- CRC16-protected serial communication protocol  
- Easy to extend for new sensors, peripherals, or command types  
- Unified design for both MCU and PC-side debugging

---

## 🚀 Build Instructions

### Qt Desktop Application
```bash
cd qt_app
mkdir build && cd build
cmake ..
cmake --build .
````

### Embedded Firmware

Import the `firmware/` folder into **MCUXpresso**, **STM32CubeIDE**, or build manually:

```bash
cd firmware
cmake -DCMAKE_TOOLCHAIN_FILE=toolchains/arm-gcc.cmake ..
make
```

---

## 🧠 Execution Concept

The firmware uses a cyclic execution loop controlled by PIT timers and GPIO triggers.
Each action progresses through specific states:

```
IDLE → PENDING → RUNNING → DONE
```

The Qt application supervises these transitions and visualizes the process in real time.

A simplified flow of the firmware execution is shown below:

![Execution Flow](docs/execution_flow.png)

---

## 🧱 Tech Stack

| Layer    | Technology                       |
| -------- | -------------------------------- |
| Desktop  | Qt6 (Widgets, SerialPort), CMake |
| Firmware | C/C++, MCU SDK (Kinetis / STM32) |
| Protocol | Custom binary frame with CRC16   |

---

## 🧩 Future Work

* Shared `common/` protocol header for Qt & firmware synchronization
* GUI configuration for complex action sets
* Logging and real-time plotting system
* CI/CD automation for firmware and desktop builds

---
