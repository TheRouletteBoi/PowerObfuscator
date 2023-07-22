# PowerObfuscator
PowerObfuscator obfuscate your PS3 .PRX/.SPRX plugins

# Features
- Compile Time String Obfuscation
- Compile Time Hash Generation
- Compile Time Random Number Generator
- Real Time Function Encryption & Decryption
- Real Time Memory Address Encryption & Decryption
- Dump Obfuscation
- Segment Obfuscation [.text] [.data] [.rodata] 

# Installation
1. Download the latest release from [Releases](https://github.com/TheRouletteBoi/PowerObfuscator/releases)
2. Navigate to your SDK installation folder (default: `C:\usr\local\cell\target\ppu`)
3. Drop the *include* and *lib* folders
4. Open up Project Properties and Add `-lc` to your project "Additional Dependencies"
5. Add `-lPowerObfuscator` to your project "Additional Dependencies"
6. Add `$(SCE_PS3_ROOT)\target\ppu\lib\libpsutil.a` to your project "Additional Dependencies"

![dependencies](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/dependencies.png)

7. Now go to *Linker* change "Output Format" to `PRX File`

![output format](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/outputformat.png)

8. Change "Debug Info and Symbol Stripping" to `Strip Debug Information Only`

![symbol stripping](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/symbolstripping.png)

9. In Project Properties navigate to *C/C++* then *Language* and change "C++ Language Standard" to `Use C++11`

![cpp11](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/cpp11.png)

10. Now go to *General* and change "Target Extension" to `.prx`

![file extension](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/fileextension.png)

11. Add `#include <libpsutils.h>` into your project
12. Add `#include <PowerObfuscator.h>` into your project

![header includes](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/headerincludes.png)


# Tutorial
1. TODO main() screenshot
2. TODO your code example
3. TODO show how to use app. example, symbol stripping, sign prx if you want to covert .prx to .sprx


 
# Building Requirements
- Visual Studio 2013+
- Sony PS3 4.75+ SDK w/ Visual Studio Integration
- [Fixed std::string library](https://github.com/skiff/libpsutil/releases)
- [Qt 6.6.0 with MSVC compilers](https://www.qt.io/download-qt-installer)
- [Qt plugins for Visual Studio](https://www.youtube.com/watch?v=rH2Kq2BIGVs) You will need Visual Studio 2022 or higher to build GUI app because QT extensions don't work with Visual Studio 2013 

# Preview Results

![Preview App](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/Preview.PNG)

![Preview ASM](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/preview_asm_code.png)

![Preview Strings](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/preview_string_code.png)


# Credits
- andrivet
- canehaxs
- cr-lupin
- gopro2027
- John Valincius
- jordywastaken
- LeFF
