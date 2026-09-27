#include "presentacion/gui/VistaNuevoPedido.hpp"

#include <exception>
#include <string>

#include "imgui.h"

namespace foodflow
{

    VistaNuevoPedido::VistaNuevoPedido(
        ClienteRepositorio &clienteRepositorio,
        IProductoRepositorio &productoRepositorio,
        PedidoServicio &pedidoServicio,
        FacturaCsvServicio &facturaServicio,
        std::int64_t restauranteId)
        : clienteRepositorio(
              clienteRepositorio),
          productoRepositorio(
              productoRepositorio),
          pedidoServicio(
              pedidoServicio),
          facturaServicio(
              facturaServicio),
          restauranteId(
              restauranteId),
          clienteSeleccionado(-1),
          metodoPagoSeleccionado(0),
          distanciaKm(0.0f),
          procesando(false)
    {

        cargarDatos();
    }

    void VistaNuevoPedido::cargarDatos()
    {

        mensajeError.clear();

        clientes.clear();

        productos.clear();

        try
        {

            // ====================================================
            // CLIENTES
            // ====================================================

            const auto clientesDb =
                clienteRepositorio.obtenerTodos();

            for (
                const auto &cliente :
                clientesDb)
            {

                clientes.push_back({cliente.id,
                                    cliente.nombre,
                                    cliente.telefono,
                                    cliente.direccion});
            }

            // ====================================================
            // PRODUCTOS
            // ====================================================

            auto productosDb =
                productoRepositorio.obtenerDisponibles(
                    restauranteId);

            for (
                const auto &producto :
                productosDb)
            {

                if (!producto)
                {
                    continue;
                }

                productos.push_back({producto->getId(),
                                     producto->getNombre(),
                                     producto->tipo(),
                                     producto->calcularPrecioFinal(),
                                     0});
            }
        }
        catch (
            const std::exception &error)
        {

            mensajeError =
                error.what();
        }
    }

    void VistaNuevoPedido::limpiarFormulario()
    {

        clienteSeleccionado =
            -1;

        metodoPagoSeleccionado =
            0;

        distanciaKm =
            0.0f;

        for (
            auto &producto :
            productos)
        {

            producto.cantidad =
                0;
        }
    }

    void VistaNuevoPedido::procesarPedido()
    {

        mensajeError.clear();

        mensajeExito.clear();

        rutaFactura.clear();

        // ========================================================
        // CLIENTE
        // ========================================================

        if (
            clienteSeleccionado < 0 ||
            clienteSeleccionado >=
                static_cast<int>(
                    clientes.size()))
        {

            mensajeError =
                "Seleccione un cliente.";

            return;
        }

        // ========================================================
        // METODO DE PAGO
        // ========================================================

        if (
            metodoPagoSeleccionado == 0)
        {

            mensajeError =
                "Seleccione un metodo de pago.";

            return;
        }

        // ========================================================
        // PRODUCTOS
        // ========================================================

        bool tieneProductos =
            false;

        for (
            const auto &producto :
            productos)
        {

            if (
                producto.cantidad > 0)
            {

                tieneProductos =
                    true;

                break;
            }
        }

        if (
            !tieneProductos)
        {

            mensajeError =
                "Seleccione al menos un producto.";

            return;
        }

        // ========================================================
        // SOLICITUD
        // ========================================================

        SolicitudPedido solicitud;

        const ClienteOpcion &cliente =
            clientes[static_cast<std::size_t>(
                clienteSeleccionado)];

        solicitud.clienteId =
            cliente.id;

        solicitud.restauranteId =
            restauranteId;

        solicitud.direccionEntrega =
            cliente.direccion;

        solicitud.distanciaKm =
            static_cast<double>(
                distanciaKm);

        if (
            metodoPagoSeleccionado == 1)
        {

            solicitud.metodoPago =
                "Efectivo";
        }
        else
        {

            solicitud.metodoPago =
                "Transferencia";
        }

        for (
            const auto &producto :
            productos)
        {

            if (
                producto.cantidad <= 0)
            {

                continue;
            }

            solicitud.items.push_back({producto.id,
                                       producto.cantidad});
        }

        // ========================================================
        // PROCESAMIENTO
        // ========================================================

        procesando =
            true;

        try
        {

            Pedido pedido =
                pedidoServicio.procesar(
                    solicitud);

            if (
                pedido.resultadoValidacion == "CONFIRMADO")
            {

                mensajeExito =
                    "Pedido #" + std::to_string(pedido.id) + " confirmado correctamente.";

                // ================================================
                // FACTURA
                // ================================================

                try
                {

                    rutaFactura =
                        facturaServicio.generar(
                            pedido);
                }
                catch (
                    const std::exception &error)
                {

                    rutaFactura =
                        std::string(
                            "Pedido confirmado, pero la factura "
                            "no pudo generarse: ") +
                        error.what();
                }

                limpiarFormulario();
            }
            else
            {

                mensajeError =
                    pedido.resultadoValidacion + ": " + pedido.mensaje;
            }
        }
        catch (
            const std::exception &error)
        {

            mensajeError =
                error.what();
        }

        procesando =
            false;
    }

