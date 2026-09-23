#pragma once

#include <cstdint>
#include <string>

#include "entidades/productos/ProductoComida.hpp"
#include "entidades/productos/ProductoBebida.hpp"
#include "entidades/productos/ProductoEspecial.hpp"

namespace foodflow {

class Producto {
private:
    std::int64_t id;
    std::int64_t restauranteId;
    std::string nombre;
    std::string descripcion;

    double precioBase;
    bool disponible;

public:
    Producto(
        std::int64_t id,
        std::int64_t restauranteId,
        std::string nombre,
        std::string descripcion,
        double precioBase,
        bool disponible
    )
        : id(id),
        restauranteId(restauranteId),
        nombre(nombre),
        descripcion(descripcion),
        precioBase(precioBase),
        disponible(disponible) {

    }

    virtual ~Producto() = default;

    std::int64_t getId() const {
        return id;
    }

    std::int64_t getRestauranteId() const {
        return restauranteId;
    }

    const std::string& getNombre() const {
        return nombre;
    }

    const std::string& getDescripcion() const {
        return descripcion;
    }

    double getPrecioBase() const {
        return precioBase;
    }

    bool estaDisponible() const {
        return disponible;
    }

    virtual double calcularPrecioFinal() const = 0;

    virtual std::string tipo() const = 0;
};

}