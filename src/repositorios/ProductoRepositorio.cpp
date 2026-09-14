#include "repositorios/ProductoRepositorio.hpp"

namespace foodflow {

ProductoRepositorio::ProductoRepositorio(ConexionPostgres& conexion)
    : db(conexion) {
}

std::vector<Producto> ProductoRepositorio::obtenerDisponibles(std::int64_t restauranteId) {
    pqxx::read_transaction transaccion{
        db.obtener()
    };

    pqxx::result resultado = transaccion.exec_params(
        R"(
            SELECT
                id,
                restaurante_id,
                nombre,
                descripcion,
                precio,
                disponible
            FROM foodflow.producto
            WHERE restaurante_id = $1
                AND disponible = TRUE
            ORDER BY id
        )",
        restauranteId
    );

    std::vector<Producto> productos;

    for (const auto& fila : resultado) {

        Producto producto;

        producto.id = fila["id"].as<std::int64_t>();

        producto.restauranteId = fila["restaurante_id"].as<std::int64_t>();

        producto.nombre = fila["nombre"].as<std::string>();

        producto.descripcion = fila["descripcion"].is_null()
            ? ""
            : fila["descripcion"].as<std::string>();

        producto.precio = fila["precio"].as<double>();

        producto.disponible = fila["disponible"].as<bool>();

        productos.push_back(producto);
    }

    return productos;
}

std::optional<Producto>ProductoRepositorio::buscarPorId(
    std::int64_t productoId,
    std::int64_t restauranteId
) {
    pqxx::read_transaction transaccion{
        db.obtener()
    };

    pqxx::result resultado = transaccion.exec_params(
        R"(
            SELECT
                id,
                restaurante_id,
                nombre,
                descripcion,
                precio,
                disponible
            FROM foodflow.producto
            WHERE id = $1
                AND restaurante_id = $2
            LIMIT 1
        )",
        productoId,
        restauranteId
    );

    if (resultado.empty()) {
        return std::nullopt;
    }

    const auto& fila = resultado[0];

    Producto producto;

    producto.id = fila["id"].as<std::int64_t>();

    producto.restauranteId = fila["restaurante_id"].as<std::int64_t>();

    producto.nombre = fila["nombre"].as<std::string>();

    producto.descripcion = fila["descripcion"].is_null()
        ? ""
        : fila["descripcion"].as<std::string>();

    producto.precio = fila["precio"].as<double>();

    producto.disponible = fila["disponible"].as<bool>();

    return producto;
}

}