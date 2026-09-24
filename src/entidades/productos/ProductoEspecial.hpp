#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "entidades/productos/ProductoComida.hpp"

namespace foodflow {

class ProductoEspecial final : public ProductoComida {
private:
    double recargoEspecial_;

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
        : ProductoComida(
              id,
              restauranteId,
              std::move(nombre),
              std::move(descripcion),
              precioBase,
              disponible,
              recargo
          ),
          recargoEspecial_(recargoEspecial) {
    }

    double getRecargoEspecial() const {
        return recargoEspecial_;
    }

    double calcularPrecioFinal() const override {
        return ProductoComida::calcularPrecioFinal()
             + recargoEspecial_;
    }

    std::string tipo() const override {
        return "ESPECIAL";
    }
};

}