#pragma once

#include <cstdint>

namespace foodflow {

struct PedidoDetalle {
    std::int64_t id{};
    std::int64_t pedidoId{};
    std::int64_t productoId{};

    int cantidad{};

    double precioUnitario{};
    double subtotal{};
};

}