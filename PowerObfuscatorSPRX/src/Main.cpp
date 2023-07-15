#include <cellstatus.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include <sys/syscall.h>
#include <sys/process.h>
#include <libpsutil.h>
#include <PowerObfuscatorAPI.h>

SYS_MODULE_INFO( PowerObfuscatorSPRX, 0, 1, 1);
SYS_MODULE_START( PowerObfuscatorSPRXMain );
SYS_MODULE_STOP( PowerObfuscatorSPRXStop );


sys_ppu_thread_t gTestEncryptedThreadId = SYS_PPU_THREAD_ID_INVALID;



void ThisFuncShouldBeEncrypted001();
int ThisFuncShouldBeEncrypted002(int left, int right);
void ThisFuncShouldBeEncrypted003();

void ThisFuncShouldBeEncrypted001()
{
    printf("ThisFuncShouldBeEncrypted001\n");

    int value = ThisFuncShouldBeEncrypted002(800, 255);
    printf("value: %d\n", value);

    int value2 = ThisFuncShouldBeEncrypted002(450, 2896);
    printf("value2: %d\n", value2);
}

int ThisFuncShouldBeEncrypted002(int left, int right)
{
    int value = left * right;
    return value;
}

void ThisFuncShouldBeEncrypted003()
{
    ThisFuncShouldBeEncrypted001();
    ThisFuncShouldBeEncrypted001();
}

void MainThread(uint64_t arg)
{
    sys_timer_sleep(8000);

    printf("MainThread\n");

    ThisFuncShouldBeEncrypted001();
    int value = ThisFuncShouldBeEncrypted002(999, 999);
    printf("value in thread %d\n", value);



    sys_ppu_thread_exit(0);
}


void pobf_CompileTime_RandomInt_Example()
{
    printf("pobf_CompileTime_RandomInt_Example\n");

    // Bernoulli engine is not fully implemented
    //pobf::MetaRand::CMetaUInt_PrintRandoms<10, typename pobf::MetaRand::Bernoulli<pobf::MetaRand::linear_congruential_engine<uint_fast32_t,3>, 1, 10>>::type >::print();
     
     
    // Meta Random
    pobf::MetaRand::CMetaUInt_PrintRandoms<15, typename pobf::MetaRand::CMetaUInt_Random<pobf::MetaRand::substract_with_carry_engine<unsigned int, 10, 2, 1>>::type >::print();


    // Last template argument is the random seed. It is optional.
    typedef pobf::MetaRand::CMetaUInt_init<pobf::MetaRand::substract_with_carry_engine<unsigned int, 10, 2, 1>>::type X;

    // Prints the first random number
    typedef pobf::MetaRand::CMetaUInt_Next<X>::type X0;
    printf("CMetaUInt_Next<X>: 0x%X\n", X0::value);

    // Prints the second random number
    typedef pobf::MetaRand::CMetaUInt_Next<X0>::type X1;
    printf("CMetaUInt_Next<X0>: 0x%X\n", X1::value);

    unsigned int randomInt1 = pobf::MetaRand::RandomCompileTimeInteger1::value;
    unsigned int randomInt2 = pobf::MetaRand::RandomCompileTimeInteger2::value;
    unsigned int randomInt3 = pobf::MetaRand::RandomCompileTimeInteger3::value;
    unsigned int randomInt4 = pobf::MetaRand::RandomCompileTimeInteger4::value;
    unsigned int randomInt5 = pobf::MetaRand::RandomCompileTimeInteger5::value;
    
    printf("RandomCompileTimeInteger1 0x%X\n", randomInt1);
    printf("RandomCompileTimeInteger2 0x%X\n", randomInt2);
    printf("RandomCompileTimeInteger3 0x%X\n", randomInt3);
    printf("RandomCompileTimeInteger4 0x%X\n", randomInt4);
    printf("RandomCompileTimeInteger5 0x%X\n", randomInt5);
}

void pobf_vxRandom1_Example()
{
    printf("pobf_vxRandom1_Example\n");

    switch (vxRANDOM(1, 4))
    {
        case 1: { printf("exampleRandom1: Code path 1!\n"); break; }
        case 2: { printf("exampleRandom1: Code path 2!\n"); break; }
        case 3: { printf("exampleRandom1: Code path 3!\n"); break; }
        case 4: { printf("exampleRandom1: Code path 4!\n"); break; }
        default: { printf("Fucking poltergeist!\n"); }
    }
}

// A small random code generator example
void pobf_vxRandom2_Example()
{
    printf("pobf_vxRandom2_Example\n");

    volatile uint32_t RndVal = vxRANDOM(0, 100);
    if (vxRAND() % 2) { RndVal += vxRANDOM(0, 100); }
    else { RndVal -= vxRANDOM(0, 200); }
    printf("exampleRandom2: %d\n", RndVal);
}

// A small string hasing example
void pobf_vxHashing_Example()
{
    printf("pobf_vxHashing_Example\n");

    printf("exampleHashing: 0x%08X\n", vxHASH("hello world!"));
    printf("exampleHashing: 0x%08X\n", vxHASH("HELLO WORLD!"));
}

