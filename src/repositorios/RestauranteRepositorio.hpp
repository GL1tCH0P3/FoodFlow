#pragma once

#include <cstdint>
#include <optional>

#include "db/ConexionPostgres.hpp"
#include "entidades/Restaurante.hpp"

namespace foodflow {

class RestauranteRepositorio {
private:
    ConexionPostgres& db;

public:
    explicit RestauranteRepositorio(ConexionPostgres& conexion);

    std::optional<Restaurante> buscarPorId(std::int64_t restauranteId);
};

}