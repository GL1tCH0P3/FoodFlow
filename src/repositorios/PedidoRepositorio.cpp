#include "repositorios/PedidoRepositorio.hpp"

namespace foodflow {

PedidoRepositorio::PedidoRepositorio(
    ConexionPostgres& conexion
)
    : db(conexion) {
}


std::int64_t PedidoRepositorio::guardar(Pedido& pedido) {
    pqxx::work tx{db.obtener()};

    pqxx::result resultado = tx.exec_params(
        R"(
            INSERT INTO foodflow.pedido (
                cliente_id,
                restaurante_id,
                direccion_entrega,
                metodo_pago,
                distancia_km,
                subtotal,
                costo_domicilio,
                total,
                tiempo_estimado_min,
                resultado_validacion,
                estado,
                mensaje
            )
            VALUES (
                $1, $2, $3, $4, $5, $6,
                $7, $8, $9, $10, $11, $12
            )
            RETURNING id
        )",
        pedido.clienteId,
        pedido.restauranteId,
        pedido.direccionEntrega,
        pedido.metodoPago,
        pedido.distanciaKm,
        pedido.subtotal,
        pedido.costoDomicilio,
        pedido.total,
        pedido.tiempoEstimadoMin,
        pedido.resultadoValidacion,
        pedido.estado,
        pedido.mensaje
    );

    pedido.id = resultado[0]["id"].as<std::int64_t>();

    for (auto& detalle : pedido.detalles) {

        pqxx::result resultadoDetalle =
            tx.exec_params(
                R"(
                    INSERT INTO foodflow.pedido_detalle (
                        pedido_id,
                        producto_id,
                        cantidad,
                        precio_unitario
                    )
                    VALUES ($1, $2, $3, $4)
                    RETURNING id, subtotal
                )",
                pedido.id,
                detalle.productoId,
                detalle.cantidad,
                detalle.precioUnitario
            );

        detalle.id = resultadoDetalle[0]["id"].as<std::int64_t>();

        detalle.pedidoId = pedido.id;

        detalle.subtotal = resultadoDetalle[0]["subtotal"].as<double>();
    }


    tx.commit();

    return pedido.id;
}


std::vector<PedidoResumen>
PedidoRepositorio::obtenerHistorico() {

    pqxx::read_transaction transaccion{
        db.obtener()
    };

    pqxx::result resultado =
        transaccion.exec(
            R"(
                SELECT
                    p.id,
                    p.fecha,
                    c.nombre AS cliente,
                    r.nombre AS restaurante,
                    p.metodo_pago,
                    p.subtotal,
                    p.costo_domicilio,
                    p.total,
                    p.resultado_validacion,
                    COALESCE(p.estado, '') AS estado

                FROM foodflow.pedido p

                INNER JOIN foodflow.cliente c
                    ON c.id = p.cliente_id

                INNER JOIN foodflow.restaurante r
                    ON r.id = p.restaurante_id

                ORDER BY p.id DESC
            )"
        );


    std::vector<PedidoResumen> pedidos;

    pedidos.reserve(resultado.size());


    for (const auto& fila : resultado) {

        PedidoResumen pedido;

        pedido.id = fila["id"].as<std::int64_t>();

        pedido.fecha = fila["fecha"].as<std::string>();

        pedido.cliente = fila["cliente"].as<std::string>();

        pedido.restaurante = fila["restaurante"].as<std::string>();

        pedido.metodoPago = fila["metodo_pago"].as<std::string>();

        pedido.subtotal = fila["subtotal"].as<double>();

        pedido.costoDomicilio = fila["costo_domicilio"].as<double>();

        pedido.total = fila["total"].as<double>();

        pedido.resultadoValidacion = fila["resultado_validacion"].as<std::string>();

        pedido.estado = fila["estado"].as<std::string>();

        pedidos.push_back(pedido);
    }


    return pedidos;
}

}