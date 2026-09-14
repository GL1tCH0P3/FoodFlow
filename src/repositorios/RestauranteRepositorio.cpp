#include "repositorios/RestauranteRepositorio.hpp"

namespace foodflow {

RestauranteRepositorio::RestauranteRepositorio(ConexionPostgres& conexion)
    : db(conexion) {
}


std::optional<Restaurante> RestauranteRepositorio::buscarPorId(std::int64_t restauranteId) {
    pqxx::read_transaction tx{db.obtener()};

    pqxx::result resultado = tx.exec_params(
        R"(
            SELECT
                id,
                nombre,
                direccion,
                telefono,
                activo
            FROM foodflow.restaurante
            WHERE id = $1
            LIMIT 1
        )",
        restauranteId
    );

    if (resultado.empty()) {
        return std::nullopt;
    }

    const auto& fila = resultado[0];

    Restaurante restaurante;

    restaurante.id = fila["id"].as<std::int64_t>();

    restaurante.nombre = fila["nombre"].as<std::string>();

    restaurante.direccion = fila["direccion"].as<std::string>();

    restaurante.telefono = fila["telefono"].as<std::string>();

    restaurante.activo = fila["activo"].as<bool>();

    return restaurante;
}

}