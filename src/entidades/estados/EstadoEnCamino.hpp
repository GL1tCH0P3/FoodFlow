#pragma once

#include "entidades/estados/EstadoPedido.hpp"

namespace foodflow {

class EstadoEnCamino final : public EstadoPedido {
public:
    std::string codigo() const override {
        return "EN_CAMINO";
    }


    std::string descripcionEstado() const override {
        return "El pedido salio del restaurante y se encuentra en camino.";
    }


    bool esFinal() const override {
        return false;
    }


    std::string siguienteEstado() const override {
        return "ENTREGADO";
    }
};

}