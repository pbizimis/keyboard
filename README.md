# A 36-key keyboard

About two years ago I switched to a 36-key keyboard and I have not looked back since. So I thought, why not make my ideal keyboard hardware and software? The keyboard hardware and software are in an early stage but the keyboard is in use as my primary keyboard. Before, I was using a 36-key split bluetooth keyboard that runs ZMK. This new one feels a lot better. Typing feels instant, I don't notice any latency. Of course, there are a few bugs.

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

## To Do
### Software
- Keys should not reactivate once a mod key is pressed
- OLEDs should be turned off by key combination to avoid burn in
- I need add an option for mod keys that allows keys to be pressed and held until the next key is pressed.
### Hardware
- I really need to redo the CAD model since there are some design flaws
- Building the keyboard was painful. So the next change should be a flex PCB


  ## Documentation

  #### MCU: Raspberry Pi Pico
  ...

  #### Communication
The two halves communicate over UART. Power, GND, and UART are connected over a USB-C cable. I am not using the USB protocol for this. I am splitting the USB lines on both ends and only connecting power, GND, UART RX, and UART TX.

The primary half communicates with a PC over USB (TinyUSB).
