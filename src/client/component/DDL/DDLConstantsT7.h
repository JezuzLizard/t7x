#pragma once

#include "game/assets.hpp"

#include <algorithm>
#include <string>
#include <cassert>

namespace game
{
    enum ddlTypeFlags_e : size_t
    {
        DDL_FLAG_SIGNED = 0x1,
        DDL_FLAG_LIMITS = 0x2,
        DDL_FLAG_BITFIELDS = 0x4,
    };

    enum ddlCategoryFlags_e : size_t
    {
        DDL_CATEGORY_FLAG_NONE = 0x0,
        DDL_CATEGORY_FLAG_ENUM = 0x1,
        DDL_CATEGORY_FLAG_STRUCT = 0x2,
        DDL_CATEGORY_FLAG_BOTH = 0x3,
    };

    class DDLTypeFeature
    {
    public:
        size_t size;
        size_t flags;
        uint64_t max;
        int64_t min;
    };

    class DDLGameFeatures
    {
    public:
        const bool m_simple_hash_table;
        const bool m_split_hash_table;
        const bool m_header;
        const bool m_padding;
        const bool m_uint_type;
        const bool m_fixed_point_type;
        const bool m_bool_type;
        const bool m_version_keyword_allowed_as_name;
    };

    // This isn't strictly necessary, but I'm doing this anyway just in case someone wanted to transpile to the official formats.
    // By maintaining the same standards the canonical DDL linking that the T7 mod tools linker has, avoids potential future issues if someone wanted to link a
    // dumped DDL dumped using the json code here.
    inline const std::unordered_set<std::string> DDL_KEYWORDS = {
        "struct",
        "root",
        "enum",
        /*"version",*/ // Canonically used in a member name.
        "bool",
        "byte",
        "short",
        "uint",
        "int",
        "uint64",
        "float",
        "fixed",
        "string",
        "codeversion"
        "checksum",
        "nopadding",
        "userflags",
        "ddlchecksum",
        "reserve",
        "server",
        "client",
        "both",
        "include",
        "",
    };

    inline const std::unordered_map<std::string, int> DDL_TYPE_NAMES = {
        {"byte",        DDL_BYTE_TYPE      },
        {"short",       DDL_SHORT_TYPE     },
        {"uint",        DDL_UINT_TYPE      },
        {"int",         DDL_INT_TYPE       },
        {"uint64",      DDL_UINT64_TYPE    },
        {"float",       DDL_FLOAT_TYPE     },
        {"fixed_float", DDL_FIXEDPOINT_TYPE},
        {"string",      DDL_STRING_TYPE    },
        {"struct",      DDL_STRUCT_TYPE    },
        {"enum",        DDL_ENUM_TYPE      },
        {"",            DDL_TYPE_COUNT     }
    };

    inline const std::unordered_map<std::string, int> DDL_PERM_NAMES = {
        {"unspecified", DDL_PERMISSIONS_UNSPECIFIED},
        {"client",      DDL_PERMISSIONS_CLIENTONLY },
        {"server",      DDL_PERMISSIONS_SERVERONLY },
        {"both",        DDL_PERMISSIONS_BOTH       },
        {"",            DDL_PERMISSIONS_COUNT      }
    };

    inline const DDLTypeFeature DDL_TYPE_FEATURES[6] = {
        {
         .size = sizeof(char) * CHAR_BIT,
         .flags = 0,
         .max = UCHAR_MAX,
         .min = 0,
         },
        {
         .size = sizeof(short) * CHAR_BIT,
         .flags = 0,
         .max = USHRT_MAX,
         .min = 0,
         },
        {
         .size = sizeof(size_t) * CHAR_BIT,
         .flags = DDL_FLAG_LIMITS | DDL_FLAG_BITFIELDS,
         .max = UINT_MAX,
         .min = 0,
         },
        {
         .size = sizeof(int) * CHAR_BIT,
         .flags = DDL_FLAG_SIGNED | DDL_FLAG_LIMITS | DDL_FLAG_BITFIELDS,
         .max = INT_MAX,
         .min = INT_MIN,
         },
        {
         .size = sizeof(uint64_t) * CHAR_BIT,
         .flags = 0,
         .max = UINT64_MAX,
         .min = 0,
         },
        {
         .size = sizeof(float) * CHAR_BIT,
         .flags = 0,
         .max = 0,
         .min = 0,
         }
    };

    /*
    inline const DDLGameFeatures DDL_GAME_FEATURES = {.m_simple_hash_table = true,
                                                      .m_split_hash_table = true,
                                                      .m_header = true,
                                                      .m_padding = true,
                                                      .m_uint_type = true,
                                                      .m_fixed_point_type = true,
                                                      .m_bool_type = true,
                                                      .m_version_keyword_allowed_as_name = true};
    */

    class DDL
    {
    public:
        // Canonical limits in T7 linker
        static constexpr auto MAX_STRUCTS = 48;
        static constexpr auto MAX_ENUMS = 32;
        static constexpr auto MAX_MEMBERS = 1023;

        class Member
        {
        public:
            DDL::Member(const std::string& name)
            {
            }

            static const bool IsStandardSize(const DDLType typeEnum, const int size, const int arraySize)
            {
                if (typeEnum > DDL_FLOAT_TYPE)
                    return false;

                return DDL_TYPE_FEATURES[typeEnum].size == size / arraySize;
            }

            static const std::string PermissionTypeToName(ddlPermissionTypes_e permission)
            {
                if (permission >= DDL_PERMISSIONS_COUNT)
                    return "unknown";

                for (auto& [k, v] : DDL_PERM_NAMES)
                {
                    if (v == permission)
                        return k;
                }

                return "unknown";
            }

            static const std::string TypeToName(DDLType typeEnum)
            {
                if (typeEnum > DDL_TYPE_NAMES.size())
                    return "unknown";

                for (const auto& [k, v] : DDL_TYPE_NAMES)
                {
                    if (v == typeEnum)
                        return k;
                }

                return "unknown";
            }
        };
    };
} // namespace game
