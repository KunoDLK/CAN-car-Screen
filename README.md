# Car Dashboard LCD Display

## Overview
A project to develop a car dashboard display using an LCD screen and microcontroller, featuring gauge displays and various connectivity options.

Arduino powder display that will display car data over CAN on to a TFT Display
![IMG_1250](https://github.com/user-attachments/assets/0b2ec6b9-b187-4029-83ed-054e06b77d36)

---

## System Architecture
- **Microcontroller**: Arduino
- **LCD Screen**: Specifications and resolution
- **Power Supply**: Details on how the system is powered

---

## Software Design
- **Current Structure**: Overview of existing code.
- **Refactoring Plan**: How we’ll improve scalability and configurability.

---

## Hardware Specifications
- **Components**: Microcontroller, LCD, sensors.
- **Connections**:
  - CAN bus
  - WinUSB (USB connection)
  - Parallel connection to LCD driver


Using:
https://github.com/sumotoy/RA8875 for display driver (Currently included in arduino IDE)

Using:
https://github.com/autowp/arduino-mcp2515 for CAN driver

---

## User Interface
- **Gauge Display**: Layout and design.
- **Interactivity**: Any interactive elements.

---

## Communication Interfaces
- **CAN Bus**: Integration details.
- **USB**: WinUSB specifics.
- **Parallel Connection**: Interface with LCD driver.

---

## Mechanical Design
- **3D Model**: CAD design process.
- **3D Printing**: Materials and specifications.
- **Mounting**: Attaching the case to the dashboard.

---

## Software Architecture
- **RTOS Kernel**: Overview of FreeRTOS.
- **Task Management**: Structuring and prioritisation.

---

## Testing and Validation
- **Methodology**: How we’ll ensure functionality and reliability.
