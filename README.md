
# Earthcontainer Deliverable

## Project Goal

This firmware demonstrates a robust, modular approach to IoT telemetry for STM32 microcontrollers using FreeRTOS. The system:

- Handles hardware-interrupt push-button events.
- Publishes the button’s state to a cloud endpoint every five seconds via cellular HTTP.
- In parallel, generates a random integer (1–5), publishes it, and controls an onboard LED for that duration.
- Transmits a JSON payload containing device ID, button state, LED state, and random delay.

## Features & Requirements

- **Button Interrupts:** Hardware interrupts capture button events, which are queued for processing.
- **Periodic Publishing:** Every 5 seconds, the button state is published to a cloud endpoint (HTTP over cellular).
- **Random Delay & LED Control:** A parallel task generates a random delay, publishes it, and signals the LED task to keep the LED on for the specified time.
- **JSON Telemetry:** All telemetry is sent as a JSON payload: `{ "device_id": ..., "button_state": ..., "led_state": ..., "random_delay": ... }`

## Handling Ambiguous Requirements

- **Continuous Random Generation:** The random task generates new values periodically, not just on boot, ensuring continuous telemetry.
- **Complete Payload on Button Interrupt:** The publisher task maintains the latest values for button, LED, and random delay, so we can send complete payloads on button press with up-to-date status.

## Firmware Architecture

The firmware is based on a **state machine pattern** and **active object design** using FreeRTOS tasks and queues for inter-task communication. Key architectural choices:

- **Active Objects:** Each functional block (button, random, LED, publisher, modem) runs in its own RTOS task, communicating via message queues.
- **State Machines:** The modem task uses a state machine to manage initialization, registration, HTTP configuration, and telemetry transmission.
- **Interrupt-driven Design:** Hardware interrupts (button, UART RX) are handled asynchronously and safely passed to tasks via queues.
- **Inter-task Communication:** Queues decouple tasks, allowing for modular, testable, and scalable design.


## Communication Protocols & Solutions

- **Cellular HTTP:** Chosen for reliability and compatibility with cloud endpoints. The modem task manages AT command sequences for HTTP configuration and telemetry POSTs.
	The modem used in this project is the Murata 1SC, which supports multiple cellular modes (NB‑IoT, LTE‑M and NTN). For this deliverable we use NB‑IoT (Narrowband IoT) because of its low-power, wide-area characteristics and good coverage for telemetry-style payloads.
- **JSON Payloads:** Standardized, human-readable, and cloud-friendly format for telemetry.
- **UART Debugging:** All TX/RX is mirrored to UART2 for easy debugging and traceability.

## Design Decisions & Trade-offs

- **RTOS vs. Super-loop:** RTOS tasks and queues were chosen over a super-loop for better modularity, concurrency, and maintainability.
- **State Machine for Modem:** Explicit states simplify error handling and recovery, making cellular communication robust.
- **Dynamic Memory for Payloads:** Publisher and UART RX use malloc/free for message buffers, ensuring safe, non-blocking queue transfers.
- **Queue-based Decoupling:** Each task only knows its own logic and queue interface, making the system easy to extend or modify.

## System Flow (see `mermaid-diagram.mmd`)

The included Mermaid diagram visualizes the initialization, task state machines, and inter-task communication.

## How to Build & Run

1. Build the project using STM32CubeIDE or your preferred toolchain.
2. Flash to a compatible STM32 board with cellular modem.
3. Connect UART2 (ST-Link2) for debug output.
4. Press the button and observe telemetry sent to the configured HTTP endpoint.

## License

See LICENSE for details.