#pragma once

#include "entidades/productos/ProductoComida.hpp"

namespace foodflow {

class ProductoEspecial : public ProductoComida {
private:
    double recargoEspecial;

public:
    ProductoEspecial(
        std::int64_t id,
        std::int64_t restauranteId,
        std::string nombre,
        std::string descripcion,
        double precioBase,
        bool disponible,
        double recargo,
        double recargoEspecial
    )
        : ProductoComida(id, restauranteId, std::move(nombre), std::move(descripcion), precioBase, disponible, recargo),
          recargoEspecial(recargoEspecial) {

    }

    double calcularPrecioFinal() const override {
        return ProductoComida::calcularPrecioFinal() + recargoEspecial;
    }

    std::string tipo() const override {
        return "ESPECIAL";
    }
};

}