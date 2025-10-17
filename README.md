# Embedded Debug Tool

An end-to-end toolkit for **developing, testing, and debugging embedded systems**.  
The project pairs a desktop **Qt application** with **microcontroller firmware** to let you craft frames, send/receive over UART/SPI/I²C, visualize logs, validate timing/CRC, and automate repeatable test flows.

---

## Purpose

- Speed up bring-up and regression testing of embedded targets.
- Provide a **repeatable**, scriptable workflow for protocol checks (UART/SPI/I²C).
- Offer clear **visibility** into packets, state, timing, and errors.
- Keep the stack **modular** so new protocols, devices, and views can be added easily.

---

## What It Is (and Isn’t)

**Is:**
- A paired **Host (Qt GUI)** + **Target (Firmware)** system for real hardware debugging.
- A place to define **frames**, **CRC**, **routes**, and **actions/sequences** you can reuse.
- A reference architecture for clean PC–MCU interactions during development.

**Is not:**
- A production device firmware.
- A vendor-specific IDE project template.
- A one-off demo; it’s intended to grow with real projects.

---

## High-Level Architecture

1. **qt_app** (Host)
   - Serial/port manager, packet composer, live log/hex views
   - Sequence/automation runner (send → wait → assert → loop)
   - Parsers, inspectors, and simple performance probes (latency/throughput)

2. **firmware** (Target)
   - Minimal MCU application exposing transport endpoints (UART/SPI/I²C)
   - Frame parsing, routing, and handler callbacks
   - Optional sensors/peripherals to emulate or validate real traffic

Both sides speak a **simple framed protocol** (SOF | PROTO | MSG_ID | LEN | PAYLOAD | CRC16) to stay device-agnostic.

---

## Repository Layout

```text
/
├─ firmware/   → Microcontroller-side code (transport drivers, frame parser, handlers)
└─ qt_app/     → Desktop Qt application (UI, protocol editors, logs, automation)
