#pragma once

#include <string>

#include "entidades/Pedido.hpp"

#include "interfaces/IProductoRepositorio.hpp"

#include "modelos/PedidoDetalleVista.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/RestauranteRepositorio.hpp"

namespace foodflow
{

    class FacturaCsvServicio
    {
    private:
        ClienteRepositorio &clienteRepositorio;

        RestauranteRepositorio &restauranteRepositorio;

        IProductoRepositorio &productoRepositorio;

        std::string escaparCsv(
            const std::string &valor) const;

    public:
        FacturaCsvServicio(
            ClienteRepositorio &clienteRepositorio,
            RestauranteRepositorio &restauranteRepositorio,
            IProductoRepositorio &productoRepositorio);

        // Factura de un pedido recién creado.
        std::string generar(
            const Pedido &pedido);

        // Factura de un pedido consultado desde el histórico.
        std::string generar(
            const PedidoDetalleVista &pedido);
    };

}