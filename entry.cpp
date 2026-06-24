#include "main.hpp"
#include "valve/interfaces/vtables/i_mem_alloc.hpp"
#include "directx/directx.hpp"
#include "features/shared/item_schema.hpp"
#include "features/skin_changer/skin_changer.hpp"
#include "features/glove_changer/glove_changer.hpp"

void destroy(HMODULE h_module) {
    g_hooks->destroy();
    logger::shutdown();

    FreeLibraryAndExitThread(h_module, 0);
}

uintptr_t __stdcall start_address(const HMODULE h_module) {
    try {
        logger::initialize();

        g_modules->m_modules.initialize();
        g_interfaces->initialize();

        g_item_schema->initialize();
        g_skin_changer->initialize();

        g_directx->initialize();
        g_hooks->initialize();

        LOG_SUCCESS(xorstr_("nerv ready"));

        while (!GetAsyncKeyState(VK_END)) {
            Sleep(100);
        }

        destroy(h_module);
    }
    catch (const std::exception& e) {
        MessageBoxA(NULL, e.what(), "Exception", MB_OK | MB_ICONERROR);
    }
    catch (...) {
        MessageBoxA(NULL, "Unknown exception occurred", "Exception", MB_OK | MB_ICONERROR);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE h_module, DWORD ul_reason_for_call, LPVOID lp_reserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        InitMemAlloc();
        DisableThreadLibraryCalls(h_module);

        HANDLE thread = CreateThread(nullptr, 0,
                                     reinterpret_cast<LPTHREAD_START_ROUTINE>(start_address),
                                     h_module, 0, nullptr);

        if (thread != nullptr && thread != INVALID_HANDLE_VALUE) {
            CloseHandle(thread);
            return TRUE;
        }

        return FALSE;
    }

    return TRUE;
}
