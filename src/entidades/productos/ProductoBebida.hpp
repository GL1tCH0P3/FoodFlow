#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "entidades/productos/Producto.hpp"

namespace foodflow {

class ProductoBebida final : public Producto {
private:
    double recargo_;

public:
    ProductoBebida(
        std::int64_t id,
        std::int64_t restauranteId,
        std::string nombre,
        std::string descripcion,
        double precioBase,
        bool disponible,
        double recargo
    )
        : Producto(
              id,
              restauranteId,
              std::move(nombre),
              std::move(descripcion),
              precioBase,
              disponible
          ),
          recargo_(recargo) {
    }

    double getRecargo() const {
        return recargo_;
    }

    double calcularPrecioFinal() const override {
        return getPrecioBase() + recargo_;
    }

    std::string tipo() const override {
        return "BEBIDA";
    }
};

}