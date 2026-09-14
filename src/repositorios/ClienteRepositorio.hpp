#pragma once

#include <vector>
#include <cstdint>
#include <optional>

#include "db/ConexionPostgres.hpp"
#include "entidades/Cliente.hpp"

namespace foodflow {

class ClienteRepositorio {
private:
    ConexionPostgres& db;

public:
    explicit ClienteRepositorio(ConexionPostgres& conexion);

    std::optional<Cliente> buscarPorId( std::int64_t clienteId);

    std::vector<Cliente> obtenerTodos();
};

}