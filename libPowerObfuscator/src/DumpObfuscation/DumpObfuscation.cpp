#include "DumpObfuscation.h"

namespace pobf
{
    namespace DumpObfuscation
    {
        void LisR3()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38600000)        // li r3, 0
                    *(uint32_t*)i = 0x3C600000;         // lis r3, 0
            }
        }

        void LisR4()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38800000)        // li r4, 0
                    *(uint32_t*)i = 0x3C800000;         // lis r4, 0
            }
        }

        void LisR5()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38A00000)        // li r5, 0
                    *(uint32_t*)i = 0x3CA00000;         // lis r5, 0
            }
        }

        void LisR6()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38C00000)        // li r6, 0
                    *(uint32_t*)i = 0x3CC00000;         // lis r6, 0
            }
        }

        void LisR7()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38E00000)        // li r7, 0
                    *(uint32_t*)i = 0x3CE00000;         // lis r7, 0
            }
        }

        void XorR3()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38600000)        // li r3, 0
                    *(uint32_t*)i = 0x7CA32A78;         // xor r3, r5, r5
            }
        }

        void XorR4()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38800000)        // li r4, 0
                    *(uint32_t*)i = 0x7CA42A78;         // xor r4, r5, r5
            }
        }

        void XorR5()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38A00000)        // li r5, 0
                    *(uint32_t*)i = 0x7CA52A78;         // xor, r5, r5, r5
            }
        }

        void XorR6()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38C00000)        // li r6, 0
                    *(uint32_t*)i = 0x7CA62A78;         // xor r6, r5, r5
            }
        }

        void XorR7()
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (*(uint32_t*)i == 0x38E00000)        // li r7, 0
                    *(uint32_t*)i = 0x7CA72A78;         // xor, r7, r5, r5
            }
        }


        // Doing something like this would be better performance then doing the individuals above
        /*void Replace_NameHere(eNameHere type)
        {
            for (uint32_t i = START_EBOOT; i < END_EBOOT; i += 4)
            {
                if (type & nameHere)
                    LisR3(i);

                if (type & nameHere2)
                    LisR4(i);
            }
        }

        // add this of one of the ways to PowerObfuscatorSPRX
        void Replace_NameHere2()
        {
            switch (vxRANDOM(1, 4))
            {
            case 1: { ReplaceAllLoadImmediateR3WithLoadImmediateShiftedR3(); break; }
            case 2: { ReplaceAllLoadImmediateR4WithLoadImmediateShiftedR4(); break; }
            case 3: { ReplaceAllLoadImmediateR5WithLoadImmediateShiftedR5(); break; }
            case 4: { ReplaceAllLoadImmediateR6WithLoadImmediateShiftedR6() break; }
            default: { ReplaceAllLoadImmediateR3WithLoadImmediateShiftedR3(); }
            }
        }*/

    }

}