#ifndef __DUMPOBFUSCATION_H__
#define __DUMPOBFUSCATION_H__

#include <types.h>
#include "Defines.h"

namespace pobf
{
    namespace DumpObfuscation
    {
        enum Type : uint32_t
        {
            LisR3 = 1 << 0,
            LisR4 = 1 << 1,
            LisR5 = 1 << 2,
            LisR6 = 1 << 3,
            LisR7 = 1 << 4,
            XorR3 = 1 << 5,
            XorR4 = 1 << 6,
            XorR5 = 1 << 7,
            XorR6 = 1 << 8,
            XorR7 = 1 << 9
        };

        /**
        * @brief set the eboot start and end address
        * @note This function must be called before any of the other DumpObfuscation functions
        */
        void POBF_API SetEbootRange(uint32_t ebootStartAddress, uint32_t ebootEndAddress);

        /**
        * @brief Replaces all 'li r3, 0' instructions with 'lis r3, 0'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceLisR3();

        /**
        * @brief Replaces all 'li r4, 0' instructions with 'lis r4, 0'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceLisR4();

        /**
        * @brief Replaces all 'li r5, 0' instructions with 'lis r5, 0'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceLisR5();

        /**
        * @brief Replaces all 'li r6, 0' instructions with 'lis r6, 0'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceLisR6();

        /**
        * @brief Replaces all 'li r7, 0' instructions with 'lis r7, 0'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceLisR7();

        /**
        * @brief Replaces all 'li r3, 0' instructions with 'xor r3, r5, r5'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceXorR3();

        /**
        * @brief Replaces all 'li r4, 0' instructions with 'xor r4, r5, r5'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceXorR4();

        /**
        * @brief Replaces all 'li r5, 0' instructions with 'xor, r5, r5, r5'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceXorR5();

        /**
        * @brief Replaces all 'li r6, 0' instructions with 'xor r6, r5, r5'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceXorR6();

        /**
        * @brief Replaces all 'li r7, 0' instructions with 'xor, r7, r5, r5'
        * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
        * All this does is slow down the attacker.
        */
        void POBF_API ReplaceXorR7();

        /**
        * @brief Replace instactions by type. Using enum DumpObfuscation::Type
        */
        void POBF_API ReplaceByType(uint32_t type);
    }
}





#endif // __DUMPOBFUSCATION_H__