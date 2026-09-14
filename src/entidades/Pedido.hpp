#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "PedidoDetalle.hpp"

namespace foodflow {

struct Pedido {
    std::int64_t id{};

    std::int64_t clienteId{};
    std::int64_t restauranteId{};

    std::string direccionEntrega;
    std::string metodoPago;

    double distanciaKm{};

    double subtotal{};
    double costoDomicilio{};
    double total{};

    int tiempoEstimadoMin{};

    std::string resultadoValidacion;
    std::optional<std::string> estado;

    std::string mensaje;

    std::vector<PedidoDetalle> detalles;
};

}