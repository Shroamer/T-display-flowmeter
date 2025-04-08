Flow Meter

Something i want to utilize with my reverse osmosis filtering system, but still leave functional for other tasks.

Programed in Arduino IDE for esp32 TTGO T_Display board https://github.com/Xinyuan-LilyGO/TTGO-T-Display

* Uses hall voltmeter turbine 3v, 8256 imp/liter, 150-1500ml/min
* Counts impulses from turbine via interrupt
* Displays values such as liters/min and liters count on embedded st7789 135x240 TFT display.
* Collects history data and plots it on display.
* Checks battery voltage (vBat) and displays battery gauge on display.
* Checks embedded BUTTON1 for short and long presses (i got issues (again))

![08.04.2025 update](https://github.com/Shroamer/T-display-flowmeter/blob/main/photo/IMG_20250408_234426_cr.jpg?raw=true)

ToDo:
* add power-off triggered by low battery voltage
* find out why i have precission issues with conversion to liters/minute. It seems i could get more precise values.
* find out why my short/long press routine doesn't work as expected
* add debug page
* now we have 2 pages, and it's time to make scroll thru them vith a button
* add another page into carousel. this time it's calculator "how long it'll take to fill XX liters with current speed". several presets instread of input, cause i don't have encoder implemented yet
* add store impulses to flash
* add poweroff

BigToDo:
* add encoder (find suitable library or put some junk code myself) and put carousel on it
* add alarms and route them out with gpio, so i can close a valve after X liters
* add standby (with interrupt by sensor)

Pages i think may be usefull to add:
* debug page with raw data such as impulses/minute. also some other temp debug data for development process
* volume fill calculator: you put a volume - it says how long it takes.
* time volume calculator: you put a time - it says how many water you will get.
* 5 odometers for filter lifetime. Each of these have configurable lifetime value, and you can reset it, so it begins countback from the full value. This way i can know when i need to replace every filter.
* alarm by volume passed
* alarm by speed out of range
* alaram by time(?)

Configurable values i want to add to config page:
* impulse/liter of connected turbine (already a variable)
* refresh rate (already a variable)
* log update rate (already a variable)
* units conversion (on all pages? deal with drastical value length change? i have to think of it)
* 
