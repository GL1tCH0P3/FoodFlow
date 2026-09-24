#pragma once

#include <cstdint>
#include <string>

#include "interfaces/IProductoRepositorio.hpp"
#include "modelos/SolicitudPedido.hpp"
#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"

namespace foodflow {

class ConsolaPedido {
private:
    ClienteRepositorio& clienteRepositorio;

    IProductoRepositorio& productoRepositorio;

    int leerEntero(const std::string& mensaje) const;

    double leerDecimal(const std::string& mensaje) const;

public:
    ConsolaPedido(
        ClienteRepositorio& clienteRepositorio,
        IProductoRepositorio& productoRepositorio
    );

    SolicitudPedido capturarSolicitud(std::int64_t restauranteId);

    void mostrarHistorico(PedidoRepositorio& pedidoRepositorio) const; 
};

}