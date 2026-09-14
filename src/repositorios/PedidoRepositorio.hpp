#pragma once

#include <cstdint>

#include "db/ConexionPostgres.hpp"
#include "entidades/Pedido.hpp"

namespace foodflow {

class PedidoRepositorio {
private:
    ConexionPostgres& db;

public:
    explicit PedidoRepositorio(ConexionPostgres& conexion);

    std::int64_t guardar(Pedido& pedido);
};

}