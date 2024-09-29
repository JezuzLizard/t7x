#include "std_include.hpp"

#include "JsonDDLRootWriter.hpp"

#include "DDLConstantsT7.hpp"
#include "JsonDDL.hpp"
#include "AbstractTextDumper.hpp"

#include <iomanip>
#include <filesystem>
#include <fstream>
#include <set>

namespace Assets
{
	namespace Ddl
	{
#define DDLAssert(expression, ...) \
	DDLAssertInternal(expression, #expression, __VA_ARGS__)

		void CreateJsonDDLRoot(JsonDDLRoot& jDDLRoot, const game::DDLRoot& ddlRoot);
		void ResolveCustomTypes(JsonDDLRoot& jDDLRoot);
		void RecursivelyTraverseStruct(const JsonDDLDef& jDDLDef, const JsonDDLStructDef& jDDLStruct);
		void DDLAssertInternal(const bool expression,
			const std::string& expressionString,
			const bool fatalOnOriginal = true,
			const bool fatalOnRelink = true);

		class DDLDumper : AbstractTextDumper
		{
		public:
			DDLDumper(std::ostream& stream)
				: AbstractTextDumper(stream)
			{
			}
			void WriteLineComment(const std::string& comment) const
			{
				Indent();
				m_stream << "// " << comment << "\n";
			}

			void DumpEnum(const JsonDDLEnumDef& _enum)
			{
				Indent();

				m_stream << "enum ";

				m_stream << _enum.name << "\n";

				Indent();
				m_stream << "{\n";

				IncIndent();

				const auto entryCount = _enum.members.size();
				for (auto i = 0u; i < entryCount; i++)
				{
					Indent();
					m_stream << _enum.members[i];
					m_stream << "\n";
				}

				DecIndent();
				Indent();
				m_stream << "};\n"; // end enum
			}

			void DumpMemberArray(const JsonDDLMemberDef& member)
			{
				if (member.arraySize.value_or(1) > 1)
				{
					if (member.enum_.has_value())
						m_stream << "[" << member.enum_.value() << "]";
					else
						m_stream << "[" << member.arraySize.value() << "]";
				}
			}

			void DumpMember(const JsonDDLDef& jDDLDef, const JsonDDLStructDef& _struct, const JsonDDLMemberDef& member)
			{
				Indent();
				if (member.link.m_type_category == game::DDL_STRING_TYPE)
				{
					m_stream << "string(" << member.maxCharacters.value() << ") " << member.name;
					DumpMemberArray(member);
				}
				else if (member.link.m_type_category == game::DDL_STRUCT_TYPE)
				{
					m_stream << member.type << " " << member.name;
					DumpMemberArray(member);
				}
				else if (game::DDL::Member::IsStandardSize(static_cast<game::DDLType>(member.link.m_type_category), member.link.m_bit_size, member.arraySize.value_or(1)))
				{
					switch (member.link.m_type_category)
					{
					case game::DDL_BYTE_TYPE:
						m_stream << "byte ";
						break;
					case game::DDL_SHORT_TYPE:
						m_stream << "short ";
						break;
					case game::DDL_UINT_TYPE:
						m_stream << "uint ";
						break;
					case game::DDL_INT_TYPE:
						m_stream << "int ";
						break;
					case game::DDL_UINT64_TYPE:
						m_stream << "uint64 ";
						break;
					case game::DDL_FLOAT_TYPE:
						m_stream << "float ";
						break;
					default:
						assert(false);
					}

					m_stream << member.name;
					DumpMemberArray(member);
				}
				else
				{
					if (member.link.m_type_category == game::DDL_FIXEDPOINT_TYPE)
					{
						m_stream << "fixed<" << member.limits->fixedMagnitudeBits.value() << "," << member.limits->fixedPrecisionBits.value() << "> ";
						m_stream << member.name;
						DumpMemberArray(member);
					}
					else if (member.limits->range.has_value())
					{
						//m_stream << member.type << "(" << member.limits->range.value() << ",client,server) " << member.name;
						m_stream << member.type << "(" << member.limits->range.value() << ") " << member.name;
						DumpMemberArray(member);
					}
					else
					{
						m_stream << member.type << ":" << member.limits->bits.value() << " " << member.name;
						DumpMemberArray(member);
					}
				}

				m_stream << ";\n";
			}

			void DumpStruct(const JsonDDLDef& jDDLDef, const JsonDDLStructDef& _struct, const size_t structIndex)
			{
#ifdef DDL_DEBUG
				Indent();
				m_stream << "// BitOffset: " << _struct.size.value() << "\n";
#endif
				if (!_struct.refCount)
				{
					Indent();
					m_stream << "// Unused\n";
				}
				Indent();

				m_stream << "struct " << _struct.name << "\n";

				Indent();
				m_stream << "{\n";

				IncIndent();

				for (const auto& member : _struct.members)
				{
					DumpMember(jDDLDef, _struct, member);
				}

				DecIndent();
				Indent();
				m_stream << "};\n"; // end struct
			}

			void DumpRoot(const JsonDDLDef& jDDLDef, const JsonDDLStructDef& _struct, const size_t structIndex)
			{
				Indent();
				m_stream << "\n";

				std::string permissionScope = "unspecified";
				for (const auto& member : _struct.members)
				{
					if (permissionScope == "unspecified" && member.permission.value() != "both" && permissionScope != member.permission.value())
					{
						permissionScope = member.permission.value();
						std::string permissionScopeFriendly = permissionScope;
						permissionScopeFriendly[0] = std::toupper(permissionScope.front());
						Indent();
						m_stream << permissionScopeFriendly + ":\n";
					}

					DumpMember(jDDLDef, _struct, member);
					}
				}

			void DumpDef(const JsonDDLDef& jDDLDef)
			{
				m_stream << "// ====================\n";
				m_stream << "// Version " << jDDLDef.version << "\n";
				m_stream << "// ====================\n";

#ifdef DDL_DEBUG
				m_stream << "// Size (Bits): " << jDDLDef.size.value() << "\n";
#endif

				m_stream << "version " << jDDLDef.version << "\n{\n";
				IncIndent();

				auto insertEmptyLine = false;

				for (const auto& _enum : jDDLDef.enums)
				{
					if (insertEmptyLine)
						m_stream << "\n";
					else
						insertEmptyLine = true;

					DumpEnum(_enum);
				}

				for (auto i = 1; i < jDDLDef.structs.size(); i++)
				{
					if (insertEmptyLine)
						m_stream << "\n";
					else
						insertEmptyLine = true;

					DumpStruct(jDDLDef, jDDLDef.structs[i], i);
				}

				DumpRoot(jDDLDef, jDDLDef.structs[0], 0);

				DecIndent();
				m_stream << "}\n"; // end version
			}
			};

		void Dump(const game::DDLRoot& ddlRoot)
		{
			JsonDDLRoot jsonDDLRoot;
			CreateJsonDDLRoot(jsonDDLRoot, ddlRoot);
			ResolveCustomTypes(jsonDDLRoot);

			for (auto i = 0u; i < jsonDDLRoot.defs.size(); i++)
			{
				for (auto& struc : jsonDDLRoot.defs[i].structs)
				{
					if (struc.name == "root")
					{
						jsonDDLRoot.defs[i].inCalculation.resize(jsonDDLRoot.defs[i].structs.size(), false);
						RecursivelyTraverseStruct(jsonDDLRoot.defs[i], struc);
					}

					DDLAssert(struc.refCount, false);

					for (auto& member : struc.members)
					{
						DDLAssert(member.refCount, false);

						DDLAssert(member.link.m_type_category != game::DDL_STRUCT_TYPE
							|| member.link.m_bit_size / member.arraySize.value_or(1) == jsonDDLRoot.defs[i].structs[member.link.m_external_index].size);

						// Only required for actual arrays
						if (member.arraySize.value_or(1) == 1)
							member.arraySize.reset();

						if (struc.name != "root")
							member.permission.reset();
					}
				}

				for (const auto& enum_ : jsonDDLRoot.defs[i].enums)
				{
					DDLAssert(enum_.refCount, false);
				}
			}

			for (auto i = 0; i < jsonDDLRoot.defFiles.size(); i++)
			{
				std::filesystem::path baseFilename = jsonDDLRoot.defFiles[i];
				auto filename = baseFilename.stem().string();
				auto parentFolder = baseFilename.parent_path().string();
				auto filenameFinal = std::format("canon/{}/{}", parentFolder, filename);
				std::filesystem::path assetFilePath(jsonDDLRoot.defFiles[i]);
				auto canonicalFile = std::ofstream(assetFilePath, std::fstream::out | std::fstream::binary);
				std::filesystem::create_directories(assetFilePath);
				DDLDumper dumper(canonicalFile);
				dumper.DumpDef(jsonDDLRoot.defs[i]);
			}

#ifdef DDL_DEBUG
			for (const auto& def : jsonDDLRoot.defs)
			{
				for (const auto& struc : def.structs)
				{
					if (struc.refCount)
						DDLAssert(struc.size.value() <= def.size.value());
				}

				DDLAssert(def.size.value() == def.structs[0].size.value());
			}
#endif
		}

		void DDLAssertInternal(const bool expression,
			const std::string& expressionString,
			const bool fatalOnOriginal,
			const bool fatalOnRelink)
		{
#ifndef DDL_DEBUG
			return;
#endif

#ifdef DDL_DEBUG_RELINK
			assert(expression || !fatalOnRelink);
			#elifdef DDL_DEBUG_ORIGNAL
				if (!expression)
				{
					//if (!expressionString.contains("refCount"))
						//__debugbreak();
					std::cout << expressionString << "\n";
					if (fatalOnOriginal)
						assert(false);
				}
#endif
		}

		bool IsLowerCase(const std::string& string)
		{
			return std::all_of(string.begin(),
				string.end(),
				[](unsigned char c)
				{
					return std::islower(c);
				});
		}

		bool CheckName(const std::string& name)
		{
			if (name.empty())
				return false;

			if (!std::isalpha(name[0]))
				return false;

			for (auto i = 1u; i < name.length(); i++)
			{
				if (!std::isalnum(name[i]) && name.substr(i, 1) != "_" && !std::islower(name[i]))
					return false;
			}

			if (name == "root" || name == "version")
				return true;

			const auto& it = game::DDL_KEYWORDS.find(name);
			if (it != game::DDL_KEYWORDS.end())
				return false;

			return true;
		}

		void TraverseMember(const JsonDDLDef& jDDLDef, const JsonDDLMemberDef& jDDLMember)
		{
			DDLAssert(jDDLMember.permission.value() == jDDLDef.permissionScope);

			auto enumIndex = jDDLMember.link.m_enum_index;
			if (enumIndex >= 0 && enumIndex < jDDLDef.enums.size())
			{
				jDDLDef.enums[enumIndex].refCount++;
			}

			auto structIndex = jDDLMember.link.m_external_index;
			if (structIndex > 0)
			{
				if (jDDLDef.inCalculation.at(structIndex))
					DDLAssert(false);

				jDDLDef.inCalculation[structIndex] = true;
				// parentDef.m_member_stack.push_back(*this);
				RecursivelyTraverseStruct(jDDLDef, jDDLDef.structs[structIndex]);
				// parentDef.m_member_stack.pop_back();
			}

			jDDLDef.inCalculation.clear();
			jDDLDef.inCalculation.resize(jDDLDef.structs.size(), false);
		}

		void RecursivelyTraverseStruct(const JsonDDLDef& jDDLDef, const JsonDDLStructDef& jDDLStruct)
		{
			jDDLStruct.refCount++;
			if (jDDLStruct.refCount > 1)
				return;

			for (const auto& member : jDDLStruct.members)
			{
				member.refCount++;
				if (member.refCount > 1)
					continue;

				if (jDDLStruct.name == "root")
					jDDLDef.permissionScope = member.permission.value();

				TraverseMember(jDDLDef, member);
			}
		}

		void ResolveCustomTypes(JsonDDLRoot& jDDLRoot)
		{
			for (auto& def : jDDLRoot.defs)
			{
				for (auto& struc : def.structs)
				{
					auto& members = struc.members;

					for (auto& member : members)
					{
						DDLAssert(member.link.m_external_index < static_cast<int>(def.structs.size()));

						if (member.link.m_external_index > 0 && member.link.m_external_index < def.structs.size())
						{
							member.type = def.structs[member.link.m_external_index].name;
							member.link.m_parent_struct = member.type;
						}
						else
							member.link.m_parent_struct = "root";

						DDLAssert(member.link.m_enum_index == -1 || def.enums.size() == 0 || member.link.m_enum_index < def.enums.size() && member.arraySize.value_or(1) != 1);

						if (member.link.m_enum_index >= 0 && member.arraySize.value_or(1) > 1)
						{
							DDLAssert(member.link.m_enum_index < def.enums.size());

							member.enum_.emplace(def.enums[member.link.m_enum_index].name);

							DDLAssert(member.arraySize == def.enums[member.link.m_enum_index].members.size());
						}
						else
						{
							//-1 enumindex indicates no enum
							//0 enumindex is a reserved value, but is not the same as -1
							//DDLAssert(member.link.m_enum_index == -1 || def.enums.size() == 0
							//|| member.link.m_enum_index < def.enums.size() && member.arraySize.value_or(1) != 1);
							//the above triggering implies a special extra usage of enums
						}
					}

					struc.size.emplace(members.back().link.m_offset + members.back().link.m_bit_size);
				}
			}
		}

		void CreateJsonDDlMemberLimits(JsonDDLMemberLimits& jDDLMemberLimits, const game::DDLMember& ddlMemberDef)
		{
			// If this happens it means the assertion that serverDelta, and clientDelta are not assigned separately is false
			DDLAssert((ddlMemberDef.rangeLimit == ddlMemberDef.serverDelta) && (ddlMemberDef.rangeLimit == ddlMemberDef.clientDelta));

			auto memberUnitSize = (ddlMemberDef.bitSize / ddlMemberDef.arraySize);

			if (ddlMemberDef.type == game::DDL_FIXEDPOINT_TYPE)
			{
				DDLAssert(ddlMemberDef.rangeLimit > 0);
				DDLAssert((memberUnitSize - ddlMemberDef.rangeLimit) > 0);

				jDDLMemberLimits.fixedMagnitudeBits.emplace(memberUnitSize - ddlMemberDef.rangeLimit);
				jDDLMemberLimits.fixedPrecisionBits.emplace(ddlMemberDef.rangeLimit);
			}
			else if (ddlMemberDef.rangeLimit != 0)
			{
				DDLAssert(ddlMemberDef.type == game::DDL_UINT_TYPE || ddlMemberDef.type == game::DDL_INT_TYPE);

				// Int type has the signed bit so the size is calculated differently
				DDLAssert((ddlMemberDef.type == game::DDL_UINT_TYPE || memberUnitSize == (std::bit_width(ddlMemberDef.rangeLimit) + 1)));
				DDLAssert((ddlMemberDef.type == game::DDL_INT_TYPE || memberUnitSize == std::bit_width(ddlMemberDef.rangeLimit)));
				jDDLMemberLimits.range.emplace(ddlMemberDef.rangeLimit);
			}
			else
				jDDLMemberLimits.bits.emplace(memberUnitSize);
		}

		void CreateJsonDDlMemberDef(JsonDDLMemberDef& jDDLMemberDef, const game::DDLMember& ddlMemberDef)
		{
			DDLAssert(CheckName(ddlMemberDef.name));
			DDLAssert(ddlMemberDef.type >= game::DDL_BYTE_TYPE && ddlMemberDef.type < game::DDL_TYPE_COUNT);
			DDLAssert(ddlMemberDef.bitSize > 0);
			DDLAssert(ddlMemberDef.arraySize > 0);
			DDLAssert(ddlMemberDef.offset >= 0);
			DDLAssert(ddlMemberDef.externalIndex >= 0 && ddlMemberDef.externalIndex < game::DDL::MAX_STRUCTS);
			DDLAssert(ddlMemberDef.enumIndex >= -1 && ddlMemberDef.enumIndex < game::DDL::MAX_ENUMS);
			DDLAssert(ddlMemberDef.type != game::DDL_STRING_TYPE || (ddlMemberDef.bitSize % CHAR_BIT) == 0);

			JsonDDLMemberLimits jLimits;
			jDDLMemberDef.name = ddlMemberDef.name;
			auto typeEnum = static_cast<game::DDLType>(ddlMemberDef.type);
			jDDLMemberDef.link.m_type_category = typeEnum;
			jDDLMemberDef.type = game::DDL::Member::TypeToName(typeEnum);

			DDLAssert(jDDLMemberDef.type != "unknown");

			jDDLMemberDef.link.m_bit_size = ddlMemberDef.bitSize;

			jDDLMemberDef.link.m_enum_index = ddlMemberDef.enumIndex;
			jDDLMemberDef.arraySize.emplace(ddlMemberDef.arraySize);

			//.size field has different implications depending on the type
			//string type treat it as max bytes
			//struct is based on the size of the struct
			//enum is based on the type, and also modifies arraySize to the count of its members
			if (ddlMemberDef.type == game::DDL_STRING_TYPE)
				jDDLMemberDef.maxCharacters.emplace(ddlMemberDef.bitSize / CHAR_BIT);
			else if (ddlMemberDef.type != game::DDL_STRUCT_TYPE && !game::DDL::Member::IsStandardSize(typeEnum, ddlMemberDef.bitSize, ddlMemberDef.arraySize))
				CreateJsonDDlMemberLimits(jDDLMemberDef.limits.emplace(jLimits), ddlMemberDef);

			DDLAssert(ddlMemberDef.type != game::DDL_STRUCT_TYPE || ddlMemberDef.rangeLimit == 0);

			DDLAssert(ddlMemberDef.type != game::DDL_ENUM_TYPE);

			jDDLMemberDef.link.m_offset = ddlMemberDef.offset;

			jDDLMemberDef.link.m_external_index = ddlMemberDef.externalIndex;

			const auto permissionCategory = static_cast<game::ddlPermissionTypes_e>(ddlMemberDef.permission);

			DDLAssert(permissionCategory >= game::DDL_PERMISSIONS_CLIENTONLY && permissionCategory < game::DDL_PERMISSIONS_COUNT);

			jDDLMemberDef.permission.emplace(game::DDL::Member::PermissionTypeToName(permissionCategory));

			DDLAssert(jDDLMemberDef.permission.has_value() && jDDLMemberDef.permission.value() != "unknown");
			DDLAssert(ddlMemberDef.arraySize != 1 || (ddlMemberDef.bitSize % ddlMemberDef.arraySize) == 0);
		}

		static auto prevHash = 0;
		static std::vector<bool> uniqueIndexes;
		static std::set<std::string> uniqueMembers;

		void CheckHashTable(const game::DDLHashTable& hashTable)
		{
			/*
			prevHash = hashTable.list;
			uniqueIndexes[hashTable.index] = true;
			uniqueMembers.insert(ddlStructDef.members.name);

			DDLAssert(hashTable.hash);
			DDLAssert(hashTable.index >= 0 && hashTable.index < ddlStructDef.memberCount);

			DDLAssert(prevHash == 0 || hashTable.hash > prevHash, false); // This is actually cursed
			DDLAssert(!uniqueIndexes[hashTable.index]);
			DDLAssert(!uniqueMembers.contains(ddlStructDef.members.name)); // HOW?!
			*/
		}

		void CreateJsonDDlStructList(JsonDDLStructDef& jDDLStructDef, const game::DDLStruct& ddlStructDef)
		{
			DDLAssert(CheckName(ddlStructDef.name));
			DDLAssert(ddlStructDef.memberCount > 0 && ddlStructDef.memberCount < game::DDL::MAX_MEMBERS);
			DDLAssert(ddlStructDef.bitSize > 0);
			DDLAssert(ddlStructDef.members);
			DDLAssert(ddlStructDef.hashTableLower.list);
			DDLAssert(ddlStructDef.hashTableUpper.list);

			jDDLStructDef.name = ddlStructDef.name;
			jDDLStructDef.members.resize(ddlStructDef.memberCount);
			jDDLStructDef.size.emplace(ddlStructDef.bitSize);

			prevHash = 0;
			auto calculatedStructSize = 0;
			auto prevOffset = 0;
			uniqueIndexes.clear();
			uniqueMembers.clear();

			for (auto i = 0; i < ddlStructDef.memberCount; i++)
			{
				DDLAssert(ddlStructDef.members[i].offset <= ddlStructDef.bitSize);
				DDLAssert(ddlStructDef.members[i].offset == 0 || ddlStructDef.members[i].offset > prevOffset);
				DDLAssert(ddlStructDef.members[i].offset == calculatedStructSize);
				DDLAssert(ddlStructDef.members[i].bitSize <= ddlStructDef.bitSize);

				prevOffset = ddlStructDef.members[i].offset;
				calculatedStructSize += ddlStructDef.members[i].bitSize;

				jDDLStructDef.sortedLowerHashTable.push_back(ddlStructDef.hashTableLower);
				jDDLStructDef.sortedUpperHashTable.push_back(ddlStructDef.hashTableUpper);
				CreateJsonDDlMemberDef(jDDLStructDef.members[i], ddlStructDef.members[i]);
			}

			uniqueMembers.clear();
			uniqueIndexes.clear();

			DDLAssert(ddlStructDef.bitSize == calculatedStructSize);
		}

		void CreateJsonDDlEnumList(JsonDDLEnumDef& jDDLEnumDef, const game::DDLEnum& ddlEnumDef)
		{
			DDLAssert(CheckName(ddlEnumDef.name));
			DDLAssert(ddlEnumDef.memberCount > 1 && ddlEnumDef.memberCount < game::DDL::MAX_MEMBERS);
			DDLAssert(ddlEnumDef.members && ddlEnumDef.members[0]);
			DDLAssert(ddlEnumDef.hashTable.list);

			jDDLEnumDef.name = ddlEnumDef.name;
			jDDLEnumDef.members.resize(ddlEnumDef.memberCount);

			auto prevHash = 0;
			std::vector<bool> uniqueIndexes;
			uniqueIndexes.resize(ddlEnumDef.memberCount, false);

			std::set<std::string> uniqueMembers;

			for (auto i = 0; i < ddlEnumDef.memberCount; i++)
			{
				DDLAssert(ddlEnumDef.hashTable.list[i].hash);
				DDLAssert(ddlEnumDef.hashTable.list[i].index >= 0 && ddlEnumDef.hashTable.list[i].index < ddlEnumDef.memberCount);

				DDLAssert(prevHash == 0 || ddlEnumDef.hashTable.list[i].hash > prevHash, false);
				DDLAssert(!uniqueIndexes[ddlEnumDef.hashTable.list[i].index]);
				DDLAssert(!uniqueMembers.contains(ddlEnumDef.members[i]), false);

				prevHash = ddlEnumDef.hashTable.list[i].hash;
				uniqueIndexes[ddlEnumDef.hashTable.list[i].index] = true;
				uniqueMembers.insert(ddlEnumDef.members[i]);

				DDLAssert(CheckName(ddlEnumDef.members[i]));

				jDDLEnumDef.sortedHashTable.push_back(ddlEnumDef.hashTable);
				jDDLEnumDef.members[i] = ddlEnumDef.members[i];
			}
		}

		static int prevDefVersion = 0;
		void CreateJsonDDlDef(JsonDDLRoot& jDDLRoot,
			const game::DDLDef* ddlDef,
			const std::filesystem::path baseFilename)
		{
			JsonDDLDef jsonDDLDef;

			DDLAssert(ddlDef->version > 0);
			DDLAssert(prevDefVersion == 0 || prevDefVersion > ddlDef->version);
			DDLAssert(ddlDef->bitSize > 0);
			DDLAssert(ddlDef->structCount > 0 && ddlDef->structCount < game::DDL::MAX_STRUCTS);
			DDLAssert(ddlDef->enumCount >= 0 && ddlDef->enumCount < game::DDL::MAX_ENUMS);

			prevDefVersion = ddlDef->version;

			jsonDDLDef.version = ddlDef->version;
			jsonDDLDef.bitSize.emplace(ddlDef->bitSize);

			if (ddlDef->structCount > 0)
			{
				DDLAssert(ddlDef->structList);
				DDLAssert(!strncmp(ddlDef->structList[0].name, "root", sizeof("root")));

				jsonDDLDef.structs.resize(ddlDef->structCount);

				for (auto i = 0; i < ddlDef->structCount; i++)
				{
					CreateJsonDDlStructList(jsonDDLDef.structs[i], ddlDef->structList[i]);
				}
			}

			if (ddlDef->enumCount > 0)
			{
				DDLAssert(ddlDef->enumList);

				jsonDDLDef.enums.resize(ddlDef->enumCount);
				for (auto i = 0; i < ddlDef->enumCount; i++)
					CreateJsonDDlEnumList(jsonDDLDef.enums[i], ddlDef->enumList[i]);
			}

			auto filename = baseFilename.stem().string();
			auto extension = ".ddl";
			auto parentFolder = baseFilename.parent_path().string();
			auto filenameFinal = std::format("dump/{}/{}.version_{}{}", parentFolder, filename, jsonDDLDef.version, extension);
			jsonDDLDef.name = ddlDef->name;
			jDDLRoot.defFiles.push_back(filenameFinal);
			jDDLRoot.defs.push_back(jsonDDLDef);

			if (!ddlDef->next)
				return;

			CreateJsonDDlDef(jDDLRoot, ddlDef->next, baseFilename);
		}

		void CreateJsonDDLRoot(JsonDDLRoot& jDDLRoot, const game::DDLRoot& ddlRoot)
		{
			std::filesystem::path baseFilename = ddlRoot.name;
			prevDefVersion = 0;
			CreateJsonDDlDef(jDDLRoot, ddlRoot.ddlDef, baseFilename);
		}

		void DumpDDLRoot(const game::DDLRoot* ddlRoot)
		{
			Dump(*ddlRoot);
		}
	}
}
