# PowerObfuscator
PowerObfuscator obfuscate your PS3 .PRX/.SPRX plugins

# TODO
- FindSectionsInPRX
- FindAPIExports
- antiDebuggerCheck 
- tamperProtectionCheck
- Dump Obfuscation (varies per game?) input EBOOT.ELF or add to lib 🤷‍♂️
- Compile time string obfuscation 
- Compile time random seed/number generator
- Real time function encryption/decryption


# TODO Tutorial
Add the following items into the tutorial incase user encounters an errors.
- Add -lc to your project library dependencies because memcpy is used in vxENCRYPT
- EnstoneStringEncrypt requires you to overrride new and delete operators because it uses std::string. Best to use with libpsutils.h
- FindPatternsInParallel requires you to overrride new and delete operators because it uses std::vector. Best to use with libpsutils.h
- you will need visual studio 2022 to build QT GUI app because extensions don't work with visual studio 2013 
- QT version used was 6.6.0

 
# Prerequisites
- Visual Studio 2013+
- Sony PS3 4.75+ SDK w/ Visual Studio Integration
- [Fixed std::string library](https://github.com/skiff/libpsutil/releases)
- [Qt with MSVC compilers](https://www.qt.io/download-qt-installer)
- [Qt plugins Visual Studio](https://www.youtube.com/watch?v=rH2Kq2BIGVs)

![Preview](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/Preview.PNG)


# Credits
- John Valincius
