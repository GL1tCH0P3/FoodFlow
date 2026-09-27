#include "presentacion/gui/VistaDashboard.hpp"

#include <algorithm>
#include <exception>
#include <iomanip>
#include <sstream>
#include <string>

#include "imgui.h"

namespace foodflow
{

    VistaDashboard::VistaDashboard(
        PedidoRepositorio &pedidoRepositorio)
        : pedidoRepositorio(
              pedidoRepositorio),
          cargando(false)
    {

        cargarDatos();
    }

    void VistaDashboard::cargarDatos()
    {

        cargando =
            true;

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

        cargando =
            false;
    }

    void VistaDashboard::recargar()
    {

        cargarDatos();
    }

    void VistaDashboard::renderizarTarjeta(
        const char *titulo,
        const std::string &valor,
        const char *descripcion)
    {

        ImGui::BeginChild(
            titulo,
            ImVec2(
                0.0f,
                125.0f),
            true);

        ImGui::TextDisabled(
            "%s",
            titulo);

        ImGui::Spacing();

        ImGui::SetWindowFontScale(
            1.35f);

        ImGui::Text(
            "%s",
            valor.c_str());

        ImGui::SetWindowFontScale(
            1.0f);

        ImGui::Spacing();

        ImGui::TextDisabled(
            "%s",
            descripcion);

        ImGui::EndChild();
    }

    void VistaDashboard::renderizar()
    {

        // ========================================================
        // ENCABEZADO
        // ========================================================

        ImGui::Text(
            "DASHBOARD");

        ImGui::Separator();

        ImGui::Spacing();

        ImGui::TextDisabled(
            "Resumen operacional de FoodFlow");

        ImGui::SameLine();

        if (
            ImGui::Button(
                "Actualizar dashboard"))
        {

            cargarDatos();
        }

        ImGui::Spacing();

        ImGui::Spacing();

        // ========================================================
        // CARGA
        // ========================================================

        if (
            cargando)
        {

            ImGui::TextDisabled(
                "Consultando informacion...");

            return;
        }

        // ========================================================
        // ERROR
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
                "No fue posible cargar el dashboard.");

            ImGui::TextWrapped(
                "%s",
                mensajeError.c_str());

