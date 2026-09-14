#include "validadores/PedidoValidador.hpp"

namespace foodflow {

bool PedidoValidador::pagoValido(
    const std::string& metodoPago
) const {
    return metodoPago == "Efectivo" || metodoPago == "Transferencia";
}


ResultadoValidacion PedidoValidador::validarDatosBasicos(
    const SolicitudPedido& solicitud
) const {

    if (solicitud.items.empty()) {
        return {
            false,
            "RECHAZADO",
            "El pedido debe contener al menos un producto."
        };
    }

    if (solicitud.distanciaKm < 0.0) {
        return {
            false,
            "RECHAZADO",
            "La distancia no puede ser negativa."
        };
    }

    if (!pagoValido(solicitud.metodoPago)) {
        return {
            false,
            "REQUIERE_VALIDACION_PAGO",
            "Metodo de pago no disponible."
        };
    }

    for (const auto& item : solicitud.items) {

        if (item.cantidad <= 0) {
            return {
                false,
                "RECHAZADO",
                "La cantidad debe ser mayor que cero."
            };
        }
    }

    return {
        true,
        "VALIDO",
        "Datos basicos validos."
    };
}

}