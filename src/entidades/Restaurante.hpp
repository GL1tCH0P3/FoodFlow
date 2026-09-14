#pragma once

#include <cstdint>
#include <string>

namespace foodflow {

struct Restaurante {
    std::int64_t id{};
    std::string nombre;
    std::string direccion;
    std::string telefono;
    bool activo{true};
};

}