#pragma once

#include "entidades/estados/EstadoPedido.hpp"

namespace foodflow {

class EstadoRecibido final : public EstadoPedido {
public:
    std::string codigo() const override {
        return "RECIBIDO";
    }


    std::string descripcionEstado() const override {
        return "El pedido fue recibido y esta pendiente de preparacion.";
    }


    bool esFinal() const override {
        return false;
    }


    std::string siguienteEstado() const override {
        return "PREPARANDO";
    }
};

}