void pobf_vxStringEncrypt_Example()
{
    printf("pobf_vxStringEncrypt_Example\n");

    printf("exampleEncryption: %s\n", vxENCRYPT("Hello world!"));
}

void pobf_AllStringEncryptions_Example()
{
    printf("pobf_AllStringEncryptions_Example\n");

    printf("ENCRYPTSTRV1 %s\n", ENCRYPTSTRV1("this string will be encrypted using V1"));

    printf("ENCRYPTSTRV2 %s\n", ENCRYPTSTRV2("this string will be encrypted using V2"));

    printf("ENCRYPTSTRV3 %s\n", ENCRYPTSTRV3("this string will be encrypted using V3"));


    // compile time string to hash
    printf("vxHASH: 0x%08X\n", vxHASH("hello world!"));
}

void pobf_DumpObfuscation_Example()
{
    pobf::DumpObfuscation::SetEbootRange(0x0010200, 0x17FDA50);

    // Example using compile time random
    switch (vxRANDOM(0, 4))
    {
    case 0: { pobf::DumpObfuscation::ReplaceLisR3(); break; }
    case 1: { pobf::DumpObfuscation::ReplaceLisR4(); break; }
    case 2: { pobf::DumpObfuscation::ReplaceLisR5(); break; }
    case 3: { pobf::DumpObfuscation::ReplaceLisR6(); break; }
    case 4: { pobf::DumpObfuscation::ReplaceLisR7(); break; }
    default: { pobf::DumpObfuscation::ReplaceLisR3(); }
    }


    // Example using multiple types 
    pobf::DumpObfuscation::ReplaceByType(pobf::DumpObfuscation::LisR3 | pobf::DumpObfuscation::LisR4 | pobf::DumpObfuscation::LisR6 | pobf::DumpObfuscation::XorR7);
}

// IMPORTANT: functions in here must be inlined otherwise the get encrypted by PowerObfuscatorGUI
// TODO(Roulette): either keep all functions inlined in main() or modify PowerObfuscatorGUI to skip symbols with 'pobf_' to allow no inline functions
extern "C" int PowerObfuscatorSPRXMain(int argc, char* argv[])
{
    printf("PowerObfuscatorSPRXMain\n");

    uint32_t functionAddressStart = ((pobf::opd_s*)PowerObfuscatorSPRXMain)->func;
    printf("functionAddressStart 0x%X\n", functionAddressStart);

    uint32_t moduleBaseAddress = (uint32_t)&__start__Ztext[0];

    uint32_t textSegmentStart = moduleBaseAddress;
    uint32_t textSegmentStop = moduleBaseAddress + pobf_header.textSegmentStart + pobf_header.textSegmentSize;
    printf("textSegmentStart 0x%X\n", textSegmentStart);
    printf("textSegmentStop 0x%X\n", textSegmentStop);

    uint32_t functionAddressStop = pobf::EncryptV3::FindEndOfMain(functionAddressStart, textSegmentStop);
    printf("functionAddressStop 0x%X\n", functionAddressStop);

    for (uint32_t i = textSegmentStart; i < textSegmentStop; i++)
    {
        // skip the main function
        if (i >= functionAddressStart && i <= functionAddressStop)
            continue;

        // read 1 byte at a time
        uint8_t byte = *(uint8_t*)(i);

        uint8_t unencryptByte = byte ^ 0x69;
        printf("unencryptByte 0x%02X at 0x%X\n", unencryptByte, i);

        pobf::EncryptV3::_write_process_memory((void*)i, &unencryptByte, sizeof(uint8_t));
    }

    uint32_t mainThreadAddress = ((pobf::opd_s*)MainThread)->func;
    printf("mainThreadAddress 0x%X\n", mainThreadAddress);


    printf("creating encrypted thread\n");
    sys_ppu_thread_create(&gTestEncryptedThreadId, MainThread, 0, 3000, 8192, SYS_PPU_THREAD_CREATE_JOINABLE, "TestEncryptedThread");

    return 0;
}

/* // old main()
extern "C" int PowerObfuscatorSPRXMain(int argc, char* argv[])
{
    uint32_t dataStart = (uint32_t)&__start__Zdata[0];
    uint32_t dataStop = (uint32_t)&__stop__Zdata[0];

    printf("dataStart 0x%X\n", dataStart);
    printf("dataStop 0x%X\n", dataStop);


    inline_encryptFunctionStart((void*)PowerObfuscatorSPRXMain);

    int var = pobf::EncryptV1::EXPORTS_TOC[0];
    pobf::EncryptV1::Start(var);


    sys_ppu_thread_create(&gTestEncryptedThreadId, MainThread, 0, 3000, 8192, SYS_PPU_THREAD_CREATE_JOINABLE, "TestEncryptedThread");

    ThisFuncShouldBeEncrypted003();


    inline_encryptFunctionEnd();
    StartPattternThread();

    return 0;
}*/

extern "C" int PowerObfuscatorSPRXStop(int argc, char* argv[])
{

    return 0;
}