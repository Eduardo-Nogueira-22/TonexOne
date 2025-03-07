| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# _Tonex ONE_ 

## Goal 

The project aims to implement footswitches directly on the microcontroller and an LCD via I2C, in addition to providing a PCB for prototyping and the case in 3D printing. In the modifications of the original code there is the implementation of the 20x4 LCD display using I2C and 6 pedals were directly inserted, and some combined functions for triggering delay, compressor and modulation.

## Video of Operation on YouTube

Watch the video of the device in operation on YouTube:

[![Video of Operation](Files/device01.jpg)](https://youtu.be/s6k85lcOOtI)


## 🙏 Acknowledgements <a name="acknowledgements"></a>
- [Greg Smith's](https://github.com/Builty/TonexOneController) All system features. This project is completely based on Greg Smith's project.


## Build
The project is ready to be built using the ESP32-S3 SUPER-MINI. To modify the platform, simply adjust the pinout in the folder.

## ESP32-S3

![ESP32-S3 Super-mini](https://ae01.alicdn.com/kf/Se5c04b089cb748cfbdbfbe46a277bd3cj.jpg_640x640q90.jpg)

## Circuit Schematics

![Schematics](Files/Schematic.png)

## 3d Circuit
![3d Circuit](Files/3d_Circuit.png)

![3d Circuit](Files/3d_circuit2.png)

In the link below we have the gerber for making the circuit board.

Gerber: ![Gerber](Files/Gerber_TONEX.zip)

## Case 3d

![Case 3D](Files/Case_3d.png)

Below we have the files for printing on a 3D printer. It is important to worry about filling the walls so that there is good mechanical resistance.

![STL Corpo](Files/Corpo.stl)

![STL Tampa](Files/Tampa.stl)

## Device
![Device](Files/device01.jpg)

![Device](Files/device02.jpg)

![Device](Files/device03.jpg)

![Device](Files/device04.jpg)

![Device Video](Files/device_video.mp4)



