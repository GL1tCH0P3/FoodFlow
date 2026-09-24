#include "servicios/FacturaCsvServicio.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace foodflow {

FacturaCsvServicio::FacturaCsvServicio(
    ClienteRepositorio& clienteRepositorio,
    RestauranteRepositorio& restauranteRepositorio,
    IProductoRepositorio& productoRepositorio
)
    : clienteRepositorio(clienteRepositorio),
      restauranteRepositorio(restauranteRepositorio),
      productoRepositorio(productoRepositorio) {
}


std::string FacturaCsvServicio::escaparCsv(
    const std::string& valor
) const {

    std::string resultado = valor;

    std::size_t posicion = 0;

    while (
        (posicion = resultado.find('"', posicion))
        != std::string::npos
    ) {
        resultado.insert(posicion, "\"");

        posicion += 2;
    }

    return "\"" + resultado + "\"";
}


std::string FacturaCsvServicio::generar(
    const Pedido& pedido
) {

    if (pedido.resultadoValidacion != "CONFIRMADO") {
        throw std::runtime_error(
            "Solo se generan facturas para pedidos confirmados."
        );
    }


    auto cliente =
        clienteRepositorio.buscarPorId(
            pedido.clienteId
        );

    if (!cliente.has_value()) {
        throw std::runtime_error(
            "No fue posible obtener el cliente de la factura."
        );
    }


    auto restaurante =
        restauranteRepositorio.buscarPorId(
            pedido.restauranteId
        );

    if (!restaurante.has_value()) {
        throw std::runtime_error(
            "No fue posible obtener el restaurante de la factura."
        );
    }


    std::filesystem::create_directories(
        "facturas"
    );


    const std::string ruta =
        "facturas/pedido_"
        + std::to_string(pedido.id)
        + ".csv";


    std::ofstream archivo(
        ruta,
        std::ios::binary
    );


    if (!archivo.is_open()) {
        throw std::runtime_error(
            "No fue posible crear la factura CSV."
        );
    }


    // BOM UTF-8 para Excel
    archivo << "\xEF\xBB\xBF";


    archivo
        << "FOODFLOW - FACTURA\n\n"

        << "Pedido;"
        << pedido.id
        << "\n"

        << "Cliente;"
        << escaparCsv(cliente->nombre)
        << "\n"

        << "Restaurante;"
        << escaparCsv(restaurante->nombre)
        << "\n"

        << "Direccion entrega;"
        << escaparCsv(pedido.direccionEntrega)
        << "\n"

        << "Metodo de pago;"
        << escaparCsv(pedido.metodoPago)
        << "\n"

        << "Estado;"
        << pedido.estado.value_or("Sin estado")
        << "\n\n";


    archivo
        << "Producto ID;"
        << "Producto;"
        << "Cantidad;"
        << "Precio unitario;"
        << "Subtotal\n";


    for (const auto& detalle : pedido.detalles) {

        auto producto =
            productoRepositorio.buscarPorId(
                detalle.productoId,
                pedido.restauranteId
            );


        std::string nombreProducto =
            producto
                ? producto->getNombre()
                : "Producto no disponible";


        archivo
            << detalle.productoId
            << ";"
            << escaparCsv(nombreProducto)
            << ";"
            << detalle.cantidad
            << ";"
            << std::fixed
            << std::setprecision(2)
            << detalle.precioUnitario
            << ";"
            << detalle.subtotal
            << "\n";
    }


    archivo
        << "\nSubtotal;"
        << pedido.subtotal
        << "\n"

        << "Costo domicilio;"
        << pedido.costoDomicilio
        << "\n"

        << "TOTAL;"
        << pedido.total
        << "\n";


    archivo.close();


    return ruta;
}

}