#include <exception>
#include <iomanip>
#include <iostream>

#include "config/ConfiguracionBD.hpp"

#include "db/ConexionPostgres.hpp"

#include "presentacion/ConsolaPedido.hpp"

#include "repositorios/ClienteRepositorio.hpp"
#include "repositorios/RestauranteRepositorio.hpp"
#include "repositorios/ProductoRepositorio.hpp"
#include "repositorios/PedidoRepositorio.hpp"

#include "servicios/PedidoServicio.hpp"

#include "validadores/PedidoValidador.hpp"


using namespace foodflow;


int main() {

    try {

        std::cout
            << "\n========================================\n"
            << "               FOODFLOW\n"
            << "========================================\n";


        ConfiguracionBD configuracion = ConfiguracionBD::desdeEntorno();

        ConexionPostgres db{
            configuracion
        };

        std::cout<< "\nConexion con PostgreSQL establecida.\n";

        ClienteRepositorio clienteRepositorio{db};

        RestauranteRepositorio restauranteRepositorio{db};

        ProductoRepositorio productoRepositorio{db};

        PedidoRepositorio pedidoRepositorio{db};

        PedidoValidador validador;

        PedidoServicio pedidoServicio{
            clienteRepositorio,
            restauranteRepositorio,
            productoRepositorio,
            pedidoRepositorio,
            validador
        };

        ConsolaPedido consola{
            clienteRepositorio,
            productoRepositorio
        };

        // Por ahora FoodFlow trabaja con el
        // restaurante inicial del prototipo.
        constexpr std::int64_t RESTAURANTE_ID = 1;

        SolicitudPedido solicitud = consola.capturarSolicitud(RESTAURANTE_ID);

        std::cout<< "\nProcesando pedido...\n\n";

        Pedido pedido = pedidoServicio.procesar(solicitud);

        std::cout
            << "========================================\n"
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
            << "Subtotal: $"
            << std::fixed
            << std::setprecision(2)
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
            << "\n\n";

    }
    catch (const std::exception& error) {

        std::cerr
            << "\nError en FoodFlow:\n"
            << error.what()
            << "\n";

        return 1;
    }


    return 0;
}