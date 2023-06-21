#ifndef __MODULE_H__
#define __MODULE_H__

#include <sys/prx.h>
#include <string.h>
#include "Defines.h"

enum eSegmentType : uint8_t
{
    TEXT_SEGMENT = 0,
    DATA_SEGMENT,
    RODATA_SEGMENT,
    RODATA2_SEGMENT,
};

ALWAYS_INLINE sys_prx_id_t POBF_API GetModuleHandle(const char* moduleName)
{
    return (moduleName) ? sys_prx_get_module_id_by_name(moduleName, 0, nullptr) : sys_prx_get_my_module_id();
}

ALWAYS_INLINE sys_prx_module_info_t POBF_API GetModuleInfo(sys_prx_id_t handle)
{
    sys_prx_module_info_t info{};
    static sys_prx_segment_info_t segments[10]{};
    static char filename[SYS_PRX_MODULE_FILENAME_SIZE]{};

    memset(segments, 0, sizeof(segments));
    memset(filename, 0, sizeof(filename));

    info.size = sizeof(info);
    info.segments = segments;
    info.segments_num = sizeof(segments) / sizeof(sys_prx_segment_info_t);
    info.filename = filename;
    info.filename_size = sizeof(filename);

    sys_prx_get_module_info(handle, 0, &info);
    return info;
}

ALWAYS_INLINE sys_prx_segment_info_t POBF_API GetModuleSegmentInfo(const char* moduleName, eSegmentType segmentType)
{
    sys_prx_module_info_t info = GetModuleInfo(GetModuleHandle(moduleName));
    return info.segments[segmentType];
}


#endif // __MODULE_H__