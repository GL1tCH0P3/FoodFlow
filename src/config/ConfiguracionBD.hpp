#pragma once

#include <string>

namespace foodflow {

struct ConfiguracionBD {
    std::string host;
    std::string puerto;
    std::string nombreBD;
    std::string usuario;
    std::string password;
    std::string sslMode;

    static ConfiguracionBD desdeEntorno();

    std::string cadenaConexion() const;
};

}