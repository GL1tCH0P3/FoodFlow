#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "db/ConexionPostgres.hpp"
#include "interfaces/IProductoRepositorio.hpp"

namespace foodflow {

class ProductoRepositorio final : public IProductoRepositorio {
private:
    ConexionPostgres& db;

public:
    explicit ProductoRepositorio(ConexionPostgres& conexion);

    std::vector<std::unique_ptr<Producto>> obtenerDisponibles(std::int64_t restauranteId) override;

    std::unique_ptr<Producto> buscarPorId(std::int64_t productoId, std::int64_t restauranteId) override;
};

}