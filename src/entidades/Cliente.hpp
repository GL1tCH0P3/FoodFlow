#pragma once

#include <cstdint>
#include <string>

namespace foodflow {

struct Cliente {
    std::int64_t id{};
    std::string nombre;
    std::string telefono;
    std::string direccion;
};

}