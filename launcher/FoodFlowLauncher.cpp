#include <windows.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

constexpr const wchar_t* VARIABLES[] = {
    L"FOODFLOW_DB_HOST",
    L"FOODFLOW_DB_PORT",
    L"FOODFLOW_DB_NAME",
    L"FOODFLOW_DB_USER",
    L"FOODFLOW_DB_PASSWORD",
    L"FOODFLOW_DB_SSLMODE"
};


std::wstring utf8AWide(
    const std::string& texto
) {

    if (texto.empty()) {
        return L"";
    }


    const int longitud =
        MultiByteToWideChar(
            CP_UTF8,
            0,
            texto.c_str(),
            static_cast<int>(
                texto.size()
            ),
            nullptr,
            0
        );


    if (longitud <= 0) {
        return L"";
    }


    std::wstring resultado(
        static_cast<std::size_t>(
            longitud
        ),
        L'\0'
    );


    MultiByteToWideChar(
        CP_UTF8,
        0,
        texto.c_str(),
        static_cast<int>(
            texto.size()
        ),
        resultado.data(),
        longitud
    );


    return resultado;
}


std::string recortar(
    const std::string& valor
) {

    const auto inicio =
        std::find_if_not(
            valor.begin(),
            valor.end(),
            [](unsigned char caracter) {
                return std::isspace(
                    caracter
                ) != 0;
            }
        );


    const auto fin =
        std::find_if_not(
            valor.rbegin(),
            valor.rend(),
            [](unsigned char caracter) {
                return std::isspace(
                    caracter
                ) != 0;
            }
        ).base();


    if (inicio >= fin) {
        return "";
    }


    return std::string(
        inicio,
        fin
    );
}


std::filesystem::path obtenerRutaLauncher() {

    std::vector<wchar_t> buffer(
        32768
    );


    const DWORD longitud =
        GetModuleFileNameW(
            nullptr,
            buffer.data(),
            static_cast<DWORD>(
                buffer.size()
            )
        );


    if (
        longitud == 0 ||
        longitud >= buffer.size()
    ) {

        return {};
    }


    return std::filesystem::path(
        std::wstring(
            buffer.data(),
            longitud
        )
    );
}


void mostrarError(
    const std::wstring& mensaje
) {

    MessageBoxW(
        nullptr,
        mensaje.c_str(),
        L"FoodFlow",
        MB_OK |
        MB_ICONERROR |
        MB_SETFOREGROUND
    );
}


bool variableDefinida(
    const wchar_t* nombre
) {

    const DWORD longitud =
        GetEnvironmentVariableW(
            nombre,
            nullptr,
            0
        );


    return longitud > 1;
}


bool cargarConfiguracion(
    const std::filesystem::path& archivoEnv
) {

    // ========================================================
    // LIMPIAR CONFIGURACION HEREDADA
    // ========================================================

    for (
        const wchar_t* variable :
        VARIABLES
    ) {

        SetEnvironmentVariableW(
            variable,
            nullptr
        );
    }


    // ========================================================
    // ABRIR DATABASE.ENV
    // ========================================================

    std::ifstream archivo(
        archivoEnv,
        std::ios::binary
    );


    if (!archivo.is_open()) {

        mostrarError(
            L"No se encontro el archivo de configuracion:\n\n"
            + archivoEnv.wstring()
        );


        return false;
    }


    // ========================================================
    // LEER VARIABLES
    // ========================================================

    std::string linea;

    bool primeraLinea =
        true;


    while (
        std::getline(
            archivo,
            linea
        )
    ) {

        // Eliminar CR de Windows.

        if (
            !linea.empty() &&
            linea.back() == '\r'
        ) {

            linea.pop_back();
        }


        // Eliminar BOM UTF-8 si existiera.

        if (
            primeraLinea &&
            linea.size() >= 3 &&
            static_cast<unsigned char>(
                linea[0]
            ) == 0xEF &&
            static_cast<unsigned char>(
                linea[1]
            ) == 0xBB &&
            static_cast<unsigned char>(
                linea[2]
            ) == 0xBF
        ) {

            linea.erase(
                0,
                3
            );
        }


        primeraLinea =
            false;


        linea =
            recortar(
                linea
            );


        if (linea.empty()) {
            continue;
        }


        if (linea.front() == '#') {
            continue;
        }


        const std::size_t posicion =
            linea.find(
                '='
            );


        if (
            posicion
            == std::string::npos
        ) {

            continue;
        }


        const std::string clave =
            recortar(
                linea.substr(
                    0,
                    posicion
                )
            );


        const std::string valor =
            recortar(
                linea.substr(
                    posicion + 1
                )
            );


        if (clave.empty()) {
            continue;
        }


        const std::wstring claveWide =
            utf8AWide(
                clave
            );


        const std::wstring valorWide =
            utf8AWide(
                valor
            );


        SetEnvironmentVariableW(
            claveWide.c_str(),
            valorWide.c_str()
        );
    }


    // ========================================================
    // VALIDAR CAMPOS OBLIGATORIOS
    // ========================================================

    if (
        !variableDefinida(
            L"FOODFLOW_DB_HOST"
        ) ||
        !variableDefinida(
            L"FOODFLOW_DB_NAME"
        ) ||
        !variableDefinida(
            L"FOODFLOW_DB_USER"
        ) ||
        !variableDefinida(
            L"FOODFLOW_DB_PASSWORD"
        )
    ) {

        mostrarError(
            L"La configuracion de PostgreSQL esta incompleta.\n\n"
            L"Revise:\n"
            L"config\\database.env\n\n"
            L"Son obligatorias:\n"
            L"FOODFLOW_DB_HOST\n"
            L"FOODFLOW_DB_NAME\n"
            L"FOODFLOW_DB_USER\n"
            L"FOODFLOW_DB_PASSWORD"
        );


        return false;
    }


    return true;
}


