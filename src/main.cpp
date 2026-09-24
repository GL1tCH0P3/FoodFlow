#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "config/ConfiguracionBD.hpp"

#include "db/ConexionPostgres.hpp"

#include "presentacion/ConsolaPedido.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"
#include "repositorios/ProductoRepositorio.hpp"
#include "repositorios/RestauranteRepositorio.hpp"

#include "servicios/FacturaCsvServicio.hpp"
#include "servicios/PedidoServicio.hpp"

#include "validadores/PedidoValidador.hpp"


using namespace foodflow;


namespace {

int leerOpcionMenu() {

    int opcion;

    while (true) {

        std::cout
            << "\nSeleccione una opcion: ";

        if (std::cin >> opcion) {
            return opcion;
        }

        std::cout
            << "\nOpcion invalida. Intente nuevamente.\n";

        std::cin.clear();

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }
}


void mostrarResultadoPedido(
    const Pedido& pedido
) {

    std::cout
        << "\n========================================\n"
        << "          RESULTADO DEL PEDIDO\n"
        << "========================================\n\n";


    std::cout
        << "Pedido ID: "
        << pedido.id
        << "\n";


    std::cout
        << "Resultado: "
        << pedido.resultadoValidacion
        << "\n";


    std::cout
        << std::fixed
        << std::setprecision(2);


    std::cout
        << "Subtotal: $"
        << pedido.subtotal
        << "\n";


    std::cout
        << "Domicilio: $"
        << pedido.costoDomicilio
        << "\n";


    std::cout
        << "Total: $"
        << pedido.total
        << "\n";


    std::cout
        << "Tiempo estimado: "
        << pedido.tiempoEstimadoMin
        << " minutos\n";


    if (pedido.estado.has_value()) {

        std::cout
            << "Estado: "
            << pedido.estado.value()
            << "\n";
    }


    std::cout
        << "Mensaje: "
        << pedido.mensaje
        << "\n";
}


void mostrarMenu() {

    std::cout
        << "\n========================================\n"
        << "               FOODFLOW\n"
        << "========================================\n"
        << "\n"
        << "1. Registrar nuevo pedido\n"
        << "2. Ver historico de pedidos\n"
        << "0. Salir\n";
}

}


int main() {

    try {

        // ====================================================
        // CONFIGURACION Y CONEXION
        // ====================================================

        ConfiguracionBD configuracion =
            ConfiguracionBD::desdeEntorno();


        ConexionPostgres db{
            configuracion
        };


        std::cout
            << "\n========================================\n"
            << "               FOODFLOW\n"
            << "========================================\n"
            << "\nConexion con PostgreSQL establecida correctamente.\n";


        // ====================================================
        // REPOSITORIOS
        // ====================================================

        ClienteRepositorio clienteRepositorio{
            db
        };


        RestauranteRepositorio restauranteRepositorio{
            db
        };


        ProductoRepositorio productoRepositorio{
            db
        };


        PedidoRepositorio pedidoRepositorio{
            db
        };


        // ====================================================
        // VALIDADORES
        // ====================================================

        PedidoValidador validador;


        // ====================================================
        // SERVICIOS
        // ====================================================

        PedidoServicio pedidoServicio{
            clienteRepositorio,
            restauranteRepositorio,
            productoRepositorio,
            pedidoRepositorio,
            validador
        };


        FacturaCsvServicio facturaServicio{
            clienteRepositorio,
            restauranteRepositorio,
            productoRepositorio
        };


        // ====================================================
        // PRESENTACION
        // ====================================================

        ConsolaPedido consola{
            clienteRepositorio,
            productoRepositorio
        };


        // Por ahora FoodFlow trabaja con el restaurante
        // principal definido para el prototipo.
        constexpr std::int64_t RESTAURANTE_ID = 1;


        // ====================================================
        // CICLO PRINCIPAL
        // ====================================================

        bool ejecutando = true;


        while (ejecutando) {

            mostrarMenu();

            const int opcion =
                leerOpcionMenu();


            switch (opcion) {

                // ============================================
                // REGISTRAR PEDIDO
                // ============================================

                case 1: {

                    try {

                        SolicitudPedido solicitud =
                            consola.capturarSolicitud(
                                RESTAURANTE_ID
                            );


                        std::cout
                            << "\nProcesando pedido...\n";


                        Pedido pedido =
                            pedidoServicio.procesar(
                                solicitud
                            );


                        mostrarResultadoPedido(
                            pedido
                        );


                        // ------------------------------------
                        // Generacion de factura
                        // ------------------------------------

                        if (
                            pedido.resultadoValidacion
                            == "CONFIRMADO"
                        ) {

                            try {

                                const std::string rutaFactura =
                                    facturaServicio.generar(
                                        pedido
                                    );


                                std::cout
                                    << "\nFactura CSV generada correctamente:\n"
                                    << rutaFactura
                                    << "\n";
                            }
                            catch (
                                const std::exception& error
                            ) {

                                // La factura es una salida
                                // secundaria. El pedido ya fue
                                // persistido correctamente.

                                std::cerr
                                    << "\nEl pedido fue registrado, "
                                    << "pero no fue posible generar "
                                    << "la factura CSV:\n"
                                    << error.what()
                                    << "\n";
                            }
                        }


                        std::cout << "\n";
                    }
                    catch (
                        const std::exception& error
                    ) {

                        std::cerr
                            << "\nNo fue posible procesar el pedido:\n"
                            << error.what()
                            << "\n";
                    }

                    break;
                }


                // ============================================
                // HISTORICO
                // ============================================

                case 2: {

                    try {

                        consola.mostrarHistorico(
                            pedidoRepositorio
                        );
                    }
                    catch (
                        const std::exception& error
                    ) {

                        std::cerr
                            << "\nNo fue posible consultar "
                            << "el historico de pedidos:\n"
                            << error.what()
                            << "\n";
                    }

                    break;
                }


                // ============================================
                // SALIR
                // ============================================

                case 0: {

                    ejecutando = false;

                    std::cout
                        << "\nCerrando FoodFlow...\n";

                    break;
                }


                // ============================================
                // OPCION INVALIDA
                // ============================================

                default: {

                    std::cout
                        << "\nOpcion no valida. "
                        << "Seleccione 0, 1 o 2.\n";

                    break;
                }
            }
        }

    }
    catch (
        const std::exception& error
    ) {

        // Los errores que llegan hasta aqui son errores
        // generales de inicializacion, por ejemplo:
        //
        // - variables de entorno faltantes
        // - conexion PostgreSQL fallida
        // - configuracion invalida

        std::cerr
            << "\n========================================\n"
            << "ERROR AL INICIAR FOODFLOW\n"
            << "========================================\n"
            << "\n"
            << error.what()
            << "\n";

        return 1;
    }


    return 0;
}