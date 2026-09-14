#include "repositorios/ClienteRepositorio.hpp"

namespace foodflow {

ClienteRepositorio::ClienteRepositorio(
    ConexionPostgres& conexion
)
    : db(conexion) {
}


std::optional<Cliente> ClienteRepositorio::buscarPorId(std::int64_t clienteId) {
    pqxx::read_transaction tx{db.obtener()};

    pqxx::result resultado = tx.exec_params(
        R"(
            SELECT
                id,
                nombre,
                telefono,
                direccion
            FROM foodflow.cliente
            WHERE id = $1
            LIMIT 1
        )",
        clienteId
    );

    if (resultado.empty()) {
        return std::nullopt;
    }

    const auto& fila = resultado[0];

    Cliente cliente;

    cliente.id = fila["id"].as<std::int64_t>();

    cliente.nombre = fila["nombre"].as<std::string>();

    cliente.telefono = fila["telefono"].as<std::string>();

    cliente.direccion = fila["direccion"].as<std::string>();

    return cliente;
}

std::vector<Cliente>
ClienteRepositorio::obtenerTodos() {

    pqxx::read_transaction tx{db.obtener()};

    pqxx::result resultado = tx.exec(
        R"(
            SELECT
                id,
                nombre,
                telefono,
                direccion
            FROM foodflow.cliente
            ORDER BY id
        )"
    );

    std::vector<Cliente> clientes;

    for (const auto& fila : resultado) {

        Cliente cliente;

        cliente.id = fila["id"].as<std::int64_t>();

        cliente.nombre = fila["nombre"].as<std::string>();

        cliente.telefono = fila["telefono"].as<std::string>();

        cliente.direccion = fila["direccion"].as<std::string>();

        clientes.push_back(cliente);
    }

    return clientes;
}

}