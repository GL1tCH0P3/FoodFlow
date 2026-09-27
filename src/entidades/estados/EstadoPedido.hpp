#pragma once

#include <string>

namespace foodflow {

class EstadoPedido {
public:
    virtual ~EstadoPedido() = default;

    virtual std::string codigo() const = 0;

    virtual std::string descripcionEstado() const = 0;

    virtual bool esFinal() const = 0;

    virtual std::string siguienteEstado() const = 0;
};

}