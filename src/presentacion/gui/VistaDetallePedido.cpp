#include "presentacion/gui/VistaDetallePedido.hpp"

#include <cstdint>
#include <exception>
#include <string>

#include "imgui.h"

namespace foodflow
{

    VistaDetallePedido::VistaDetallePedido(
        PedidoRepositorio &pedidoRepositorio,
        PedidoEstadoServicio &pedidoEstadoServicio,
        FacturaCsvServicio &facturaServicio)
        : pedidoRepositorio(
              pedidoRepositorio),
          pedidoEstadoServicio(
              pedidoEstadoServicio),
          facturaServicio(
              facturaServicio)
    {
    }

    void VistaDetallePedido::abrir(
        std::int64_t pedidoId)
    {

        mensajeError.clear();

        mensajeExito.clear();

        rutaFactura.clear();

        cargar(
            pedidoId);
    }

    void VistaDetallePedido::cargar(
        std::int64_t pedidoId)
    {

        mensajeError.clear();

        try
        {

            pedido =
                pedidoRepositorio.buscarDetallePorId(
                    pedidoId);

            if (
                !pedido.has_value())
            {

                mensajeError =
                    "El pedido seleccionado no existe.";
            }
        }
        catch (
            const std::exception &error)
        {

            pedido.reset();

            mensajeError =
                error.what();
        }
    }

