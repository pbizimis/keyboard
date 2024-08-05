# A 36-key keyboard

About two years ago I switched to a 36-key keyboard and I have not looked back since. So I thought, why not make my ideal keyboard hardware and software?

Hardware:

- 2x 18 keys
- 2x rotary encoders
- 2x OLED screens (256x64)
- 2x Raspberry Pi Pico

Software:

- low latency
- different applications (handled by the second core)
  - pin protection
  - LLM client (via API, future on-device?)
  - todo tasks
  - timer for focus
  - settings
  - snake


  ## Documentation

  #### MCU: Raspberry Pi Pico
  ...

  #### Communication
The two halves communicate over UART. Power, GND, and UART are connected over a USB-C cable. I am not using the USB protocol for this. I am splitting the USB lines on both ends and only connecting power, GND, UART RX, and UART TX.

The primary half communicates with a PC over USB (TinyUSB).
