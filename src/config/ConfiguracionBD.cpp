#include "config/ConfiguracionBD.hpp"

#include <cstdlib>
#include <stdexcept>

namespace foodflow {

namespace {

std::string obtenerVariable(
    const char* nombre,
    bool requerida = true,
    const std::string& valorDefecto = ""
) {
    const char* valor = std::getenv(nombre);

    if (valor == nullptr || std::string(valor).empty()) {
        if (requerida) {
            throw std::runtime_error(
                std::string("Variable de entorno requerida no definida: ")
                + nombre
            );
        }

        return valorDefecto;
    }

    return valor;
}


std::string escaparValorConexion(const std::string& valor) {
    std::string resultado = "'";

    for (char caracter : valor) {
        if (caracter == '\\' || caracter == '\'') {
            resultado += '\\';
        }

        resultado += caracter;
    }

    resultado += "'";

    return resultado;
}

}


ConfiguracionBD ConfiguracionBD::desdeEntorno() {
    ConfiguracionBD config;

    config.host = obtenerVariable("FOODFLOW_DB_HOST");

    config.puerto = obtenerVariable("FOODFLOW_DB_PORT", false, "5432");

    config.nombreBD = obtenerVariable("FOODFLOW_DB_NAME");

    config.usuario = obtenerVariable("FOODFLOW_DB_USER");

    config.password = obtenerVariable("FOODFLOW_DB_PASSWORD");

    config.sslMode = obtenerVariable("FOODFLOW_DB_SSLMODE", false, "require");

    return config;
}


std::string ConfiguracionBD::cadenaConexion() const {
    return
        "host=" + escaparValorConexion(host) +
        " port=" + escaparValorConexion(puerto) +
        " dbname=" + escaparValorConexion(nombreBD) +
        " user=" + escaparValorConexion(usuario) +
        " password=" + escaparValorConexion(password) +
        " sslmode=" + escaparValorConexion(sslMode);
}

}