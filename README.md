# BEAMO

BEAMO is a simple, low-cost RF remote controller for robots and toys based on the common nRF24L01+ and its more ubiquitous counterfeits. 32 may be operated in the same space by selecting different channels using the configuration DIP switch.

The board itself is a simple 2-layer PCB designed to be operated without an enclosure. There are mounting holes to allow attachment of a custom grip.

Example receiver code has been tested on an Arduino Uno using a matching nRF24L01+ module.

# Firmware

The remote is intended for real-time control of a vehicle. As such, it sends button updates at a minimum of every 100ms. When the data changes, it may send as frequently as every 10ms.

The remote firmware was built in PlatformIO (for Raspberry Pi Pico) but should also be portable to the Arduino IDE. The receiver example for Arduino Uno is built in the Arduino IDE.

# BOM

| Reference | Name | Approx Cost | Description |
| :-:       | :--  | :-: | :-- |
| RZ1 | [RP2040-Zero](https://www.aliexpress.us/item/3256804095235134.html) | $3.25 | Inexpensive RP2040-based microcontroller board |
| J1 | [nRF24L01+ module](https://www.amazon.com/gp/product/B082VLTTTX) | $2.60/pair | RF module (affixed with [2x4 0.1" connector](https://www.amazon.com/dp/B01IHBCO2K)) |
| SW1-4, SW6-8 | [TS02-66-50-BK-100-SCR-D](https://www.digikey.com/en/products/detail/cui-devices/TS02-66-50-BK-100-SCR-D/15634357)| $0.56 per 7 | Pushbuttons (low-force) |
| SW10 | [DS04-254-1L-05BK](https://www.digikey.com/en/products/detail/cui-devices/DS04-254-1L-05BK/11310878) | $0.64 | 5-position DIP switch |
| SW5 | [PS4 controller joystick](https://www.amazon.com/dp/B09WDNSBBR) | $1.30 | Generic PS4 joystick (ALPS TKJXV1224-like) |
| SW9 | [0.1" SPDT switch](https://www.amazon.com/dp/B09R43HCY3) | $0.10 | Slide switch (for power) |
| BT1 | [2xAA battery holder](https://www.amazon.com/dp/B013GNC08C) | $0.75 | Battery holder (held on with double-sided tape) |
