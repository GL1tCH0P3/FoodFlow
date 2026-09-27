#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "modelos/PedidoResumen.hpp"
#include "repositorios/PedidoRepositorio.hpp"

namespace foodflow
{

    class VistaPedidos
    {
    private:
        PedidoRepositorio &pedidoRepositorio;

        std::vector<PedidoResumen> pedidos;

        std::string mensajeError;

        bool cargando;

        void cargarPedidos();

    public:
        explicit VistaPedidos(
            PedidoRepositorio &pedidoRepositorio);

        void recargar();

        std::optional<std::int64_t>
        renderizar();
    };

}