    bool VistaDetallePedido::renderizar()
    {

        bool volver =
            false;

        // ========================================================
        // NAVEGACION
        // ========================================================

        if (
            ImGui::Button(
                "< Volver a pedidos"))
        {

            volver =
                true;
        }

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        // ========================================================
        // ERRORES
        // ========================================================

        if (
            !mensajeError.empty())
        {

            ImGui::TextColored(
                ImVec4(
                    1.0f,
                    0.40f,
                    0.40f,
                    1.0f),
                "%s",
                mensajeError.c_str());

            return volver;
        }

        // ========================================================
        // PEDIDO NO CARGADO
        // ========================================================

        if (
            !pedido.has_value())
        {

            ImGui::TextDisabled(
                "No existe un pedido seleccionado.");

            return volver;
        }

        PedidoDetalleVista &detalle =
            pedido.value();

        // ========================================================
        // ENCABEZADO
        // ========================================================

        ImGui::Text(
            "PEDIDO #%lld",
            static_cast<long long>(
                detalle.id));

        ImGui::SameLine();

        ImGui::TextDisabled(
            "| %s",
            detalle.fecha.c_str());

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        // ========================================================
        // DATOS GENERALES
        // ========================================================

        ImGui::Text(
            "Cliente: %s",
            detalle.cliente.c_str());

        ImGui::Text(
            "Restaurante: %s",
            detalle.restaurante.c_str());

        ImGui::Text(
            "Direccion de entrega: %s",
            detalle.direccionEntrega.c_str());

        ImGui::Text(
            "Metodo de pago: %s",
            detalle.metodoPago.c_str());

        ImGui::Text(
            "Resultado de validacion: %s",
            detalle.resultadoValidacion.c_str());

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        // ========================================================
        // PRODUCTOS
        // ========================================================

        ImGui::Text(
            "Productos del pedido");

        ImGui::Spacing();

        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp;

        if (
            ImGui::BeginTable(
                "detalle_productos",
                4,
                flags))
        {

            ImGui::TableSetupColumn(
                "Producto");

            ImGui::TableSetupColumn(
                "Cantidad",
                ImGuiTableColumnFlags_WidthFixed,
                100.0f);

            ImGui::TableSetupColumn(
                "Precio unitario");

            ImGui::TableSetupColumn(
                "Subtotal");

            ImGui::TableHeadersRow();

            for (
                const auto &producto :
                detalle.productos)
            {

                ImGui::TableNextRow();

                // =================================================
                // PRODUCTO
                // =================================================

                ImGui::TableSetColumnIndex(
                    0);

                ImGui::TextUnformatted(
                    producto.nombre.c_str());

                // =================================================
                // CANTIDAD
                // =================================================

                ImGui::TableSetColumnIndex(
                    1);

                ImGui::Text(
                    "%d",
                    producto.cantidad);

                // =================================================
                // PRECIO UNITARIO
                // =================================================

                ImGui::TableSetColumnIndex(
                    2);

                ImGui::Text(
                    "$ %.2f",
                    producto.precioUnitario);

                // =================================================
                // SUBTOTAL
                // =================================================

                ImGui::TableSetColumnIndex(
                    3);

                ImGui::Text(
                    "$ %.2f",
                    producto.subtotal);
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        // ========================================================
        // TOTALES
        // ========================================================

        ImGui::Text(
            "Subtotal: $ %.2f",
            detalle.subtotal);

        ImGui::Text(
            "Costo domicilio: $ %.2f",
            detalle.costoDomicilio);

        ImGui::Text(
            "TOTAL: $ %.2f",
            detalle.total);

        ImGui::Spacing();

        ImGui::Separator();

        ImGui::Spacing();

        // ========================================================
        // FACTURA
        // ========================================================

        if (
            detalle.resultadoValidacion == "CONFIRMADO")
        {

            ImGui::Text(
                "Factura");

            ImGui::Spacing();

            if (
                ImGui::Button(
                    "Exportar factura CSV",
                    ImVec2(
                        220.0f,
                        44.0f)))
            {

                try
                {

                    rutaFactura =
                        facturaServicio.generar(
                            detalle);

                    mensajeExito =
                        "Factura exportada correctamente.";

                    mensajeError.clear();
                }
                catch (
                    const std::exception &error)
                {

                    mensajeError =
                        error.what();

                    mensajeExito.clear();

                    rutaFactura.clear();
                }
            }

            if (
                !rutaFactura.empty())
            {

                ImGui::Spacing();

                ImGui::TextDisabled(
                    "Archivo generado:");

                ImGui::TextWrapped(
                    "%s",
                    rutaFactura.c_str());
            }

            ImGui::Spacing();

            ImGui::Separator();

            ImGui::Spacing();
        }

        // ========================================================
        // CICLO DE VIDA
        // ========================================================

        ImGui::Text(
            "Estado del pedido");

        ImGui::Spacing();

        // Solo un pedido confirmado puede ingresar al
        // ciclo de vida operativo.

        if (
            detalle.resultadoValidacion != "CONFIRMADO")
        {

            ImGui::TextDisabled(
                "Este pedido no fue confirmado y no posee "
                "un ciclo de preparacion.");

            return volver;
        }

        if (
            detalle.estado.empty())
        {

            ImGui::TextDisabled(
                "El pedido no tiene un estado registrado.");

            return volver;
        }

        // ========================================================
        // ESTADO ACTUAL
        // ========================================================

        ImGui::Text(
            "Estado actual: %s",
            detalle.estado.c_str());

        try
        {

            const std::string descripcion =
                pedidoEstadoServicio.descripcion(
                    detalle.estado);

            ImGui::TextWrapped(
                "%s",
                descripcion.c_str());

            ImGui::Spacing();

            // ====================================================
            // TRANSICION DE ESTADO
            // ====================================================

            if (
                pedidoEstadoServicio.puedeAvanzar(
                    detalle.estado))
            {

                const std::string siguiente =
                    pedidoEstadoServicio.siguienteEstado(
                        detalle.estado);

                const std::string textoBoton =
                    "Avanzar a " + siguiente;

                if (
                    ImGui::Button(
                        textoBoton.c_str(),
                        ImVec2(
                            230.0f,
                            44.0f)))
                {

                    const std::int64_t pedidoId =
                        detalle.id;

                    const std::string estadoActual =
                        detalle.estado;

                    try
                    {

                        const std::string nuevoEstado =
                            pedidoEstadoServicio.avanzar(
                                pedidoId,
                                estadoActual);

                        mensajeExito =
                            "Estado actualizado correctamente a " + nuevoEstado + ".";

                        mensajeError.clear();

                        // ========================================
                        // RECARGAR DESDE POSTGRESQL
                        //
                        // No asumimos que el UPDATE funciono.
                        // Volvemos a consultar la fuente real.
                        // ========================================

                        cargar(
                            pedidoId);
                    }
                    catch (
                        const std::exception &error)
                    {

                        mensajeError =
                            error.what();

                        mensajeExito.clear();
                    }
                }
            }
            else
            {

                ImGui::TextColored(
                    ImVec4(
                        0.35f,
                        0.90f,
                        0.45f,
                        1.0f),
                    "Pedido completado.");
            }
        }
        catch (
            const std::exception &error)
        {

            mensajeError =
                error.what();
        }

        // ========================================================
        // MENSAJE DE EXITO
        // ========================================================

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
        }

        // ========================================================
        // MENSAJE DE ERROR
        // ========================================================

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

        return volver;
    }

}