#include "presentacion/gui/AplicacionGui.hpp"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(
    HWND hwnd,
    UINT mensaje,
    WPARAM wParam,
    LPARAM lParam);

namespace foodflow
{

    AplicacionGui::AplicacionGui(
        PedidoRepositorio &pedidoRepositorio,
        ClienteRepositorio &clienteRepositorio,
        IProductoRepositorio &productoRepositorio,
        PedidoServicio &pedidoServicio,
        FacturaCsvServicio &facturaServicio,
        PedidoEstadoServicio &pedidoEstadoServicio,
        std::int64_t restauranteId)
        : instancia_(
              GetModuleHandleW(nullptr)),
          ventana_(nullptr),
          dispositivo_(nullptr),
          contexto_(nullptr),
          swapChain_(nullptr),
          renderTarget_(nullptr),
          vistaActiva_(
              VistaActiva::DASHBOARD),
          vistaDashboard_(
              pedidoRepositorio),
          vistaPedidos_(
              pedidoRepositorio),
          vistaNuevoPedido_(
              clienteRepositorio,
              productoRepositorio,
              pedidoServicio,
              facturaServicio,
              restauranteId),
          vistaDetallePedido_(
              pedidoRepositorio,
              pedidoEstadoServicio,
              facturaServicio)
    {
    }

    bool AplicacionGui::crearVentana()
    {

        ImGui_ImplWin32_EnableDpiAwareness();

        WNDCLASSEXW clase{};

        clase.cbSize =
            sizeof(
                WNDCLASSEXW);

        clase.style =
            CS_CLASSDC;

        clase.lpfnWndProc =
            AplicacionGui::wndProc;

        clase.hInstance =
            instancia_;

        clase.hCursor =
            LoadCursor(
                nullptr,
                IDC_ARROW);

        clase.lpszClassName =
            L"FoodFlowWindow";

        if (
            !RegisterClassExW(
                &clase))
        {

            return false;
        }

        ventana_ =
            CreateWindowW(
                clase.lpszClassName,
                L"FoodFlow - Gestion y trazabilidad de pedidos",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                1450,
                900,
                nullptr,
                nullptr,
                instancia_,
                this);

        return ventana_ != nullptr;
    }

    bool AplicacionGui::crearDirectX()
    {

        DXGI_SWAP_CHAIN_DESC descripcion{};

        descripcion.BufferCount =
            2;

        descripcion.BufferDesc.Width =
            0;

        descripcion.BufferDesc.Height =
            0;

        descripcion.BufferDesc.Format =
            DXGI_FORMAT_R8G8B8A8_UNORM;

        descripcion.BufferDesc.RefreshRate.Numerator =
            60;

        descripcion.BufferDesc.RefreshRate.Denominator =
            1;

        descripcion.Flags =
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        descripcion.BufferUsage =
            DXGI_USAGE_RENDER_TARGET_OUTPUT;

        descripcion.OutputWindow =
            ventana_;

        descripcion.SampleDesc.Count =
            1;

        descripcion.SampleDesc.Quality =
            0;

        descripcion.Windowed =
            TRUE;

        descripcion.SwapEffect =
            DXGI_SWAP_EFFECT_DISCARD;

        constexpr D3D_FEATURE_LEVEL niveles[] = {

            D3D_FEATURE_LEVEL_11_0,

            D3D_FEATURE_LEVEL_10_0};

        D3D_FEATURE_LEVEL nivelObtenido{};

        HRESULT resultado =
            D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                0,
                niveles,
                2,
                D3D11_SDK_VERSION,
                &descripcion,
                &swapChain_,
                &dispositivo_,
                &nivelObtenido,
                &contexto_);

        if (
            resultado == DXGI_ERROR_UNSUPPORTED)
        {

            resultado =
                D3D11CreateDeviceAndSwapChain(
                    nullptr,
                    D3D_DRIVER_TYPE_WARP,
                    nullptr,
                    0,
                    niveles,
                    2,
                    D3D11_SDK_VERSION,
                    &descripcion,
                    &swapChain_,
                    &dispositivo_,
                    &nivelObtenido,
                    &contexto_);
        }

        if (
            FAILED(
                resultado))
        {

            return false;
        }

        crearRenderTarget();

