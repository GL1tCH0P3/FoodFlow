#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

// Definimos una constante para el límite de distancia en kilómetros correspondiente a la zona de operación de un restaurante, en este caso, 15km.
const double LIMITE_DISTANCIA_KM = 15.0;

//Función para evaluar si el método de pago es válido, se escoge booleano porque solo hay dos opciones hasta ahora.
bool pagoValido(string metodoPago) {
    return metodoPago == "Efectivo" || metodoPago == "Transferencia";
}
// Función que evaúa si cada valor es valido y ejecuta un mensaje de confirmación, en este caso, si el metodo de pago, la distancia, la cantidad son validos.
bool decidirPedido(
    string metodoPago,
    double distanciaKm,
    vector<string> productos,
    vector<int> cantidades,
    vector<double> precios,
    double &total,
    string &mensaje
) {
    total = 0.0;

    if (productos.empty()) {
        mensaje = "Pedido rechazado: no hay productos.";
        return false;
    }

    if (productos.size() != cantidades.size() ||
        productos.size() != precios.size()) {
        mensaje = "Pedido rechazado: los datos de los productos no coinciden.";
        return false;
    }

    if (!pagoValido(metodoPago)) {
        mensaje = "Pedido rechazado: metodo de pago no disponible.";
        return false;
    }

    if (distanciaKm < 0.0) {
        mensaje = "Pedido rechazado: la distancia no puede ser negativa.";
        return false;
    }

    if (distanciaKm > LIMITE_DISTANCIA_KM) {
        mensaje = "Pedido rechazado: la distancia supera los 15 km de cobertura.";
        return false;
    }

    for (int i = 0; i < static_cast<int>(productos.size()); i++) {
        if (cantidades[i] <= 0) {
            mensaje = "Pedido rechazado: la cantidad debe ser mayor que cero.";
            return false;
        }

        if (precios[i] < 0.0) {
            mensaje = "Pedido rechazado: el precio no puede ser negativo.";
            return false;
        }

        total += cantidades[i] * precios[i];
    }

    mensaje = "Pedido confirmado.";
    return true;
}
//Función que reune los datos y los enseña como texto.
void mostrarResultado(
    int numeroCaso,
    string nombreCaso,
    bool resultado,
    double total,
    double distanciaKm,
    string mensaje
) {
    cout << "Caso " << numeroCaso << " - " << nombreCaso << "\n";
    cout << "Total: $" << fixed << setprecision(2) << total << "\n";
    cout << "Distancia: " << distanciaKm << " km\n";
    cout << "Decision: " << (resultado ? "ACEPTADO" : "RECHAZADO") << "\n";
    cout << "Mensaje: " << mensaje << "\n\n";
}

int main() {
    cout << "=== FoodFlow ===\n\n";

    // Caso 1: pedido valido.
    string pago1 = "Transferencia";
    double distancia1 = 7.5;
    vector<string> productos1 = {"Hamburguesa clasica", "Jugo natural"};
    vector<int> cantidades1 = {2, 1};
    vector<double> precios1 = {22000.0, 7000.0};
    double total1;
    string mensaje1;
//Añadimos la información a la función que agrupa los datos.
    bool resultado1 = decidirPedido(
        pago1, distancia1, productos1, cantidades1, precios1,
        total1, mensaje1
    );
//Pedimos que se enseñe la información contenida en resultado1.
    mostrarResultado(
        1, "Pedido valido", resultado1, total1, distancia1, mensaje1
    );

    // Caso 2: metodo de pago no permitido.
    string pago2 = "Tarjeta";
    double distancia2 = 5.0;
    vector<string> productos2 = {"Pizza personal"};
    vector<int> cantidades2 = {1};
    vector<double> precios2 = {25000.0};
    double total2;
    string mensaje2;

    bool resultado2 = decidirPedido(
        pago2, distancia2, productos2, cantidades2, precios2,
        total2, mensaje2
    );

    mostrarResultado(
        2, "Metodo de pago no permitido", resultado2,
        total2, distancia2, mensaje2
    );

    // Caso 3: frontera. Exactamente 15 km debe aceptarse.
    string pago3 = "Efectivo";
    double distancia3 = 15.0;
    vector<string> productos3 = {"Combo familiar"};
    vector<int> cantidades3 = {1};
    vector<double> precios3 = {48000.0};
    double total3;
    string mensaje3;

    bool resultado3 = decidirPedido(
        pago3, distancia3, productos3, cantidades3, precios3,
        total3, mensaje3
    );

    mostrarResultado(
        3, "Caso de frontera: 15 km", resultado3,
        total3, distancia3, mensaje3
    );
    // Caso 3.5: frontera. Superior a 15 km debe aceptarse.
    string pago4 = "Efectivo";
    double distancia4 = 15.1;
    vector<string> productos4 = {"Combo familiar"};
    vector<int> cantidades4 = {1};
    vector<double> precios4 = {48000.0};
    double total4;
    string mensaje4;

    bool resultado4 = decidirPedido(
        pago4, distancia4, productos4, cantidades4, precios4,
        total4, mensaje4
    );

    mostrarResultado(
        4, "Caso de frontera: 15.1 km", resultado4,
        total4, distancia4, mensaje4
    );
    return 0;
}
