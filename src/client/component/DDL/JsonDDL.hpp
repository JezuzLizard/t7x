#pragma once

#include "JsonCommon.hpp"
#include <json.hpp>

class JsonDDLMemberLimits
{
public:
    // Represents the size of the member in bits.
    // Used by the bitfield syntax to explicitly define the size.
    std::optional<size_t> bits;
    // Represents the size of the member based on the magnitude of the value of .range.
    std::optional<size_t> range;
    // T5 specific implementation of rangelimit
    std::optional<int> min;
    std::optional<int> max;
    // Represents the size of the member split between precision bits, and magnitude bits.
    std::optional<size_t> fixedPrecisionBits;
    // Represents the size of the member split between precision bits, and magnitude bits.
    std::optional<size_t> fixedMagnitudeBits;
};

class CommonDDLMemberDefLinkData
{
public:
    int m_bit_size = 0;
    int m_limit_size = 0;
    int m_offset = 0;
    int m_type_category = -1;
    int m_external_index = 0;
    int m_enum_index = -1;
    std::string m_parent_struct = "";
};

NLOHMANN_DEFINE_TYPE_EXTENSION_ORDERED(JsonDDLMemberLimits, bits, range, min, max, fixedPrecisionBits, fixedMagnitudeBits);

class JsonDDLMemberDef
{
public:
    std::string name;
    int index;
    std::string type;
    std::optional<JsonDDLMemberLimits> limits;
    std::optional<int> arraySize;
    std::optional<std::string> permission;
    std::optional<std::string> enum_;
    std::optional<int> maxCharacters;
    CommonDDLMemberDefLinkData link;
    mutable size_t refCount = 0;
};

NLOHMANN_DEFINE_TYPE_EXTENSION_ORDERED(JsonDDLMemberDef, name, type, limits, arraySize, permission, enum_, maxCharacters);

struct DDLHashTable
{
    std::vector<game::DDLHash> list;
    int max;
};

class JsonDDLStructDef
{
public:
    std::string name;
    int structBitSize;
    int calculatedStructBitSize;
    std::vector<JsonDDLMemberDef> members;
	DDLHashTable sortedLowerHashTable;
	DDLHashTable sortedUpperHashTable;
    mutable size_t refCount = 0;
};

NLOHMANN_DEFINE_TYPE_EXTENSION_ORDERED(JsonDDLStructDef, name, members);

class JsonDDLEnumDef
{
public:
    std::string name;
    std::vector<std::string> members;
	DDLHashTable sortedHashTable;
    mutable size_t refCount = 0;
};

NLOHMANN_DEFINE_TYPE_EXTENSION_ORDERED(JsonDDLEnumDef, name, members);

class JsonDDLDef
{
public:
    std::string name;
    unsigned short version;
    unsigned int checksum;
    unsigned char flags;
    int headerBitSize;
    int headerByteSize;
    int reserveSize;
    int userFlagsSize;
    bool paddingUsed;
    int defBitSize;
    int calculatedDefBitSize;
    int defByteSize;
    int calculatedDefByteSize;
    std::vector<JsonDDLEnumDef> enums;
    std::vector<JsonDDLStructDef> structs;
    std::vector<std::string> includeFiles;
    mutable std::vector<bool> inCalculation;
    mutable std::string permissionScope;
};

NLOHMANN_DEFINE_TYPE_EXTENSION_ORDERED(JsonDDLDef, includeFiles, version, enums, structs);

class JsonDDLRoot
{
public:
    std::string filename;
    std::vector<std::string> defFiles;
    std::vector<JsonDDLDef> defs;
    std::unordered_map<std::string, std::vector<JsonDDLDef>> includeDefs;
};

NLOHMANN_DEFINE_TYPE_EXTENSION_ORDERED(JsonDDLRoot, defFiles);

