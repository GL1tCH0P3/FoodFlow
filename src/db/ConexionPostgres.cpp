#include "db/ConexionPostgres.hpp"

namespace foodflow {

ConexionPostgres::ConexionPostgres(
    const ConfiguracionBD& configuracion
)
    : conexion(configuracion.cadenaConexion()) {
}


pqxx::connection& ConexionPostgres::obtener() {
    return conexion;
}


bool ConexionPostgres::estaActiva() const {
    return conexion.is_open();
}

}