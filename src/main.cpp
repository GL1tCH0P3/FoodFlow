#include <cstdint>
#include <exception>
#include <iostream>

#include "config/ConfiguracionBD.hpp"

#include "db/ConexionPostgres.hpp"

#include "presentacion/gui/AplicacionGui.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"
#include "repositorios/ProductoRepositorio.hpp"
#include "repositorios/RestauranteRepositorio.hpp"

#include "servicios/FacturaCsvServicio.hpp"
#include "servicios/PedidoEstadoServicio.hpp"
#include "servicios/PedidoServicio.hpp"

#include "validadores/PedidoValidador.hpp"

using namespace foodflow;

int main()
{

    try
    {

        // ====================================================
        // CONFIGURACION
        // ====================================================

        ConfiguracionBD configuracion =
            ConfiguracionBD::desdeEntorno();

        // ====================================================
        // CONEXION POSTGRESQL
        // ====================================================

        ConexionPostgres db{
            configuracion};

        // ====================================================
        // REPOSITORIOS
        // ====================================================

        ClienteRepositorio clienteRepositorio{
            db};

        RestauranteRepositorio restauranteRepositorio{
            db};

        ProductoRepositorio productoRepositorio{
            db};

        PedidoRepositorio pedidoRepositorio{
            db};

        // ====================================================
        // VALIDADORES
        // ====================================================

        PedidoValidador pedidoValidador;

        // ====================================================
        // SERVICIO DE PEDIDOS
        // ====================================================

        PedidoServicio pedidoServicio{
            clienteRepositorio,
            restauranteRepositorio,
            productoRepositorio,
            pedidoRepositorio,
            pedidoValidador};

        // ====================================================
        // SERVICIO DE FACTURACION
        // ====================================================

        FacturaCsvServicio facturaServicio{
            clienteRepositorio,
            restauranteRepositorio,
            productoRepositorio};

        // ====================================================
        // SERVICIO DE ESTADOS
        // ====================================================

        PedidoEstadoServicio pedidoEstadoServicio{
            pedidoRepositorio};

        // ====================================================
        // RESTAURANTE PRINCIPAL DEL PROTOTIPO
        // ====================================================

        constexpr std::int64_t RESTAURANTE_ID =
            1;

        // ====================================================
        // APLICACION GRAFICA
        // ====================================================

        AplicacionGui aplicacion{
            pedidoRepositorio,
            clienteRepositorio,
            productoRepositorio,
            pedidoServicio,
            facturaServicio,
            pedidoEstadoServicio,
            RESTAURANTE_ID};

        // ====================================================
        // EJECUTAR
        // ====================================================

        return aplicacion.ejecutar();
    }
    catch (
        const std::exception &error)
    {

        std::cerr
            << "\n========================================\n"
            << "ERROR AL INICIAR FOODFLOW\n"
            << "========================================\n"
            << "\n"
            << error.what()
            << "\n";

        return 1;
    }
}