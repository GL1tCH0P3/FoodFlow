#pragma once

#include "entidades/productos/Producto.hpp"
#include "modelos/SolicitudPedido.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/RestauranteRepositorio.hpp"
#include "repositorios/ProductoRepositorio.hpp"
#include "interfaces/IProductoRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"

#include "validadores/PedidoValidador.hpp"

namespace foodflow {

class PedidoServicio {
private:
    ClienteRepositorio& clienteRepositorio;
    RestauranteRepositorio& restauranteRepositorio;
    IProductoRepositorio& productoRepositorio;
    PedidoRepositorio& pedidoRepositorio;

    PedidoValidador& validador;

public:
    PedidoServicio(
        ClienteRepositorio& clienteRepositorio,
        RestauranteRepositorio& restauranteRepositorio,
        IProductoRepositorio& productoRepositorio,
        PedidoRepositorio& pedidoRepositorio,
        PedidoValidador& validador
    );

    Pedido procesar(
        const SolicitudPedido& solicitud
    );
};

}