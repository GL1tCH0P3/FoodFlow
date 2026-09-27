#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "modelos/PedidoDetalleVista.hpp"

#include "repositorios/PedidoRepositorio.hpp"

#include "servicios/FacturaCsvServicio.hpp"
#include "servicios/PedidoEstadoServicio.hpp"

namespace foodflow
{

    class VistaDetallePedido
    {
    private:
        PedidoRepositorio &pedidoRepositorio;

        PedidoEstadoServicio &pedidoEstadoServicio;

        FacturaCsvServicio &facturaServicio;

        std::optional<PedidoDetalleVista> pedido;

        std::string mensajeError;

        std::string mensajeExito;

        std::string rutaFactura;

        void cargar(
            std::int64_t pedidoId);

    public:
        VistaDetallePedido(
            PedidoRepositorio &pedidoRepositorio,
            PedidoEstadoServicio &pedidoEstadoServicio,
            FacturaCsvServicio &facturaServicio);

        void abrir(
            std::int64_t pedidoId);

        bool renderizar();
    };

}