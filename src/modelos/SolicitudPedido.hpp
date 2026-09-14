#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace foodflow {

struct ItemSolicitudPedido {
    std::int64_t productoId{};
    int cantidad{};
};


struct SolicitudPedido {
    std::int64_t clienteId{};
    std::int64_t restauranteId{};

    std::string direccionEntrega;
    std::string metodoPago;

    double distanciaKm{};

    std::vector<ItemSolicitudPedido> items;
};

}