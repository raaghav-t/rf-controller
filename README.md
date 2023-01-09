# BEAMO

BEAMO is a simple, low-cost RF remote controller for robots and toys based on the common nRF24L01+ and its more ubiquitous counterfeits.

The board itself is a custom 2-layer PCB designed to be operated without an enclosure. Example receiver code has been tested on an Arduino Uno using a matching nRF24L01+ module.

# BOM

| Reference | Name | Description |
| :-:       | :--  | :-- |
| RZ1 | [RP2040-Zero](https://www.aliexpress.us/item/3256804095235134.html) | Inexpensive RP2040-based microcontroller board |
| J1 | [nRF24L01+ module](https://www.amazon.com/gp/product/B082VLTTTX) | RF module (affixed with [2x4 0.1" connector](https://www.amazon.com/dp/B01IHBCO2K)) |
| SW1-4, SW6-8 | [TS02-66-50-BK-100-SCR-D](https://www.digikey.com/en/products/detail/cui-devices/TS02-66-50-BK-100-SCR-D/15634357) | Pushbuttons |
| SW10 | [DS04-254-1L-05BK](https://www.digikey.com/en/products/detail/cui-devices/DS04-254-1L-05BK/11310878) | 5-position DIP switch |
| SW5 | [PS4 controller joystick](https://www.amazon.com/dp/B09WDNSBBR) | Generic PS4 joystick (ALPS TKJXV1224-like) |
| SW9 | [0.1" SPDT switch](https://www.amazon.com/dp/B09R43HCY3) | Slide switch (for power) |
| BT1 | [2xAA battery holder](https://www.amazon.com/dp/B013GNC08C) | Battery holder (held on with double-sided tape) |
