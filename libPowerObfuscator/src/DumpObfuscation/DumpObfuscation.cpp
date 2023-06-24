#include "DumpObfuscation.h"

namespace pobf
{
    namespace DumpObfuscation
    {
        uint32_t g_EbootStart = 0;
        uint32_t g_EbootEnd = 0;

        void SetEbootRange(uint32_t ebootStartAddress, uint32_t ebootEndAddress)
        {
            g_EbootStart = ebootStartAddress;
            g_EbootEnd = ebootEndAddress;
        }

        void ReplaceLiR3WithLisR3(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38600000)        // li r3, 0
                *(uint32_t*)i = 0x3C600000;         // lis r3, 0
        }

        void ReplaceLiR4WithLisR4(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38800000)        // li r4, 0
                *(uint32_t*)i = 0x3C800000;         // lis r4, 0
        }

        void ReplaceLiR5WithLisR5(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38A00000)        // li r5, 0
                *(uint32_t*)i = 0x3CA00000;         // lis r5, 0
        }

        void ReplaceLiR6WithLisR6(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38C00000)        // li r6, 0
                *(uint32_t*)i = 0x3CC00000;         // lis r6, 0
        }

        void ReplaceLiR7WithLisR7(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38E00000)        // li r7, 0
                *(uint32_t*)i = 0x3CE00000;         // lis r7, 0
        }

        void ReplaceLiR3WithXorR3(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38600000)        // li r3, 0
                *(uint32_t*)i = 0x7CA32A78;         // xor r3, r5, r5
        }

        void ReplaceLiR4WithXorR4(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38800000)        // li r4, 0
                *(uint32_t*)i = 0x7CA42A78;         // xor r4, r5, r5
        }

        void ReplaceLiR5WithXorR5(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38A00000)        // li r5, 0
                *(uint32_t*)i = 0x7CA52A78;         // xor, r5, r5, r5
        }

        void ReplaceLiR6WithXorR6(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38C00000)        // li r6, 0
                *(uint32_t*)i = 0x7CA62A78;         // xor r6, r5, r5
        }

        void ReplaceLiR7WithXorR7(uint32_t i)
        {
            if (*(uint32_t*)i == 0x38E00000)        // li r7, 0
                *(uint32_t*)i = 0x7CA72A78;         // xor, r7, r5, r5
        }

        void ReplaceLisR3()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR3WithLisR3(i);
        }

        void ReplaceLisR4()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR4WithLisR4(i);
        }

        void ReplaceLisR5()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR5WithLisR5(i);
        }

        void ReplaceLisR6()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR6WithLisR6(i);
        }

        void ReplaceLisR7()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR7WithLisR7(i);
        }

        void ReplaceXorR3()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR3WithXorR3(i);
        }

        void ReplaceXorR4()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR4WithXorR4(i);
        }

        void ReplaceXorR5()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR5WithXorR5(i);
        }

        void ReplaceXorR6()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR6WithXorR6(i);
        }

        void ReplaceXorR7()
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
                ReplaceLiR7WithXorR7(i);
        }

        void ReplaceByType(uint32_t type)
        {
            for (uint32_t i = g_EbootStart; i < g_EbootEnd; i += 4)
            {
                if (type & LisR3)
                    ReplaceLiR3WithLisR3(i);

                if (type & LisR4)
                    ReplaceLiR4WithLisR4(i);

                if (type & LisR5)
                    ReplaceLiR5WithLisR5(i);

                if (type & LisR6)
                    ReplaceLiR6WithLisR6(i);

                if (type & LisR7)
                    ReplaceLiR7WithLisR7(i);

                if (type & XorR3)
                    ReplaceLiR3WithXorR3(i);

                if (type & XorR4)
                    ReplaceLiR4WithXorR4(i);

                if (type & XorR5)
                    ReplaceLiR5WithXorR5(i);

                if (type & XorR6)
                    ReplaceLiR6WithXorR6(i);

                if (type & XorR7)
                    ReplaceLiR7WithXorR7(i);
            }
        }
    }

}