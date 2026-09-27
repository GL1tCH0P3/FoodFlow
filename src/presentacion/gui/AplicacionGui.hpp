#pragma once

#include <cstdint>

#include <d3d11.h>
#include <windows.h>

#include "interfaces/IProductoRepositorio.hpp"

#include "presentacion/gui/VistaDashboard.hpp"
#include "presentacion/gui/VistaDetallePedido.hpp"
#include "presentacion/gui/VistaNuevoPedido.hpp"
#include "presentacion/gui/VistaPedidos.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"

#include "servicios/FacturaCsvServicio.hpp"
#include "servicios/PedidoEstadoServicio.hpp"
#include "servicios/PedidoServicio.hpp"

namespace foodflow
{

    class AplicacionGui
    {
    private:
        enum class VistaActiva
        {
            DASHBOARD,
            PEDIDOS,
            NUEVO_PEDIDO,
            DETALLE_PEDIDO
        };

        HINSTANCE instancia_;

        HWND ventana_;

        ID3D11Device *dispositivo_;

        ID3D11DeviceContext *contexto_;

        IDXGISwapChain *swapChain_;

        ID3D11RenderTargetView *renderTarget_;

        VistaActiva vistaActiva_;

        VistaDashboard vistaDashboard_;

        VistaPedidos vistaPedidos_;

        VistaNuevoPedido vistaNuevoPedido_;

        VistaDetallePedido vistaDetallePedido_;

        bool crearVentana();

        bool crearDirectX();

        void crearRenderTarget();

        void liberarRenderTarget();

        void liberarDirectX();

        void destruirVentana();

        void configurarEstilo();

        void renderizarMenuLateral();

        void renderizarInterfaz();

        LRESULT procesarMensaje(
            HWND hwnd,
            UINT mensaje,
            WPARAM wParam,
            LPARAM lParam);

        static LRESULT CALLBACK wndProc(
            HWND hwnd,
            UINT mensaje,
            WPARAM wParam,
            LPARAM lParam);

    public:
        AplicacionGui(
            PedidoRepositorio &pedidoRepositorio,
            ClienteRepositorio &clienteRepositorio,
            IProductoRepositorio &productoRepositorio,
            PedidoServicio &pedidoServicio,
            FacturaCsvServicio &facturaServicio,
            PedidoEstadoServicio &pedidoEstadoServicio,
            std::int64_t restauranteId);

        int ejecutar();
    };

}