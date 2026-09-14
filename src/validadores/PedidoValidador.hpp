#pragma once

#include <string>

#include "modelos/SolicitudPedido.hpp"

namespace foodflow {

struct ResultadoValidacion {
    bool valido{};
    std::string codigo;
    std::string mensaje;
};


class PedidoValidador {
public:
    ResultadoValidacion validarDatosBasicos(
        const SolicitudPedido& solicitud
    ) const;

    bool pagoValido(
        const std::string& metodoPago
    ) const;
};

}