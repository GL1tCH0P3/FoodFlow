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

}