#pragma once

#include <cstdint>
#include <string>

namespace foodflow {

struct PedidoResumen {
    std::int64_t id{};
    std::string fecha;

    std::string cliente;
    std::string restaurante;

    std::string metodoPago;

    double subtotal{};
    double costoDomicilio{};
    double total{};

    std::string resultadoValidacion;
    std::string estado;
};

}