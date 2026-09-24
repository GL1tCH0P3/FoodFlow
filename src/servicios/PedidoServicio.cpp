#include "servicios/PedidoServicio.hpp"

#include <cmath>
#include <stdexcept>

#include "config/ReglasNegocio.hpp"

namespace foodflow {

PedidoServicio::PedidoServicio(
    ClienteRepositorio& clienteRepositorio,
    RestauranteRepositorio& restauranteRepositorio,
    IProductoRepositorio& productoRepositorio,
    PedidoRepositorio& pedidoRepositorio,
    PedidoValidador& validador
)
    : clienteRepositorio(clienteRepositorio),
      restauranteRepositorio(restauranteRepositorio),
      productoRepositorio(productoRepositorio),
      pedidoRepositorio(pedidoRepositorio),
      validador(validador) {
}


Pedido PedidoServicio::procesar(const SolicitudPedido& solicitud) {
    Pedido pedido;

    pedido.clienteId = solicitud.clienteId;

    pedido.restauranteId = solicitud.restauranteId;

    pedido.direccionEntrega = solicitud.direccionEntrega;

    pedido.metodoPago = solicitud.metodoPago;

    pedido.distanciaKm = solicitud.distanciaKm;


    // ---------------------------------------------------------
    // Cliente
    // ---------------------------------------------------------

    auto cliente = clienteRepositorio.buscarPorId(solicitud.clienteId);
    if (!cliente.has_value()) {
        throw std::runtime_error(
            "El cliente indicado no existe."
        );
    }


    // ---------------------------------------------------------
    // Restaurante
    // ---------------------------------------------------------

    auto restaurante = restauranteRepositorio.buscarPorId(solicitud.restauranteId);
    if (!restaurante.has_value()) {
        throw std::runtime_error(
            "El restaurante indicado no existe."
        );
    }

    if (!restaurante->activo) {
        throw std::runtime_error(
            "El restaurante no se encuentra activo."
        );
    }

    // ---------------------------------------------------------
    // Validaciones iniciales
    // ---------------------------------------------------------

    ResultadoValidacion validacion = validador.validarDatosBasicos(solicitud);

    if (!validacion.valido) {
        pedido.resultadoValidacion = validacion.codigo;

        pedido.mensaje = validacion.mensaje;

        pedidoRepositorio.guardar(pedido);

        return pedido;
    }


    // ---------------------------------------------------------
    // Resolver productos desde PostgreSQL
    // ---------------------------------------------------------

    for (const auto& itemSolicitud : solicitud.items) {

        auto producto = productoRepositorio.buscarPorId(itemSolicitud.productoId, solicitud.restauranteId);
        if (!producto) {
            pedido.resultadoValidacion = "RECHAZADO";

            pedido.mensaje = "Uno de los productos solicitados no existe.";

            pedidoRepositorio.guardar(pedido);

            return pedido;
        }

        if (!producto->estaDisponible()) {

            pedido.resultadoValidacion = "RECHAZADO";

            pedido.mensaje = "Uno de los productos no se encuentra disponible.";

            pedidoRepositorio.guardar(pedido);

            return pedido;
        }

        PedidoDetalle detalle;

        detalle.productoId = producto->getId();

        detalle.cantidad = itemSolicitud.cantidad;

        detalle.precioUnitario = producto->calcularPrecioFinal();

        detalle.subtotal = detalle.cantidad * detalle.precioUnitario;

        pedido.detalles.push_back(detalle);

        pedido.subtotal += detalle.subtotal;
    }


    // ---------------------------------------------------------
    // Cobertura
    // ---------------------------------------------------------

    if (solicitud.distanciaKm > reglas::LIMITE_DISTANCIA_KM) {
        pedido.resultadoValidacion = "REQUIERE_REVISION_COBERTURA";

        pedido.mensaje = "La direccion supera la cobertura configurada.";

        pedidoRepositorio.guardar(pedido);

        return pedido;
    }


    // ---------------------------------------------------------
    // Cálculos
    // ---------------------------------------------------------

    pedido.costoDomicilio = solicitud.distanciaKm * reglas::COSTO_POR_KM;

    pedido.total = pedido.subtotal + pedido.costoDomicilio;

    pedido.tiempoEstimadoMin = reglas::TIEMPO_BASE_MIN + static_cast<int>(
            std::round(solicitud.distanciaKm * reglas::TIEMPO_POR_KM_MIN)
        );


    pedido.resultadoValidacion = "CONFIRMADO";

    pedido.estado = "RECIBIDO";

    pedido.mensaje = "Pedido validado correctamente.";


    // ---------------------------------------------------------
    // Persistencia
    // ---------------------------------------------------------

    pedidoRepositorio.guardar(pedido);

    return pedido;
}

}