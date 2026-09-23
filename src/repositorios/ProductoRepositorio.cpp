#include "repositorios/ProductoRepositorio.hpp"
#include "utilidades/utilidades.hpp"

namespace foodflow {

ProductoRepositorio::ProductoRepositorio(ConexionPostgres& conexion)
    : db(conexion) {
}

std::vector<std::unique_ptr<Producto>> ProductoRepositorio::obtenerDisponibles(std::int64_t restauranteId) {
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
                disponible,
                tipo,
                recargo,
                recargo_especial
            FROM foodflow.producto
            WHERE restaurante_id = $1
                AND disponible = TRUE
            ORDER BY id
        )",
        restauranteId
    );

    std::vector<std::unique_ptr<Producto>> productos;

    for (const auto& fila : resultado) {

        std::unique_ptr<Producto> producto = mapearProducto(fila);

        productos.push_back(producto);
    }

    return productos;
}

std::optional<std::unique_ptr<Producto>> ProductoRepositorio::buscarPorId(
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
                disponible,
                tipo,
                recargo,
                recargo_especial
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

    std::unique_ptr<Producto> producto = mapearProducto(fila);

    return producto;
}

std::unique_ptr<Producto> ProductoRepositorio::mapearProducto(const pqxx::row& fila) {

    const auto id = fila["id"].as<std::int64_t>();
    const auto restauranteId = fila["restaurante_id"].as<std::int64_t>();
    const auto nombre = fila["nombre"].as<std::string>();
    const auto descripcion = fila["descripcion"].is_null()
        ? "" 
        : fila["descripcion"].as<std::string>();

    const auto precio = fila["precio"].as<double>();
    const auto disponible = fila["disponible"].as<bool>();
    const auto tipo = fila["tipo"].as<std::string>();
    const auto recargo = fila["recargo"].as<double>();
    const auto recargoEspecial = fila["recargo_especial"].as<double>();

    if (tipo == convTexto(TiposProducto::COMIDA)) {
        return std::make_unique<ProductoComida>(id, restauranteId, nombre, descripcion, precio, disponible, recargo);
    }

    if (tipo == convTexto(TiposProducto::BEBIDA)) {
        return std::make_unique<ProductoBebida>(id, restauranteId, nombre, descripcion, precio, disponible, recargo);
    }

    if (tipo == convTexto(TiposProducto::ESPECIAL)) {
        return std::make_unique<ProductoEspecial>(id, restauranteId, nombre, descripcion, precio, disponible, recargo, recargoEspecial);
    }

    throw std::runtime_error("Tipo de producto desconocido: " + tipo);
}

}