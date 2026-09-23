#pragma once

#include "entidades/productos/Producto.hpp"

namespace foodflow {

class ProductoComida : public Producto {
private:
    double recargo;

public:
    ProductoComida(
        std::int64_t id,
        std::int64_t restauranteId,
        std::string nombre,
        std::string descripcion,
        double precioBase,
        bool disponible,
        double recargo
    )
        : Producto(id, restauranteId, std::move(nombre), std::move(descripcion), precioBase, disponible),
          recargo(recargo) {

    }

    double calcularPrecioFinal() const override {
        return getPrecioBase() + recargo;
    }

    std::string tipo() const override {
        return "COMIDA";
    }
};

}