#pragma once

#include "entidades/Pedido.hpp"
#include "interfaces/IProductoRepositorio.hpp"
#include "modelos/SolicitudPedido.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"
#include "repositorios/RestauranteRepositorio.hpp"

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