bool ejecutarFoodFlow(
    const std::filesystem::path& raiz
) {

    const std::filesystem::path ejecutable =
        raiz /
        L"dist" /
        L"foodflow.exe";


    if (
        !std::filesystem::exists(
            ejecutable
        )
    ) {

        mostrarError(
            L"No se encontro la aplicacion compilada:\n\n"
            + ejecutable.wstring()
            + L"\n\nEjecute scripts\\setup.bat "
              L"si esta preparando el proyecto."
        );


        return false;
    }


    const std::filesystem::path libpq =
        raiz /
        L"dist" /
        L"libpq.dll";


    if (
        !std::filesystem::exists(
            libpq
        )
    ) {

        mostrarError(
            L"La distribucion de FoodFlow esta incompleta.\n\n"
            L"Falta:\n"
            L"dist\\libpq.dll"
        );


        return false;
    }


    // ========================================================
    // DIRECTORIO DE TRABAJO
    //
    // Esto conserva:
    // facturas/pedido_X.csv
    // relativo a la raiz del proyecto.
    // ========================================================

    SetCurrentDirectoryW(
        raiz.c_str()
    );


    std::wstring comando =
        L"\""
        + ejecutable.wstring()
        + L"\"";


    std::vector<wchar_t> lineaComando(
        comando.begin(),
        comando.end()
    );


    lineaComando.push_back(
        L'\0'
    );


    STARTUPINFOW startup{};

    startup.cb =
        sizeof(
            STARTUPINFOW
        );


    PROCESS_INFORMATION proceso{};


    // CREATE_NO_WINDOW evita mostrar una consola negra.
    // FoodFlow sigue creando normalmente su ventana Win32.

    const BOOL creado =
        CreateProcessW(
            ejecutable.c_str(),
            lineaComando.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_NO_WINDOW,
            nullptr,
            raiz.c_str(),
            &startup,
            &proceso
        );


    if (!creado) {

        const DWORD codigo =
            GetLastError();


        mostrarError(
            L"No fue posible iniciar FoodFlow.\n\n"
            L"Codigo de Windows: "
            + std::to_wstring(
                codigo
            )
        );


        return false;
    }


    // Esperamos para poder detectar un cierre anormal.

    WaitForSingleObject(
        proceso.hProcess,
        INFINITE
    );


    DWORD codigoSalida =
        0;


    GetExitCodeProcess(
        proceso.hProcess,
        &codigoSalida
    );


    CloseHandle(
        proceso.hThread
    );


    CloseHandle(
        proceso.hProcess
    );


    if (
        codigoSalida != 0
    ) {

        mostrarError(
            L"FoodFlow finalizo con un error.\n\n"
            L"Codigo de salida: "
            + std::to_wstring(
                codigoSalida
            )
        );


        return false;
    }


    return true;
}

}


int WINAPI wWinMain(
    HINSTANCE,
    HINSTANCE,
    PWSTR,
    int
) {

    const std::filesystem::path launcher =
        obtenerRutaLauncher();


    if (launcher.empty()) {

        mostrarError(
            L"No fue posible determinar la ubicacion "
            L"de FoodFlow."
        );


        return 1;
    }


    const std::filesystem::path raiz =
        launcher.parent_path();


    const std::filesystem::path configuracion =
        raiz /
        L"config" /
        L"database.env";


    if (
        !cargarConfiguracion(
            configuracion
        )
    ) {

        return 1;
    }


    if (
        !ejecutarFoodFlow(
            raiz
        )
    ) {

        return 1;
    }


    return 0;
}