#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "interfaces/IProductoRepositorio.hpp"

#include "repositorios/ClienteRepositorio.hpp"

#include "servicios/FacturaCsvServicio.hpp"
#include "servicios/PedidoServicio.hpp"

namespace foodflow
{

    class VistaNuevoPedido
    {
    private:
        struct ClienteOpcion
        {
            std::int64_t id{};

            std::string nombre;

            std::string telefono;

            std::string direccion;
        };

        struct ProductoOpcion
        {
            std::int64_t id{};

            std::string nombre;

            std::string tipo;

            double precio{};

            int cantidad{};
        };

        ClienteRepositorio &clienteRepositorio;

        IProductoRepositorio &productoRepositorio;

        PedidoServicio &pedidoServicio;

        FacturaCsvServicio &facturaServicio;

        std::int64_t restauranteId;

        std::vector<ClienteOpcion> clientes;

        std::vector<ProductoOpcion> productos;

        int clienteSeleccionado;

        int metodoPagoSeleccionado;

        float distanciaKm;

        std::string mensajeError;

        std::string mensajeExito;

        std::string rutaFactura;

        bool procesando;

        void cargarDatos();

        void limpiarFormulario();

        void procesarPedido();

        void renderizarCliente();

        void renderizarProductos();

        void renderizarDatosEntrega();

        void renderizarResultado();

    public:
        VistaNuevoPedido(
            ClienteRepositorio &clienteRepositorio,
            IProductoRepositorio &productoRepositorio,
            PedidoServicio &pedidoServicio,
            FacturaCsvServicio &facturaServicio,
            std::int64_t restauranteId);

        void renderizar();
    };

}