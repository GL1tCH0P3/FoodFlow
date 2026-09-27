#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "entidades/estados/EstadoEnCamino.hpp"
#include "entidades/estados/EstadoEntregado.hpp"
#include "entidades/estados/EstadoPreparando.hpp"
#include "entidades/estados/EstadoRecibido.hpp"

namespace foodflow {

inline std::unique_ptr<EstadoPedido>
crearEstadoPedido(
    const std::string& codigo
) {

    if (codigo == "RECIBIDO") {

        return std::make_unique<
            EstadoRecibido
        >();
    }


    if (codigo == "PREPARANDO") {

        return std::make_unique<
            EstadoPreparando
        >();
    }


    if (codigo == "EN_CAMINO") {

        return std::make_unique<
            EstadoEnCamino
        >();
    }


    if (codigo == "ENTREGADO") {

        return std::make_unique<
            EstadoEntregado
        >();
    }


    throw std::runtime_error(
        "Estado de pedido desconocido: "
        + codigo
    );
}

}