#pragma once

#include <cstdint>
#include <vector>

#include "db/ConexionPostgres.hpp"
#include "entidades/Pedido.hpp"
#include "modelos/PedidoResumen.hpp"

namespace foodflow {

class PedidoRepositorio {
private:
    ConexionPostgres& db;

public:
    explicit PedidoRepositorio(ConexionPostgres& conexion);

    std::int64_t guardar(Pedido& pedido);

    std::vector<PedidoResumen> obtenerHistorico();
};

}