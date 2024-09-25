#include "std_include.hpp"

#include "loader/component_loader.hpp"

#include <game/structs.hpp>
#include <game/game.hpp>

#include <utils/io.hpp>
#include <utils/string.hpp>
#include <utils/hook.hpp>

#include <json.hpp>

#include "DDL/JsonDDLRootWriter.h"

namespace Assets
{
	namespace //Ddl
	{
		const char* ddlTypeNames[] =
		{
			  "DDL_BYTE_TYPE",
			  "DDL_SHORT_TYPE",
			  "DDL_UINT_TYPE",
			  "DDL_INT_TYPE",
			  "DDL_INT64_TYPE",
			  "DDL_FLOAT_TYPE",
			  "DDL_FIXEDPOINT_TYPE",
			  "DDL_STRING_TYPE",
			  "DDL_STRUCT_TYPE",
			  "DDL_ENUM_TYPE",
			  "DDL_PAD_TYPE"
		};

		void dump_permission(const nlohmann::ordered_json& data)
		{

		}

		void dump_enum_member(const nlohmann::ordered_json& data)
		{

		}

		void dump_struct_member(const nlohmann::ordered_json& data)
		{

		}

		void dump_enum(const nlohmann::ordered_json& data)
		{

		}

		void dump_struct(const nlohmann::ordered_json& data)
		{

		}

		void dump_version(const nlohmann::ordered_json& data)
		{

		}

		void dump_flags(const nlohmann::ordered_json& data)
		{

		}

		void dump_official_format(const nlohmann::ordered_json& data)
		{
			std::string buffer;

			for (const nlohmann::ordered_json& def : data)
			{

			}

			//utils::io::write_file(std::format("{}/{}.json", "ddl_dump", data["name"]), buffer);
		}

		void dump(game::DDLRoot* asset)
		{
			nlohmann::ordered_json data;
			auto* ddlDef = asset->ddlDef;
			int i;

			data["name"] = asset->name;
			data["defs"] = nlohmann::json::array();

			for (i = 0; ; ddlDef = ddlDef->next, i++)
			{
				auto& defData = data["defs"][i];
				defData["version"] = ddlDef->version;
				defData["checksum"] = ddlDef->checksum;
				defData["flags"] = ddlDef->flags;
				defData["bitSize"] = ddlDef->bitSize;
				defData["byteSize"] = ddlDef->byteSize;
				defData["structCount"] = ddlDef->structCount;
				defData["enumCount"] = ddlDef->enumCount;
				defData["headerBitSize"] = ddlDef->headerBitSize;
				defData["headerByteSize"] = ddlDef->headerByteSize;
				defData["reserveSize"] = ddlDef->reserveSize;
				defData["userFlagsSize"] = ddlDef->userFlagsSize;
				defData["paddingUsed"] = ddlDef->paddingUsed;

				defData["structList"] = nlohmann::json::array();

				for (auto j = 0u; j < ddlDef->structCount; j++)
				{
					auto* structList = &ddlDef->structList[j];
					auto& ddlStruct = defData["structs"][j];

					ddlStruct["name"] = structList->name;
					ddlStruct["index"] = j;
					ddlStruct["bitSize"] = structList->bitSize;
					ddlStruct["memberCount"] = structList->memberCount;
					ddlStruct["members"] = nlohmann::json::array();

					for (auto k = 0u; k < structList->memberCount; k++)
					{
						auto* memberDef = &structList->members[k];
						auto& memberJson = ddlStruct["memberDefs"][k];

						memberJson["name"] = memberDef->name;
						memberJson["index"] = memberDef->index;
						memberJson["bitSize"] = memberDef->bitSize;
						memberJson["limitSize"] = memberDef->limitSize;
						memberJson["offset"] = memberDef->offset;
						if (memberDef->type >= game::DDL_BYTE_TYPE && memberDef->type < game::DDL_PAD_TYPE)
						{
							memberJson["type"] = ddlTypeNames[memberDef->type];
						}
						else
						{
							memberJson["type"] = memberDef->type;
						}
						memberJson["externalIndex"] = memberDef->externalIndex;
						memberJson["rangeLimit"] = memberDef->rangeLimit;
						memberJson["serverDelta"] = memberDef->serverDelta;
						memberJson["clientDelta"] = memberDef->clientDelta;
						memberJson["arraySize"] = memberDef->arraySize;
						memberJson["enumIndex"] = memberDef->enumIndex;
						memberJson["permission"] = memberDef->permission;
						//memberJson["hash"] = structList->hashTable[k].hash;
						//memberJson["index"] = structList->hashTable[k].index;
					}


				}

				defData["enumList"] = nlohmann::json::array();

				for (auto j = 0u; j < ddlDef->enumCount; j++)
				{
					auto* enumList = &ddlDef->enumList[j];
					auto& ddlEnum = defData["enums"][j];

					ddlEnum["name"] = enumList->name;
					ddlEnum["memberCount"] = enumList->memberCount;
					ddlEnum["members"] = nlohmann::json::array();

					for (auto k = 0u; k < enumList->memberCount; k++)
					{
						ddlEnum["members"][k] = enumList->members[k];
					}
				}

				if (!ddlDef->next)
				{
					break;
				}
			}

			std::string buffer;

			dump_official_format(data);

			buffer = data.dump(4);

			utils::io::write_file(std::format("{}/{}.json", "ddl_dump", asset->name), buffer);
		}
	}
}

namespace assets
{
	utils::hook::detour db_add_x_asset_hook;

	game::XAssetHeader db_add_x_asset_stub(game::XAssetType type, game::XAssetHeader header)
	{
		switch (type)
		{
		case game::ASSET_TYPE_DDL:
			//Assets::Ddl::dump(header.ddlRoot);
			Assets::Ddl::DumpDDLRoot(header.ddlRoot);
			break;
		default:
			break;
		}

		return db_add_x_asset_hook.invoke<game::XAssetHeader>(type, header);
	}

	class component final : public generic_component
	{
	public:
		void post_unpack() override
		{
			db_add_x_asset_hook.create(0x14141F4C0_g, db_add_x_asset_stub);
		}
	};
}

REGISTER_COMPONENT(assets::component)
