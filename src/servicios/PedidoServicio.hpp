#pragma once

#include "entidades/Pedido.hpp"
#include "modelos/SolicitudPedido.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/RestauranteRepositorio.hpp"
#include "repositorios/ProductoRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"

#include "validadores/PedidoValidador.hpp"

namespace foodflow {

class PedidoServicio {
private:
    ClienteRepositorio& clienteRepositorio;
    RestauranteRepositorio& restauranteRepositorio;
    ProductoRepositorio& productoRepositorio;
    PedidoRepositorio& pedidoRepositorio;

    PedidoValidador& validador;

public:
    PedidoServicio(
        ClienteRepositorio& clienteRepositorio,
        RestauranteRepositorio& restauranteRepositorio,
        ProductoRepositorio& productoRepositorio,
        PedidoRepositorio& pedidoRepositorio,
        PedidoValidador& validador
    );

    Pedido procesar(
        const SolicitudPedido& solicitud
    );
};

}