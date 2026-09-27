#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace foodflow
{

    struct ProductoPedidoVista
    {
        std::int64_t productoId{};

        std::string nombre;

        int cantidad{};

        double precioUnitario{};

        double subtotal{};
    };

    struct PedidoDetalleVista
    {
        std::int64_t id{};

        std::string fecha;

        std::string cliente;

        std::string restaurante;

        std::string direccionEntrega;

        std::string metodoPago;

        double subtotal{};

        double costoDomicilio{};

        double total{};

        std::string resultadoValidacion;

        std::string estado;

        std::vector<ProductoPedidoVista> productos;
    };

}