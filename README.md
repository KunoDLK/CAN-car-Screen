# Car Dashboard LCD Display

## Overview
A project to develop a car dashboard display using an LCD screen and microcontroller, featuring gauge displays and various connectivity options.

Arduino powder display that will display car data over CAN on to a TFT Display


---

## System Architecture
- **Microcontroller**: STM32 F4 (might be okay with lower grade of MCU)
- **LCD Screen**: Specifications and resolution
  - 7 inch 800x480 TFT panel w/ 40 pin ribbon connection
- **Power Supply**: Details on how the system is powered
  - Usb-C, standard 5v 500mA might be okay for power beget however we might need to think about more power depending on  

---

## Software Design
- **Current Structure**: Overview of existing code.
  - Threads:
    - Display Thread:
      - Drawing to the screen
      - Buffering draw calls
      - Managing the display layers
    - CAN massage thread
      - Receives can bus frames and enqueue them
      - Filter out invalid data
    - Gauge drawing:
      - Updates dynamic UI elements by firing draw calls
    - USB communication:
      - handles communication with host PC

- **Refactoring Plan**: How we’ll improve scalability and configurability.

---

## Hardware Specifications
- **Components**: Microcontroller, LCD, EEPROM 
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
Something like this
![IMG_1250](https://github.com/user-attachments/assets/0b2ec6b9-b187-4029-83ed-054e06b77d36)
  
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
