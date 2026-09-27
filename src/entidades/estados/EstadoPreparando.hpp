#pragma once

#include "entidades/estados/EstadoPedido.hpp"

namespace foodflow {

class EstadoPreparando final : public EstadoPedido {
public:
    std::string codigo() const override {
        return "PREPARANDO";
    }


    std::string descripcionEstado() const override {
        return "El restaurante se encuentra preparando el pedido.";
    }


    bool esFinal() const override {
        return false;
    }


    std::string siguienteEstado() const override {
        return "EN_CAMINO";
    }
};

}