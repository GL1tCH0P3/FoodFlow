#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "entidades/productos/Producto.hpp"

namespace foodflow {

class IProductoRepositorio {
public:
    virtual ~IProductoRepositorio() = default;

    virtual std::vector<std::unique_ptr<Producto>> obtenerDisponibles(std::int64_t restauranteId) = 0;

    virtual std::unique_ptr<Producto> buscarPorId(
        std::int64_t productoId,
        std::int64_t restauranteId
    ) = 0;
};

}