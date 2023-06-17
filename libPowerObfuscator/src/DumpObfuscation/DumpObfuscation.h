#ifndef __DUMPOBFUSCATION_H__
#define __DUMPOBFUSCATION_H__

#include <types.h>
#include "Defines.h"

namespace pobf
{

// TODO(Roulette): let user apply eboot values because they vary per game.
#define START_EBOOT		0x0010200
#define END_EBOOT		0x17FDA50

namespace DumpObfuscation
{
    /**
    * @brief Replaces all 'li r3, 0' instructions with 'lis r3, 0'
    * @note If anyone tries to dump the game memory they will need to replace all these instruction in the dump if compared to the original eboot.
    * All this does is slow down the attacker.
    */
    void POBF_API LisR3();

    void POBF_API LisR4();

    void POBF_API LisR5();

    void POBF_API LisR6();

    void POBF_API LisR7();

    void POBF_API XorR3();

    void POBF_API XorR4();

    void POBF_API XorR5();

    void POBF_API XorR6();

    void POBF_API XorR7();



}

}





#endif // __DUMPOBFUSCATION_H__