#pragma once

#include <cstdint>

#include "modelos/SolicitudPedido.hpp"
#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/ProductoRepositorio.hpp"

namespace foodflow {

class ConsolaPedido {
private:
    ClienteRepositorio& clienteRepositorio;
    ProductoRepositorio& productoRepositorio;

    int leerEntero(
        const std::string& mensaje
    ) const;

    double leerDecimal(
        const std::string& mensaje
    ) const;

public:
    ConsolaPedido(
        ClienteRepositorio& clienteRepositorio,
        ProductoRepositorio& productoRepositorio
    );

    SolicitudPedido capturarSolicitud(std::int64_t restauranteId);
};

}