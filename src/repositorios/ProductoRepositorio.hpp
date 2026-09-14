#pragma once

#include <cstdint>
#include <vector>

#include "db/ConexionPostgres.hpp"
#include "entidades/Producto.hpp"

namespace foodflow {

class ProductoRepositorio {
private:
    ConexionPostgres& db;

public:
    explicit ProductoRepositorio(ConexionPostgres& conexion);

    std::vector<Producto> obtenerDisponibles(std::int64_t restauranteId);

    std::optional<Producto> buscarPorId(std::int64_t productoId, std::int64_t restauranteId);
};

}