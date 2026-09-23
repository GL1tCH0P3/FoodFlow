#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "entidades/productos/Producto.hpp"

namespace foodflow {

class IProductoRepositorio {

private:
    virtual std::unique_ptr<Producto> mapearProducto(const pqxx::row& fila) = 0;
    
public:
    virtual ~IProductoRepositorio() = default;

    virtual std::vector<std::unique_ptr<Producto>> obtenerDisponibles(std::int64_t restauranteId) = 0;

    virtual std::optional<std::unique_ptr<Producto>> buscarPorId(std::int64_t productoId, std::int64_t restauranteId) = 0;
};

} 