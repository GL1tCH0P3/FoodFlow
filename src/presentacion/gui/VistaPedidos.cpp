#include "presentacion/gui/VistaPedidos.hpp"

#include <exception>

#include "imgui.h"

namespace foodflow
{

    VistaPedidos::VistaPedidos(
        PedidoRepositorio &pedidoRepositorio)
        : pedidoRepositorio(
              pedidoRepositorio),
          cargando(false)
    {

        cargarPedidos();
    }

    void VistaPedidos::cargarPedidos()
    {

        cargando = true;

        mensajeError.clear();

        try
        {

            pedidos =
                pedidoRepositorio.obtenerHistorico();
        }
        catch (
            const std::exception &error)
        {

            pedidos.clear();

            mensajeError =
                error.what();
        }

        cargando = false;
    }

    void VistaPedidos::recargar()
    {

        cargarPedidos();
    }

    std::optional<std::int64_t>
    VistaPedidos::renderizar()
    {

        std::optional<std::int64_t>
            pedidoSeleccionado;

        // ========================================================
        // ENCABEZADO
        // ========================================================

        ImGui::Text(
            "GESTION DE PEDIDOS");

        ImGui::Separator();

        ImGui::Spacing();

        ImGui::Text(
            "Historico registrado en PostgreSQL");

        ImGui::SameLine();

        if (
            ImGui::Button(
                "Actualizar"))
        {

            cargarPedidos();
        }

        ImGui::Spacing();

        // ========================================================
        // ESTADO DE CARGA
        // ========================================================

        if (cargando)
        {

            ImGui::TextDisabled(
                "Consultando pedidos...");

            return std::nullopt;
        }

        // ========================================================
        // ERROR
        // ========================================================

        if (!mensajeError.empty())
        {

            ImGui::TextColored(
                ImVec4(
                    1.0f,
                    0.4f,
                    0.4f,
                    1.0f),
                "No fue posible consultar PostgreSQL.");

            ImGui::TextWrapped(
                "%s",
                mensajeError.c_str());

            return std::nullopt;
        }

        // ========================================================
        // RESUMEN
        // ========================================================

        ImGui::Text(
            "Pedidos encontrados: %d",
            static_cast<int>(
                pedidos.size()));

        ImGui::Spacing();

        if (pedidos.empty())
        {

            ImGui::TextDisabled(
                "No existen pedidos registrados.");

            return std::nullopt;
        }

        // ========================================================
        // TABLA
        // ========================================================

        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingStretchProp;

        if (
            ImGui::BeginTable(
                "tabla_pedidos",
                10,
                flags,
                ImVec2(
                    0.0f,
                    0.0f)))
        {

            // ====================================================
            // COLUMNAS
            // ====================================================

            ImGui::TableSetupColumn(
                "ID",
                ImGuiTableColumnFlags_WidthFixed,
                55.0f);

            ImGui::TableSetupColumn(
                "Fecha");

            ImGui::TableSetupColumn(
                "Cliente");

            ImGui::TableSetupColumn(
                "Restaurante");

            ImGui::TableSetupColumn(
                "Pago");

            ImGui::TableSetupColumn(
                "Subtotal");

            ImGui::TableSetupColumn(
                "Domicilio");

            ImGui::TableSetupColumn(
                "Total");

            ImGui::TableSetupColumn(
                "Estado");

            ImGui::TableSetupColumn(
                "Acciones",
                ImGuiTableColumnFlags_WidthFixed,
                100.0f);

            ImGui::TableHeadersRow();

            // ====================================================
            // FILAS
            // ====================================================

            for (
                const auto &pedido :
                pedidos)
            {

                ImGui::TableNextRow();

                // =================================================
                // ID
                // =================================================

                ImGui::TableSetColumnIndex(0);

                ImGui::Text(
                    "%lld",
                    static_cast<long long>(
                        pedido.id));

                // =================================================
                // FECHA
                // =================================================

                ImGui::TableSetColumnIndex(1);

                ImGui::TextUnformatted(
                    pedido.fecha.c_str());

                // =================================================
                // CLIENTE
                // =================================================

                ImGui::TableSetColumnIndex(2);

                ImGui::TextUnformatted(
                    pedido.cliente.c_str());

                // =================================================
                // RESTAURANTE
                // =================================================

                ImGui::TableSetColumnIndex(3);

                ImGui::TextUnformatted(
                    pedido.restaurante.c_str());

                // =================================================
                // PAGO
                // =================================================

                ImGui::TableSetColumnIndex(4);

                ImGui::TextUnformatted(
                    pedido.metodoPago.c_str());

                // =================================================
                // SUBTOTAL
                // =================================================

                ImGui::TableSetColumnIndex(5);

                ImGui::Text(
                    "$ %.2f",
                    pedido.subtotal);

                // =================================================
                // DOMICILIO
                // =================================================

                ImGui::TableSetColumnIndex(6);

                ImGui::Text(
                    "$ %.2f",
                    pedido.costoDomicilio);

                // =================================================
                // TOTAL
                // =================================================

                ImGui::TableSetColumnIndex(7);

                ImGui::Text(
                    "$ %.2f",
                    pedido.total);

                // =================================================
                // ESTADO
                // =================================================

                ImGui::TableSetColumnIndex(8);

                if (
                    pedido.estado.empty())
                {

                    ImGui::TextDisabled(
                        "N/A");
                }
                else
                {

                    ImGui::TextUnformatted(
                        pedido.estado.c_str());
                }

                // =================================================
                // ACCIONES
                // =================================================

                ImGui::TableSetColumnIndex(9);

                ImGui::PushID(
                    static_cast<int>(
                        pedido.id));

                if (
                    ImGui::Button(
                        "Ver"))
                {

                    pedidoSeleccionado =
                        pedido.id;
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        return pedidoSeleccionado;
    }

}