            return;
        }

        // ========================================================
        // METRICAS
        // ========================================================

        int confirmados =
            0;

        int recibidos =
            0;

        int preparando =
            0;

        int enCamino =
            0;

        int entregados =
            0;

        double ventas =
            0.0;

        for (
            const auto &pedido :
            pedidos)
        {

            if (
                pedido.resultadoValidacion == "CONFIRMADO")
            {

                ++confirmados;

                ventas +=
                    pedido.total;
            }

            if (
                pedido.estado == "RECIBIDO")
            {

                ++recibidos;
            }
            else if (
                pedido.estado == "PREPARANDO")
            {

                ++preparando;
            }
            else if (
                pedido.estado == "EN_CAMINO")
            {

                ++enCamino;
            }
            else if (
                pedido.estado == "ENTREGADO")
            {

                ++entregados;
            }
        }

        std::ostringstream ventasTexto;

        ventasTexto
            << "$ "
            << std::fixed
            << std::setprecision(2)
            << ventas;

        // ========================================================
        // TARJETAS
        // ========================================================

        if (
            ImGui::BeginTable(
                "metricas_dashboard",
                4,
                ImGuiTableFlags_SizingStretchSame))
        {

            // ----------------------------------------------------
            // TOTAL PEDIDOS
            // ----------------------------------------------------

            ImGui::TableNextColumn();

            renderizarTarjeta(
                "Pedidos procesados",
                std::to_string(
                    pedidos.size()),
                "Total registrado");

            // ----------------------------------------------------
            // CONFIRMADOS
            // ----------------------------------------------------

            ImGui::TableNextColumn();

            renderizarTarjeta(
                "Confirmados",
                std::to_string(
                    confirmados),
                "Pedidos aceptados");

            // ----------------------------------------------------
            // ENTREGADOS
            // ----------------------------------------------------

            ImGui::TableNextColumn();

            renderizarTarjeta(
                "Entregados",
                std::to_string(
                    entregados),
                "Ciclo completado");

            // ----------------------------------------------------
            // VENTAS
            // ----------------------------------------------------

            ImGui::TableNextColumn();

            renderizarTarjeta(
                "Ventas confirmadas",
                ventasTexto.str(),
                "Valor acumulado");

            ImGui::EndTable();
        }

        ImGui::Spacing();

        ImGui::Spacing();

        // ========================================================
        // ESTADOS
        // ========================================================

        ImGui::Text(
            "Estado operacional");

        ImGui::Separator();

        ImGui::Spacing();

        if (
            ImGui::BeginTable(
                "estados_dashboard",
                4,
                ImGuiTableFlags_Borders |
                    ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_SizingStretchSame))
        {

            ImGui::TableSetupColumn(
                "Recibidos");

            ImGui::TableSetupColumn(
                "Preparando");

            ImGui::TableSetupColumn(
                "En camino");

            ImGui::TableSetupColumn(
                "Entregados");

            ImGui::TableHeadersRow();

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(
                0);

            ImGui::Text(
                "%d",
                recibidos);

            ImGui::TableSetColumnIndex(
                1);

            ImGui::Text(
                "%d",
                preparando);

            ImGui::TableSetColumnIndex(
                2);

            ImGui::Text(
                "%d",
                enCamino);

            ImGui::TableSetColumnIndex(
                3);

            ImGui::Text(
                "%d",
                entregados);

            ImGui::EndTable();
        }

        ImGui::Spacing();

        ImGui::Spacing();

        // ========================================================
        // ULTIMOS PEDIDOS
        // ========================================================

        ImGui::Text(
            "Ultimos pedidos");

        ImGui::Separator();

        ImGui::Spacing();

        if (
            pedidos.empty())
        {

            ImGui::TextDisabled(
                "No existen pedidos registrados.");

            return;
        }

        constexpr ImGuiTableFlags flagsTabla =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp;

        if (
            ImGui::BeginTable(
                "ultimos_pedidos_dashboard",
                5,
                flagsTabla))
        {

            ImGui::TableSetupColumn(
                "ID",
                ImGuiTableColumnFlags_WidthFixed,
                60.0f);

            ImGui::TableSetupColumn(
                "Cliente");

            ImGui::TableSetupColumn(
                "Total");

            ImGui::TableSetupColumn(
                "Resultado");

            ImGui::TableSetupColumn(
                "Estado");

            ImGui::TableHeadersRow();

            const std::size_t limite =
                std::min<std::size_t>(
                    pedidos.size(),
                    8);

            for (
                std::size_t indice = 0;
                indice < limite;
                ++indice)
            {

                const PedidoResumen &pedido =
                    pedidos[indice];

                ImGui::TableNextRow();

                // =================================================
                // ID
                // =================================================

                ImGui::TableSetColumnIndex(
                    0);

                ImGui::Text(
                    "%lld",
                    static_cast<long long>(
                        pedido.id));

                // =================================================
                // CLIENTE
                // =================================================

                ImGui::TableSetColumnIndex(
                    1);

                ImGui::TextUnformatted(
                    pedido.cliente.c_str());

                // =================================================
                // TOTAL
                // =================================================

                ImGui::TableSetColumnIndex(
                    2);

                ImGui::Text(
                    "$ %.2f",
                    pedido.total);

                // =================================================
                // RESULTADO
                // =================================================

                ImGui::TableSetColumnIndex(
                    3);

                ImGui::TextUnformatted(
                    pedido.resultadoValidacion.c_str());

                // =================================================
                // ESTADO
                // =================================================

                ImGui::TableSetColumnIndex(
                    4);

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
            }

            ImGui::EndTable();
        }
    }

}