        return true;
    }

    void AplicacionGui::crearRenderTarget()
    {

        ID3D11Texture2D *backBuffer =
            nullptr;

        if (
            SUCCEEDED(
                swapChain_->GetBuffer(
                    0,
                    IID_PPV_ARGS(
                        &backBuffer))))
        {

            dispositivo_->CreateRenderTargetView(
                backBuffer,
                nullptr,
                &renderTarget_);

            backBuffer->Release();
        }
    }

    void AplicacionGui::liberarRenderTarget()
    {

        if (
            renderTarget_)
        {

            renderTarget_->Release();

            renderTarget_ =
                nullptr;
        }
    }

    void AplicacionGui::liberarDirectX()
    {

        liberarRenderTarget();

        if (
            swapChain_)
        {

            swapChain_->Release();

            swapChain_ =
                nullptr;
        }

        if (
            contexto_)
        {

            contexto_->Release();

            contexto_ =
                nullptr;
        }

        if (
            dispositivo_)
        {

            dispositivo_->Release();

            dispositivo_ =
                nullptr;
        }
    }

    void AplicacionGui::destruirVentana()
    {

        if (
            ventana_)
        {

            DestroyWindow(
                ventana_);

            ventana_ =
                nullptr;
        }

        UnregisterClassW(
            L"FoodFlowWindow",
            instancia_);
    }

    void AplicacionGui::configurarEstilo()
    {

        ImGuiStyle &estilo =
            ImGui::GetStyle();

        // ========================================================
        // DIMENSIONES
        // ========================================================

        estilo.WindowPadding =
            ImVec2(
                20.0f,
                20.0f);

        estilo.FramePadding =
            ImVec2(
                13.0f,
                9.0f);

        estilo.CellPadding =
            ImVec2(
                10.0f,
                8.0f);

        estilo.ItemSpacing =
            ImVec2(
                11.0f,
                11.0f);

        estilo.ItemInnerSpacing =
            ImVec2(
                8.0f,
                7.0f);

        estilo.IndentSpacing =
            22.0f;

        estilo.ScrollbarSize =
            18.0f;

        estilo.GrabMinSize =
            12.0f;

        // ========================================================
        // REDONDEO
        // ========================================================

        estilo.WindowRounding =
            7.0f;

        estilo.ChildRounding =
            7.0f;

        estilo.FrameRounding =
            5.0f;

        estilo.PopupRounding =
            6.0f;

        estilo.ScrollbarRounding =
            6.0f;

        estilo.GrabRounding =
            5.0f;

        estilo.TabRounding =
            5.0f;

        // ========================================================
        // BORDES
        // ========================================================

        estilo.WindowBorderSize =
            0.0f;

        estilo.ChildBorderSize =
            1.0f;

        estilo.FrameBorderSize =
            0.0f;

        // ========================================================
        // PALETA FOODFLOW
        // ========================================================

        ImVec4 *colores =
            estilo.Colors;

        // Fondo general

        colores[ImGuiCol_WindowBg] =
            ImVec4(
                0.055f,
                0.060f,
                0.070f,
                1.00f);

        // Paneles

        colores[ImGuiCol_ChildBg] =
            ImVec4(
                0.075f,
                0.080f,
                0.095f,
                1.00f);

        colores[ImGuiCol_PopupBg] =
            ImVec4(
                0.080f,
                0.085f,
                0.100f,
                1.00f);

        // Bordes

        colores[ImGuiCol_Border] =
            ImVec4(
                0.18f,
                0.20f,
                0.24f,
                1.00f);

        // Campos

        colores[ImGuiCol_FrameBg] =
            ImVec4(
                0.115f,
                0.125f,
                0.145f,
                1.00f);

        colores[ImGuiCol_FrameBgHovered] =
            ImVec4(
                0.150f,
                0.165f,
                0.190f,
                1.00f);

        colores[ImGuiCol_FrameBgActive] =
            ImVec4(
                0.175f,
                0.195f,
                0.225f,
                1.00f);

        // Botones normales

        colores[ImGuiCol_Button] =
            ImVec4(
                0.115f,
                0.130f,
                0.155f,
                1.00f);

        colores[ImGuiCol_ButtonHovered] =
            ImVec4(
                0.165f,
                0.205f,
                0.245f,
                1.00f);

        colores[ImGuiCol_ButtonActive] =
            ImVec4(
                0.180f,
                0.235f,
                0.280f,
                1.00f);

        // Encabezados y selección

        colores[ImGuiCol_Header] =
            ImVec4(
                0.115f,
                0.160f,
                0.195f,
                1.00f);

        colores[ImGuiCol_HeaderHovered] =
            ImVec4(
                0.145f,
                0.205f,
                0.250f,
                1.00f);

        colores[ImGuiCol_HeaderActive] =
            ImVec4(
                0.170f,
                0.235f,
                0.285f,
                1.00f);

        // Tablas

        colores[ImGuiCol_TableHeaderBg] =
            ImVec4(
                0.100f,
                0.115f,
                0.135f,
                1.00f);

        colores[ImGuiCol_TableRowBg] =
            ImVec4(
                0.070f,
                0.075f,
                0.088f,
                1.00f);

        colores[ImGuiCol_TableRowBgAlt] =
            ImVec4(
                0.090f,
                0.095f,
                0.110f,
                1.00f);

        colores[ImGuiCol_TableBorderStrong] =
            ImVec4(
                0.20f,
                0.22f,
                0.25f,
                1.00f);

        colores[ImGuiCol_TableBorderLight] =
            ImVec4(
                0.14f,
                0.15f,
                0.18f,
                1.00f);

        // Separadores

        colores[ImGuiCol_Separator] =
            ImVec4(
                0.20f,
                0.22f,
                0.25f,
                1.00f);

        // Texto

        colores[ImGuiCol_Text] =
            ImVec4(
                0.92f,
                0.94f,
                0.96f,
                1.00f);

        colores[ImGuiCol_TextDisabled] =
            ImVec4(
                0.55f,
                0.59f,
                0.64f,
                1.00f);
    }

    void AplicacionGui::renderizarMenuLateral()
    {

        ImGui::BeginChild(
            "menu_lateral",
            ImVec2(
                230.0f,
                0.0f),
            true);

        // ========================================================
        // MARCA
        // ========================================================

        ImGui::SetWindowFontScale(
            1.30f);

        ImGui::Text(
            "FOODFLOW");

        ImGui::SetWindowFontScale(
            1.0f);

        ImGui::TextDisabled(
            "Gestion y trazabilidad");

        ImGui::TextDisabled(
            "de pedidos");

        ImGui::Spacing();

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        // ========================================================
        // FUNCION AUXILIAR PARA NAVEGACION
        // ========================================================

        auto botonNavegacion =
            [this](
                const char *texto,
                VistaActiva vista) -> bool
        {
            const bool activa =
                vistaActiva_ == vista;

            if (
                activa)
            {

                ImGui::PushStyleColor(
                    ImGuiCol_Button,
                    ImVec4(
                        0.10f,
                        0.32f,
                        0.42f,
                        1.00f));

                ImGui::PushStyleColor(
                    ImGuiCol_ButtonHovered,
                    ImVec4(
                        0.12f,
                        0.37f,
                        0.48f,
                        1.00f));

                ImGui::PushStyleColor(
                    ImGuiCol_ButtonActive,
                    ImVec4(
                        0.10f,
                        0.32f,
                        0.42f,
                        1.00f));
            }

            const bool presionado =
                ImGui::Button(
                    texto,
                    ImVec2(
                        -1.0f,
                        48.0f));

            if (
                activa)
            {

                ImGui::PopStyleColor(
                    3);
            }

            return presionado;
        };

        // ========================================================
        // DASHBOARD
        // ========================================================

        if (
            botonNavegacion(
                "Dashboard",
                VistaActiva::DASHBOARD))
        {

            vistaDashboard_.recargar();

            vistaActiva_ =
                VistaActiva::DASHBOARD;
        }

        // ========================================================
        // PEDIDOS
        // ========================================================

        if (
            botonNavegacion(
                "Pedidos",
                VistaActiva::PEDIDOS))
        {

            vistaPedidos_.recargar();

            vistaActiva_ =
                VistaActiva::PEDIDOS;
        }

        // ========================================================
        // NUEVO PEDIDO
        // ========================================================

        if (
            botonNavegacion(
                "Nuevo pedido",
                VistaActiva::NUEVO_PEDIDO))
        {

            vistaActiva_ =
                VistaActiva::NUEVO_PEDIDO;
        }

        // ========================================================
        // ESTADO DE CONEXION
        // ========================================================

        const float posicionEstado =
            ImGui::GetWindowHeight() - 105.0f;

        if (
            posicionEstado > ImGui::GetCursorPosY())
        {

            ImGui::SetCursorPosY(
                posicionEstado);
        }

        ImGui::Separator();

        ImGui::Spacing();

        ImGui::TextDisabled(
            "PERSISTENCIA");

        ImGui::TextColored(
            ImVec4(
                0.35f,
                0.85f,
                0.50f,
                1.00f),
            "PostgreSQL conectado");

        ImGui::TextDisabled(
            "AWS RDS");

        ImGui::EndChild();
    }

    void AplicacionGui::renderizarInterfaz()
    {

        const ImGuiViewport *viewport =
            ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(
            viewport->WorkPos);

        ImGui::SetNextWindowSize(
            viewport->WorkSize);

        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin(
            "FoodFlow",
            nullptr,
            flags);

        // ========================================================
        // MENU LATERAL
        // ========================================================

        renderizarMenuLateral();

        ImGui::SameLine(
            0.0f,
            16.0f);

        // ========================================================
        // CONTENIDO PRINCIPAL
        // ========================================================

        ImGui::BeginChild(
            "contenido_principal",
            ImVec2(
                0.0f,
                0.0f),
            false);

        switch (
            vistaActiva_)
        {

            // ====================================================
            // DASHBOARD
            // ====================================================

        case VistaActiva::DASHBOARD:
        {

            vistaDashboard_.renderizar();

            break;
        }

            // ====================================================
            // PEDIDOS
            // ====================================================

        case VistaActiva::PEDIDOS:
        {

            const auto pedidoSeleccionado =
                vistaPedidos_.renderizar();

            if (
                pedidoSeleccionado.has_value())
            {

                vistaDetallePedido_.abrir(
                    pedidoSeleccionado.value());

                vistaActiva_ =
                    VistaActiva::DETALLE_PEDIDO;
            }

            break;
        }

            // ====================================================
            // NUEVO PEDIDO
            // ====================================================

        case VistaActiva::NUEVO_PEDIDO:
        {

            vistaNuevoPedido_.renderizar();

            break;
        }

            // ====================================================
            // DETALLE DE PEDIDO
            // ====================================================

        case VistaActiva::DETALLE_PEDIDO:
        {

            const bool volver =
                vistaDetallePedido_.renderizar();

            if (
                volver)
            {

                vistaPedidos_.recargar();

                vistaDashboard_.recargar();

                vistaActiva_ =
                    VistaActiva::PEDIDOS;
            }

            break;
        }
        }

        ImGui::EndChild();

        ImGui::End();
    }

    int AplicacionGui::ejecutar()
    {

        // ========================================================
        // VENTANA
        // ========================================================

        if (
            !crearVentana())
        {

            return 1;
        }

        // ========================================================
        // DIRECTX
        // ========================================================

        if (
            !crearDirectX())
        {

            destruirVentana();

            return 1;
        }

        ShowWindow(
            ventana_,
            SW_SHOWDEFAULT);

        UpdateWindow(
            ventana_);

        // ========================================================
        // CONTEXTO IMGUI
        // ========================================================

        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO &io =
            ImGui::GetIO();

        io.ConfigFlags |=
            ImGuiConfigFlags_NavEnableKeyboard;

        // ========================================================
        // FUENTE
        // ========================================================

        ImFontConfig configuracionFuente{};

        configuracionFuente.SizePixels =
            19.0f;

        io.Fonts->AddFontDefault(
            &configuracionFuente);

        // ========================================================
        // ESTILO
        // ========================================================

        ImGui::StyleColorsDark();

        configurarEstilo();

        // ========================================================
        // BACKEND WIN32
        // ========================================================

        if (
            !ImGui_ImplWin32_Init(
                ventana_))
        {

            ImGui::DestroyContext();

            liberarDirectX();

            destruirVentana();

            return 1;
        }

        // ========================================================
        // BACKEND DIRECTX 11
        // ========================================================

        if (
            !ImGui_ImplDX11_Init(
                dispositivo_,
                contexto_))
        {

            ImGui_ImplWin32_Shutdown();

            ImGui::DestroyContext();

            liberarDirectX();

            destruirVentana();

            return 1;
        }

        // ========================================================
        // LOOP PRINCIPAL
        // ========================================================

        bool ejecutando =
            true;

        while (
            ejecutando)
        {

            MSG mensaje{};

            while (
                PeekMessageW(
                    &mensaje,
                    nullptr,
                    0,
                    0,
                    PM_REMOVE))
            {

                TranslateMessage(
                    &mensaje);

                DispatchMessageW(
                    &mensaje);

                if (
                    mensaje.message == WM_QUIT)
                {

                    ejecutando =
                        false;
                }
            }

            if (
                !ejecutando)
            {

                break;
            }

            // ====================================================
            // NUEVO FRAME
            // ====================================================

            ImGui_ImplDX11_NewFrame();

            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();

            // ====================================================
            // INTERFAZ
            // ====================================================

            renderizarInterfaz();

            // ====================================================
            // RENDER
            // ====================================================

            ImGui::Render();

            constexpr float fondo[4] = {

                0.040f,

                0.045f,

                0.055f,

                1.00f};

            contexto_->OMSetRenderTargets(
                1,
                &renderTarget_,
                nullptr);

            contexto_->ClearRenderTargetView(
                renderTarget_,
                fondo);

            ImGui_ImplDX11_RenderDrawData(
                ImGui::GetDrawData());

            swapChain_->Present(
                1,
                0);
        }

        // ========================================================
        // LIMPIEZA
        // ========================================================

        ImGui_ImplDX11_Shutdown();

        ImGui_ImplWin32_Shutdown();

        ImGui::DestroyContext();

        liberarDirectX();

        destruirVentana();

        return 0;
    }

    LRESULT AplicacionGui::procesarMensaje(
        HWND hwnd,
        UINT mensaje,
        WPARAM wParam,
        LPARAM lParam)
    {

        // ========================================================
        // IMGUI
        // ========================================================

        if (
            ImGui::GetCurrentContext() != nullptr)
        {

            if (
                ImGui_ImplWin32_WndProcHandler(
                    hwnd,
                    mensaje,
                    wParam,
                    lParam))
            {

                return 1;
            }
        }

        // ========================================================
        // WINDOWS
        // ========================================================

        switch (
            mensaje)
        {

        case WM_SIZE:
        {

            if (
                dispositivo_ && wParam != SIZE_MINIMIZED)
            {

                liberarRenderTarget();

                swapChain_->ResizeBuffers(
                    0,
                    static_cast<UINT>(
                        LOWORD(
                            lParam)),
                    static_cast<UINT>(
                        HIWORD(
                            lParam)),
                    DXGI_FORMAT_UNKNOWN,
                    0);

                crearRenderTarget();
            }

            return 0;
        }

        case WM_SYSCOMMAND:
        {

            if (
                (wParam & 0xFFF0) == SC_KEYMENU)
            {

                return 0;
            }

            break;
        }

        case WM_DESTROY:
        {

            PostQuitMessage(
                0);

            return 0;
        }
        }

        return DefWindowProcW(
            hwnd,
            mensaje,
            wParam,
            lParam);
    }

    LRESULT CALLBACK AplicacionGui::wndProc(
        HWND hwnd,
        UINT mensaje,
        WPARAM wParam,
        LPARAM lParam)
    {

        AplicacionGui *aplicacion =
            reinterpret_cast<
                AplicacionGui *>(
                GetWindowLongPtrW(
                    hwnd,
                    GWLP_USERDATA));

        // ========================================================
        // ASOCIAR HWND CON LA INSTANCIA
        // ========================================================

        if (
            mensaje == WM_NCCREATE)
        {

            const auto *datos =
                reinterpret_cast<
                    CREATESTRUCTW *>(
                    lParam);

            aplicacion =
                static_cast<
                    AplicacionGui *>(
                    datos->lpCreateParams);

            SetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA,
                reinterpret_cast<
                    LONG_PTR>(
                    aplicacion));
        }

        // ========================================================
        // DELEGAR
        // ========================================================

        if (
            aplicacion)
        {

            return aplicacion->procesarMensaje(
                hwnd,
                mensaje,
                wParam,
                lParam);
        }

        return DefWindowProcW(
            hwnd,
            mensaje,
            wParam,
            lParam);
    }

}