# SolarCube ☀️

This project is a 6×6×6 LED cube driven by an ATmega328p microcontroller, featuring multiple 3D animations and a real-time “solar clock”.

![E8D2069D-72BB-4F0F-A0EF-DB09E961F216_1_201_a](https://github.com/user-attachments/assets/14f70566-0d61-4330-b7ca-67a9d4374871)

### Solar clock animation

The LED cube visualizes the sun’s position throughout the day as a percentage of illuminated LEDs.
At sunrise, the first LED gradually turns on; as the day progresses, more LEDs light up until all LEDs are illuminated at midday. From there, the process reverses, with LEDs gradually turning off until the last one fades out at sunset.

The cube uses a real-time clock module together with a precomputed database of sunrise and sunset times for every day of the year, allowing the animation to track daylight progression accurately to the minute.

### Visual animations

The cube also includes additional aesthetic animations. These can be selected via the push button or automatic cycling.


## Hardware

- ATmega328P microcontroller
- 216 LEDs
- clock module (DS1307) + battery (CR2032)
- Push button
- 6 shift registers (74HC595)
- 6 MOSFETs (BS170)
- USB to TTL UART converter (CP2102)
- USB-C female connector
- Cables, resistors and a capacitor

**Approximate build cost**: ~18€

![DSC01097](https://github.com/user-attachments/assets/79f33cd5-3679-4371-bf29-c89cbafb014e)



https://github.com/user-attachments/assets/f1f9c02c-3fa1-4046-ab14-ff17ebea558f

