#pragma once

#include "entidades/estados/EstadoPedido.hpp"

namespace foodflow {

class EstadoEntregado final : public EstadoPedido {
public:
    std::string codigo() const override {
        return "ENTREGADO";
    }


    std::string descripcionEstado() const override {
        return "El pedido fue entregado al cliente.";
    }


    bool esFinal() const override {
        return true;
    }


    std::string siguienteEstado() const override {
        return "ENTREGADO";
    }
};

}