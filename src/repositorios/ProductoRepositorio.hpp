#pragma once

#include <cstdint>
#include <vector>

#include "db/ConexionPostgres.hpp"
#include "entidades/productos/Producto.hpp"
#include "interfaces/IProductoRepositorio.hpp"

namespace foodflow {

class ProductoRepositorio : public IProductoRepositorio {
private:
    ConexionPostgres& db;

    std::unique_ptr<Producto> mapearProducto(const pqxx::row& fila) override;


public:
    explicit ProductoRepositorio(ConexionPostgres& conexion);

    std::vector<std::unique_ptr<Producto>> obtenerDisponibles(std::int64_t restauranteId) override;

    std::optional<std::unique_ptr<Producto>> buscarPorId(std::int64_t productoId, std::int64_t restauranteId) override;
};

}