    void VistaNuevoPedido::renderizarCliente()
    {

        ImGui::Text(
            "Cliente");

        ImGui::SetNextItemWidth(
            420.0f);

        const char *textoActual =
            "Seleccione un cliente";

        if (
            clienteSeleccionado >= 0 &&
            clienteSeleccionado <
                static_cast<int>(
                    clientes.size()))
        {

            textoActual =
                clientes[static_cast<std::size_t>(
                             clienteSeleccionado)]
                    .nombre
                    .c_str();
        }

        if (
            ImGui::BeginCombo(
                "##cliente",
                textoActual))
        {

            for (
                int indice = 0;
                indice <
                static_cast<int>(
                    clientes.size());
                ++indice)
            {

                const bool seleccionado =
                    clienteSeleccionado == indice;

                if (
                    ImGui::Selectable(
                        clientes[static_cast<std::size_t>(
                                     indice)]
                            .nombre
                            .c_str(),
                        seleccionado))
                {

                    clienteSeleccionado =
                        indice;
                }

                if (
                    seleccionado)
                {

                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        if (
            clienteSeleccionado >= 0 &&
            clienteSeleccionado <
                static_cast<int>(
                    clientes.size()))
        {

            const auto &cliente =
                clientes[static_cast<std::size_t>(
                    clienteSeleccionado)];

            ImGui::Spacing();

            ImGui::TextDisabled(
                "Telefono: %s",
                cliente.telefono.c_str());

            ImGui::TextDisabled(
                "Direccion: %s",
                cliente.direccion.c_str());
        }
    }

    void VistaNuevoPedido::renderizarProductos()
    {

        ImGui::Text(
            "Productos");

        ImGui::Spacing();

        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp;

        if (
            ImGui::BeginTable(
                "productos_nuevo_pedido",
                5,
                flags))
        {

            ImGui::TableSetupColumn(
                "ID",
                ImGuiTableColumnFlags_WidthFixed,
                55.0f);

            ImGui::TableSetupColumn(
                "Producto");

            ImGui::TableSetupColumn(
                "Tipo");

            ImGui::TableSetupColumn(
                "Precio");

            // Suficiente espacio para:
            // [-]   0   [+]

            ImGui::TableSetupColumn(
                "Cantidad",
                ImGuiTableColumnFlags_WidthFixed,
                145.0f);

            ImGui::TableHeadersRow();

            for (
                auto &producto :
                productos)
            {

                ImGui::TableNextRow();

                // =================================================
                // ID
                // =================================================

                ImGui::TableSetColumnIndex(0);

                ImGui::Text(
                    "%lld",
                    static_cast<long long>(
                        producto.id));

                // =================================================
                // PRODUCTO
                // =================================================

                ImGui::TableSetColumnIndex(1);

                ImGui::TextUnformatted(
                    producto.nombre.c_str());

                // =================================================
                // TIPO
                // =================================================

                ImGui::TableSetColumnIndex(2);

                ImGui::TextUnformatted(
                    producto.tipo.c_str());

                // =================================================
                // PRECIO
                // =================================================

                ImGui::TableSetColumnIndex(3);

                ImGui::Text(
                    "$ %.2f",
                    producto.precio);

                // =================================================
                // CANTIDAD
                // =================================================

                ImGui::TableSetColumnIndex(4);

                ImGui::PushID(
                    static_cast<int>(
                        producto.id));

                if (
                    ImGui::SmallButton(
                        "-"))
                {

                    if (
                        producto.cantidad > 0)
                    {

                        --producto.cantidad;
                    }
                }

                ImGui::SameLine(
                    0.0f,
                    10.0f);

                ImGui::Text(
                    "%d",
                    producto.cantidad);

                ImGui::SameLine(
                    0.0f,
                    10.0f);

                if (
                    ImGui::SmallButton(
                        "+"))
                {

                    ++producto.cantidad;
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        // ========================================================
        // SUBTOTAL ESTIMADO
        // ========================================================

        double subtotal =
            0.0;

        for (
            const auto &producto :
            productos)
        {

            subtotal +=
                producto.precio * producto.cantidad;
        }

        ImGui::Spacing();

        ImGui::Text(
            "Subtotal estimado: $ %.2f",
            subtotal);
    }

    void VistaNuevoPedido::renderizarDatosEntrega()
    {

        ImGui::Text(
            "Datos de entrega");

        ImGui::Spacing();

        const char *metodosPago[] = {

            "Seleccione...",

            "Efectivo",

            "Transferencia"};

        ImGui::SetNextItemWidth(
            250.0f);

        ImGui::Combo(
            "Metodo de pago",
            &metodoPagoSeleccionado,
            metodosPago,
            3);

        ImGui::Spacing();

        ImGui::SetNextItemWidth(
            180.0f);

        ImGui::InputFloat(
            "Distancia (km)",
            &distanciaKm,
            0.5f,
            1.0f,
            "%.2f");
    }

    void VistaNuevoPedido::renderizarResultado()
    {

        if (
            !mensajeExito.empty())
        {

            ImGui::Spacing();

            ImGui::TextColored(
                ImVec4(
                    0.35f,
                    0.90f,
                    0.45f,
                    1.0f),
                "%s",
                mensajeExito.c_str());

            if (
                !rutaFactura.empty())
            {

                ImGui::TextWrapped(
                    "Factura: %s",
                    rutaFactura.c_str());
            }
        }

        if (
            !mensajeError.empty())
        {

            ImGui::Spacing();

            ImGui::TextColored(
                ImVec4(
                    1.0f,
                    0.40f,
                    0.40f,
                    1.0f),
                "%s",
                mensajeError.c_str());
        }
    }

    void VistaNuevoPedido::renderizar()
    {

        ImGui::Text(
            "NUEVO PEDIDO");

        ImGui::Separator();

        ImGui::Spacing();

        if (
            ImGui::Button(
                "Recargar datos"))
        {

            cargarDatos();
        }

        ImGui::Spacing();

        ImGui::Spacing();

        renderizarCliente();

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        renderizarProductos();

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        renderizarDatosEntrega();

        ImGui::Spacing();

        ImGui::Spacing();

        if (
            procesando)
        {

            ImGui::TextDisabled(
                "Procesando pedido...");
        }
        else
        {

            if (
                ImGui::Button(
                    "Confirmar pedido",
                    ImVec2(
                        200.0f,
                        42.0f)))
            {

                procesarPedido();
            }
        }

        renderizarResultado();
    }

}