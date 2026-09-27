#include "servicios/PedidoEstadoServicio.hpp"

#include <stdexcept>

#include "entidades/estados/EstadoPedidoFactory.hpp"

namespace foodflow
{

    PedidoEstadoServicio::PedidoEstadoServicio(
        PedidoRepositorio &pedidoRepositorio)
        : pedidoRepositorio(
              pedidoRepositorio)
    {
    }

    std::string
    PedidoEstadoServicio::descripcion(
        const std::string &estado) const
    {

        auto objetoEstado = crearEstadoPedido(estado);

        return objetoEstado->descripcionEstado();
    }

    bool
    PedidoEstadoServicio::puedeAvanzar(
        const std::string &estado) const
    {

        auto objetoEstado = crearEstadoPedido(estado);

        return !objetoEstado->esFinal();
    }

    std::string
    PedidoEstadoServicio::siguienteEstado(
        const std::string &estado) const
    {

        auto objetoEstado = crearEstadoPedido(estado);

        return objetoEstado->siguienteEstado();
    }

    std::string
    PedidoEstadoServicio::avanzar(
        std::int64_t pedidoId,
        const std::string &estadoActual)
    {

        auto estado = crearEstadoPedido(estadoActual);

        if (estado->esFinal())
        {
            throw std::runtime_error(
                "El pedido ya fue entregado.");
        }

        const std::string nuevoEstado = estado->siguienteEstado();

        const bool actualizado = pedidoRepositorio.actualizarEstado(pedidoId, estadoActual, nuevoEstado);

        if (!actualizado)
        {
            throw std::runtime_error(
                "No fue posible actualizar el estado. "
                "El pedido pudo haber cambiado previamente.");
        }

        return nuevoEstado;
    }

}