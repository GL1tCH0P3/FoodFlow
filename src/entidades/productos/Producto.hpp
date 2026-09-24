#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace foodflow {

class Producto {
private:
    std::int64_t id_;
    std::int64_t restauranteId_;

    std::string nombre_;
    std::string descripcion_;

    double precioBase_;
    bool disponible_;

protected:
    Producto(
        std::int64_t id,
        std::int64_t restauranteId,
        std::string nombre,
        std::string descripcion,
        double precioBase,
        bool disponible
    )
        : id_(id),
          restauranteId_(restauranteId),
          nombre_(std::move(nombre)),
          descripcion_(std::move(descripcion)),
          precioBase_(precioBase),
          disponible_(disponible) {
    }

public:
    virtual ~Producto() = default;

    std::int64_t getId() const {
        return id_;
    }

    std::int64_t getRestauranteId() const {
        return restauranteId_;
    }

    const std::string& getNombre() const {
        return nombre_;
    }

    const std::string& getDescripcion() const {
        return descripcion_;
    }

    double getPrecioBase() const {
        return precioBase_;
    }

    bool estaDisponible() const {
        return disponible_;
    }

    virtual double calcularPrecioFinal() const = 0;

    virtual std::string tipo() const = 0;
};

}