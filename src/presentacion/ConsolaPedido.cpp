#include "presentacion/ConsolaPedido.hpp"

#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace foodflow {

ConsolaPedido::ConsolaPedido(
    ClienteRepositorio& clienteRepositorio,
    ProductoRepositorio& productoRepositorio
)
    : clienteRepositorio(clienteRepositorio),
      productoRepositorio(productoRepositorio) {
}


int ConsolaPedido::leerEntero(const std::string& mensaje) const {

    int valor;

    while (true) {

        std::cout << mensaje;

        if (std::cin >> valor) {
            return valor;
        }

        std::cout
            << "Valor invalido. Intente nuevamente.\n";

        std::cin.clear();

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }
}


double ConsolaPedido::leerDecimal(
    const std::string& mensaje
) const {

    double valor;

    while (true) {

        std::cout << mensaje;

        if (std::cin >> valor) {
            return valor;
        }

        std::cout<< "Valor invalido. Intente nuevamente.\n";

        std::cin.clear();

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }
}


SolicitudPedido ConsolaPedido::capturarSolicitud(std::int64_t restauranteId) {

    SolicitudPedido solicitud;

    solicitud.restauranteId = restauranteId;

    // ========================================================
    // CLIENTES
    // ========================================================

    auto clientes = clienteRepositorio.obtenerTodos();
    if (clientes.empty()) {
        throw std::runtime_error(
            "No existen clientes registrados."
        );
    }


    std::cout
        << "\nCLIENTES DISPONIBLES\n"
        << "--------------------------------------------------\n";


    for (const auto& cliente : clientes) {

        std::cout
            << cliente.id
            << ". "
            << cliente.nombre
            << " | "
            << cliente.telefono
            << " | "
            << cliente.direccion
            << "\n";
    }


    int clienteId = leerEntero("\nSeleccione el ID del cliente: ");

    auto cliente = clienteRepositorio.buscarPorId(clienteId);

    if (!cliente.has_value()) {
        throw std::runtime_error(
            "El cliente seleccionado no existe."
        );
    }

    solicitud.clienteId = cliente->id;

    solicitud.direccionEntrega = cliente->direccion;


    // ========================================================
    // PRODUCTOS
    // ========================================================

    auto productos = productoRepositorio.obtenerDisponibles(restauranteId);

    if (productos.empty()) {
        throw std::runtime_error(
            "No existen productos disponibles."
        );
    }

    std::cout
        << "\nPRODUCTOS DISPONIBLES\n"
        << "--------------------------------------------------\n";


    for (const auto& producto : productos) {
        std::cout
            << producto.id
            << ". "
            << producto.nombre
            << " - $"
            << std::fixed
            << std::setprecision(2)
            << producto.precio
            << "\n";
    }


    std::cout
        << "\nIngrese los productos del pedido.\n"
        << "Digite 0 cuando termine.\n\n";


    while (true) {

        int productoId = leerEntero("Producto: ");
        if (productoId == 0) {
            break;
        }

        auto producto = productoRepositorio.buscarPorId(productoId, restauranteId);
        if (!producto.has_value()) {
            std::cout<< "Producto no valido.\n";

            continue;
        }

        int cantidad = leerEntero("Cantidad: ");

        solicitud.items.push_back({productoId, cantidad});

        std::cout
            << producto->nombre
            << " agregado al pedido.\n\n";
    }


    // ========================================================
    // PAGO
    // ========================================================

    std::cout
        << "\nMETODOS DE PAGO\n"
        << "--------------------------------------------------\n"
        << "1. Efectivo\n"
        << "2. Transferencia\n";


    int opcionPago = leerEntero("\nSeleccione metodo de pago: ");

    if (opcionPago == 1) {
        solicitud.metodoPago = "Efectivo";
    }
    else if (opcionPago == 2) {
        solicitud.metodoPago = "Transferencia";
    }
    else {
        // Deliberadamente permitimos que llegue una opción
        // inválida al validador del dominio.
        solicitud.metodoPago = "No soportado";
    }

    solicitud.distanciaKm =  leerDecimal("Distancia de entrega en km: ");

    return solicitud;
}

}