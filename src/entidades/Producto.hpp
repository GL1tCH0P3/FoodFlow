#pragma once

#include <cstdint>
#include <string>

namespace foodflow {

struct Producto {
    std::int64_t id{};
    std::int64_t restauranteId{};

    std::string nombre;
    std::string descripcion;

    double precio{};
    bool disponible{true};
};

}