#include "PowerObfuscator.h"

namespace pobf
{
    pobfHeader pobf_header = {
        POBF_MAGIC,
        POBF_SIGNATURE,
        POBF_TEXT_SEGMENT_DUMMY_VALUES,
        POBF_DATA_SEGMENT_DUMMY_VALUES,
        POBF_RODATA_SEGMENT_DUMMY_VALUES,
        POBF_PLACEHOLDER_DUMMY_VALUES
    };
}