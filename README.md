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

# Preview Results
![Preview ASM](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/preview_asm_code.png)

![Preview Strings](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/preview_string_code.png)

# How To Use
1. In order for your sprx to decrypt you need to add `pobf::Segment::DecryptSegments(main);`to your main(). It must be the first function 

![Main Example](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/code_example_main.png)

2. In your prx go to definition of pobf::Segment::DecryptSegments(main); and replace the encryption key with your own.

3. Now load your prx in app and enter your encryption key. Default encryption key is: `5DF8B37ADF473A006210F4C66B4C8ECA148192813560F950F1804EF92C10EFDE0EEE63D63010088D705BCA549FC38383EB2F5110BEE4F1D88CD7F1D4719EBDB6`

![App Example](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/example_app.png)

# Installing PowerObfuscator into your sprx source code
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

 
# Building Requirements
- Visual Studio 2013+
- Sony PS3 4.75+ SDK w/ Visual Studio Integration
- [Fixed std::string library](https://github.com/skiff/libpsutil/releases)
- [Qt 6.6.0 with MSVC compilers](https://www.qt.io/download-qt-installer)
- [Qt plugins for Visual Studio](https://www.youtube.com/watch?v=rH2Kq2BIGVs) You will need Visual Studio 2022 or higher to build GUI app because QT extensions don't work with Visual Studio 2013

## When building make sure to set PowerObfuscatorGUI as the startup project 
- Right click project PowerObfuscatorGUI
![right click](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/right_click_gui_project.png)
- Click on Set As Startup Project
![set as startup project](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/set_as_startup_project.png)


# Credits
- andrivet
- canehaxs
- cr-lupin
- gopro2027
- John Valincius
- jordywastaken
- LeFF
