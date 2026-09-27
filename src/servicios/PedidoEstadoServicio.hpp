#pragma once

#include <cstdint>
#include <string>

#include "repositorios/PedidoRepositorio.hpp"

namespace foodflow
{

    class PedidoEstadoServicio
    {
    private:
        PedidoRepositorio &pedidoRepositorio;

    public:
        explicit PedidoEstadoServicio(
            PedidoRepositorio &pedidoRepositorio);

        std::string descripcion(
            const std::string &estado) const;

        bool puedeAvanzar(
            const std::string &estado) const;

        std::string siguienteEstado(
            const std::string &estado) const;

        std::string avanzar(
            std::int64_t pedidoId,
            const std::string &estadoActual);
    };

}