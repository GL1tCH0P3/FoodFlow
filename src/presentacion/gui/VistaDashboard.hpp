#pragma once

#include <string>
#include <vector>

#include "modelos/PedidoResumen.hpp"
#include "repositorios/PedidoRepositorio.hpp"

namespace foodflow
{

    class VistaDashboard
    {
    private:
        PedidoRepositorio &pedidoRepositorio;

        std::vector<PedidoResumen> pedidos;

        std::string mensajeError;

        bool cargando;

        void cargarDatos();

        void renderizarTarjeta(
            const char *titulo,
            const std::string &valor,
            const char *descripcion);

    public:
        explicit VistaDashboard(
            PedidoRepositorio &pedidoRepositorio);

        void recargar();

        void renderizar();
    };

}