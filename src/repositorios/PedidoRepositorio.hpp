#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "db/ConexionPostgres.hpp"

#include "entidades/Pedido.hpp"

#include "modelos/PedidoDetalleVista.hpp"
#include "modelos/PedidoResumen.hpp"

namespace foodflow
{

    class PedidoRepositorio
    {
    private:
        ConexionPostgres &db;

    public:
        explicit PedidoRepositorio(
            ConexionPostgres &conexion);

        std::int64_t guardar(
            Pedido &pedido);

        std::vector<PedidoResumen>
        obtenerHistorico();

        std::optional<PedidoDetalleVista>
        buscarDetallePorId(
            std::int64_t pedidoId);

        bool actualizarEstado(
            std::int64_t pedidoId,
            const std::string &estadoActual,
            const std::string &nuevoEstado);
    };

}