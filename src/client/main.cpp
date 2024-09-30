#include <std_include.hpp>

#include "loader/component_loader.hpp"
#include "loader/loader.hpp"

#include <utils/finally.hpp>
#include <utils/hook.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>
#include <utils/flags.hpp>

#include "game/game.hpp"

namespace
{
	volatile bool g_call_tls_callbacks = false;
	std::pair<void**, void*> g_original_import{};

	DECLSPEC_NORETURN void WINAPI exit_hook(const uint32_t code)
	{
		component_loader::pre_destroy();
		ExitProcess(code);
	}

	BOOL set_process_dpi_aware_stub()
	{
		component_loader::post_unpack();
		return SetProcessDPIAware();
	}

	void patch_imports()
	{
		const utils::nt::library game{};
		utils::hook::set(game.get_iat_entry("kernel32.dll", "ExitProcess"), exit_hook);
		//utils::hook::set(game.get_iat_entry("user32.dll", "SetProcessDPIAware"), set_process_dpi_aware_stub);
	}

	PIMAGE_TLS_CALLBACK* get_tls_callbacks()
	{
		const utils::nt::library game{};
		const auto& entry = game.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
		if (!entry.VirtualAddress || !entry.Size)
		{
			return nullptr;
		}

		const auto* tls_dir = reinterpret_cast<IMAGE_TLS_DIRECTORY*>(game.get_ptr() + entry.VirtualAddress);
		return reinterpret_cast<PIMAGE_TLS_CALLBACK*>(tls_dir->AddressOfCallBacks);
	}

	void run_tls_callbacks(const DWORD reason)
	{
		if (!g_call_tls_callbacks)
		{
			return;
		}

		auto* callback = get_tls_callbacks();
		while (callback && *callback)
		{
			(*callback)(GetModuleHandleA(nullptr), reason, nullptr);
			++callback;
		}
	}

	[[maybe_unused]] thread_local struct tls_runner
	{
		tls_runner()
		{
			run_tls_callbacks(DLL_THREAD_ATTACH);
		}

		~tls_runner()
		{
			run_tls_callbacks(DLL_THREAD_DETACH);
		}
	} tls_runner;

	FARPROC load_process(const std::string& procname)
	{
		const auto proc = loader::load_binary(procname);

		auto* const peb = reinterpret_cast<PPEB>(__readgsqword(0x60));
		peb->Reserved3[1] = proc.get_ptr();
		static_assert(offsetof(PEB, Reserved3[1]) == 0x10);

		return FARPROC(proc.get_ptr() + proc.get_relative_entry_point());
	}

	bool handle_process_runner()
	{
		const auto* const command = "-proc ";
		const char* parent_proc = strstr(GetCommandLineA(), command);

		if (!parent_proc)
		{
			return false;
		}

		const auto pid = DWORD(atoi(parent_proc + strlen(command)));
		const utils::nt::handle<> process_handle = OpenProcess(SYNCHRONIZE, FALSE, pid);
		if (process_handle)
		{
			WaitForSingleObject(process_handle, INFINITE);
		}

		return true;
	}
}

int main()
{
	if (handle_process_runner())
	{
		return 0;
	}

	FARPROC entry_point{};
	srand(uint32_t(time(nullptr)) ^ ~(GetTickCount() * GetCurrentProcessId()));


	{
		auto premature_shutdown = true;
		const auto _ = utils::finally([&premature_shutdown]
		{
			if (premature_shutdown)
			{
				component_loader::pre_destroy();
			}
		});

		try
		{
			const auto client_binary = "linker_modtools.exe"s;

			if (!component_loader::activate())
			{
				return 1;
			}

			entry_point = load_process(client_binary);
			if (!entry_point)
			{
				throw std::runtime_error("Unable to load binary into memory");
			}

			patch_imports();

			if (!component_loader::post_load())
			{
				return 1;
			}

			premature_shutdown = false;
		}
		catch (std::exception& e)
		{
			game::show_error(e.what());
			return 1;
		}
	}

	g_call_tls_callbacks = true;
	return static_cast<int>(entry_point());
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	return main();
}
