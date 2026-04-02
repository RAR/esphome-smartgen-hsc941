# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESPHome external component for the **SmartGen HSC941** generator controller. Communicates over Modbus RTU/RS485 using a custom implementation built on the ESP-IDF UART driver (not the built-in ESPHome modbus stack). Targets ESP32-S3.

## Build & Deploy

This is an ESPHome component — there is no standalone build system. To compile and flash:

```bash
esphome compile smartgen_hsc941.yaml
esphome run smartgen_hsc941.yaml        # compile + flash + monitor
esphome logs smartgen_hsc941.yaml       # serial/network log monitor
```

There are no tests, linters, or CI pipelines in this repo.

## Architecture

Two ESPHome components, both under `components/`:

### `smartgen_hsc941` — Modbus RTU driver (hub)
- **`__init__.py`** — ESPHome config schema and `to_code()` for the hub component
- **`sensor.py`** — 59 analog sensors from holding registers (voltages, currents, temps, power, runtime)
- **`binary_sensor.py`** — 44 status flags from coil reads (alarms, shutdowns, warnings, modes)
- **`button.py`** — 7 remote control buttons via coil writes (start, stop, auto, manual, gen switch, fault reset)
- **`smartgen_hsc941.h/.cpp`** — C++ implementation: UART init, CRC-16, Modbus FC01/FC03/FC05, polling loop, FreeRTOS mutex

The hub reads 88 coils (addresses 0–87) and 59 holding registers (0x0007–0x0041) each update cycle (default 5s). After 5 consecutive failures it reports disconnected.

### `smartgen_hsc941_web` — Web dashboard companion
- **`__init__.py`** — Config schema with 20+ parameters (port, PIN, fuel tracking, relay IDs, sensors)
- **`smartgen_hsc941_web.h/.cpp`** — Self-contained HTTP server on `esp_http_server`, serves a single-page dark-themed dashboard with live updates, REST API (`GET /api/status`, `POST /api/command`), NVS persistence, event log, relay thermostat, exercise scheduler

The web component references the hub via `smartgen_hsc941_id` and calls its accessor methods (`get_engine_rpm()`, `build_sensors_json()`, `is_engine_running()`, etc.).

## Key Conventions

- **Framework:** ESP-IDF only (not Arduino). All C++ uses ESP-IDF APIs (`driver/uart.h`, `esp_http_server`, `nvs_flash`).
- **Python files** follow ESPHome component conventions: `CONFIG_SCHEMA`, `async def to_code(config)`, platform registration via `SENSOR_SCHEMA`/`BINARY_SENSOR_SCHEMA`.
- **Register addresses** come from `docs/HSC941_Protocol_en.pdf`. When fixing register mappings, verify against that document.
- **Sensor definitions** in Python use `cv.Optional()` — every sensor/binary_sensor is opt-in via YAML config.
- The web component embeds all HTML/CSS/JS as C++ string literals in the `.cpp` file (no external assets).

## Hardware Context

Reference board: Waveshare ESP32-S3-ETH-8DI-8RO (16 MB flash, 8 MB PSRAM, built-in RS485 on GPIO17 TX / GPIO18 RX). The `partitions.csv` provides a custom partition layout for 16 MB flash with dual OTA slots.
