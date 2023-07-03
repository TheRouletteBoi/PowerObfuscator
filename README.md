# PowerObfuscator
PowerObfuscator obfuscate your PS3 .PRX/.SPRX plugins

# Features
- Compile Time String Obfuscation
- Compile Time Hash Generation
- Compile time Random Number Generator
- Real Time Function Encryption & Decryption
- Real Time Memory Address Encryption & Decryption
- Dump Obfuscation


# TODO Tutorial
- Add -lc to your project library dependencies because memcpy is used in vxENCRYPT
- EnstoneStringEncrypt requires you to overrride new and delete operators because it uses std::string. Best to use with libpsutils.h
- FindPatternsInParallel requires you to overrride new and delete operators because it uses std::vector. Best to use with libpsutils.h
- You will need visual studio 2022 or higher to build QT GUI app because QT extensions don't work with visual studio 2013 

 
# Prerequisites
- Visual Studio 2013+
- Sony PS3 4.75+ SDK w/ Visual Studio Integration
- [Fixed std::string library](https://github.com/skiff/libpsutil/releases)
- [Qt 6.6.0 with MSVC compilers](https://www.qt.io/download-qt-installer)
- [Qt plugins for Visual Studio](https://www.youtube.com/watch?v=rH2Kq2BIGVs)

![Preview](https://github.com/TheRouletteBoi/PowerObfuscator/blob/master/Assets/Screenshots/Preview.PNG)


# Credits
- andrivet
- cr-lupin
- Gopro2027
- John Valincius
- jordywastaken
- LeFF
