#pragma once

#include <string>
#include "utilidades/constantes.hpp"

std::string convTexto(TiposProducto tipo) {
    switch (tipo) {
        case TiposProducto::COMIDA: return "COMIDA";
        case TiposProducto::BEBIDA: return "BEBIDA";
        case TiposProducto::ESPECIAL: return "ESPECIAL";
        default: return "";
    }
}