#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>
#include <utils/thread.hpp>

#include "DDL/JsonDDLRootWriter.hpp"

namespace test
{
	namespace
	{
		utils::hook::detour DDLParse_Load_hook;
		void* __fastcall DDLParse_Load_stub(const char* filename)
		{
			auto* const asset_ptr = DDLParse_Load_hook.invoke<void*>(filename);

			const game::DDLRoot* const ddlAsset = reinterpret_cast<game::DDLRoot*>(asset_ptr);

			Assets::Ddl::DumpDDLRoot(ddlAsset);

			return asset_ptr;
		}
	}

	struct component final : generic_component
	{
		void post_unpack() override
		{
			DDLParse_Load_hook.create(0x305A30_g, DDLParse_Load_stub);
		}
	};
}

REGISTER_COMPONENT(test::component)
