#pragma once

#include <pqxx/pqxx>

#include "config/ConfiguracionBD.hpp"

namespace foodflow {

class ConexionPostgres {
private:
    pqxx::connection conexion;

public:
    explicit ConexionPostgres(
        const ConfiguracionBD& configuracion
    );

    pqxx::connection& obtener();

    bool estaActiva() const;
};

}