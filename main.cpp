#include "imagen.h"
#include "buddy_allocator.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <sys/resource.h>  
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Muestra cómo se usa el programa desde la línea de comandos
void mostrarUso() {
    cout << "Uso: ./programa_imagen <archivo_entrada> <archivo_salida> <-angulo> <45> <-factor> <1.5> <-buddy|-no-buddy>" << endl;
    cout << "  <archivo_entrada>   Archivo de imagen de entrada (PNG, BMP, JPG)" << endl;
    cout << "  <archivo_salida>    Archivo de salida para la imagen procesada" << endl;
    cout << "  -angulo             Ángulo de rotación (en grados)" << endl;
    cout << "  -factor             Factor de escalado (ej. 1.5 para aumentar un 50%)" << endl;
    cout << "  -buddy              Usa Buddy System para la asignación de memoria" << endl;
    cout << "  -no-buddy           Usa new/delete para la asignación de memoria" << endl;
}

// Muestra una lista de chequeo para verificar que los parámetros son correctos
void mostrarListaChequeo(const string &archivoEntrada, const string &archivoSalida, bool usarBuddy) {
    cout << "\n=== PROCESAMIENTO DE  IMAGEN ===" << endl;
    cout << "Archivo de entrada: " << archivoEntrada << endl;
    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "Modo de asignación de memoria: " << (usarBuddy ? "Buddy System" : "new/delete") << endl;
    cout << "------------------------" << endl;
}
void mostrarMemoriaUtilizada() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    // Convertir KB a MB con precisión de un decimal
    double memoria_mb = usage.ru_maxrss / 1024.0;
    
    cout << "\nMEMORIA UTILIZADA:" << endl;
    cout << " : " << fixed << setprecision(1) << memoria_mb << " MB" << endl;
}

int main(int argc, char* argv[]) {
    // Verificar número de argumentos
    if (argc != 8) {
        cerr << "Uso incorrecto" << endl;
        mostrarUso();
        return 1;
    }

    // Parámetros de línea de comandos
    string archivoEntrada = argv[1];
    string archivoSalida = argv[2];
    
    
    double angulo = std::stod(argv[4]);
    double factor = std::stod(argv[6]);
    string modoAsignacion = argv[7];

    // Verifica si el modo de asignación es válido
    bool usarBuddy = false;
    if (modoAsignacion == "-buddy") {
        usarBuddy = true;
    } else if (modoAsignacion == "-no-buddy") {
        usarBuddy = false;
    } else {
        cerr << "Error: Opción de modo inválida." << endl;
        mostrarUso();
        return 1;
    }

    // Mostrar lista de chequeo
    mostrarListaChequeo(archivoEntrada, archivoSalida, usarBuddy);
    
    // Medir memoria inicial
    double memoria_inicial = 0.0;
    {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        memoria_inicial = usage.ru_maxrss / 1024.0;  // Convertir KB a MB
    }

    // Medir el tiempo de ejecución
    auto inicio = high_resolution_clock::now();

    if (usarBuddy) {
        // Crear el allocador Buddy System de 256 MB
        BuddyAllocator allocador(256 * 1024 * 1024);

        // Cargar imagen usando Buddy System
        Imagen img(archivoEntrada, &allocador);
        Imagen img2(archivoEntrada, &allocador);
        
         // Mostrar información de la imagen y procesarla, ya sea rotarla o escalarla
         img.mostrarInfo(false);
         img.escalar(factor);
         // Guardar imagen procesada
         img.guardarImagen("escalada_"+archivoSalida);
 
 
         img2.mostrarInfo(true);
         img2.rotar(angulo);
         // Guardar imagen procesada
         img2.guardarImagen("rotada_"+archivoSalida);
    } else {

        // Cargar imagen usando new/delete
        Imagen img(archivoEntrada);
        Imagen img2(archivoEntrada);
        
        // Mostrar información de la imagen y procesarla, ya sea rotarla o escalarla
        img.mostrarInfo(false);
        img.escalar(factor);
        // Guardar imagen procesada
        img.guardarImagen("escalada_"+archivoSalida);


        img2.mostrarInfo(true);
        img2.rotar(angulo);
        // Guardar imagen procesada
        img2.guardarImagen("rotada_"+archivoSalida);
    }

    // Medir tiempo de finalización
    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio).count();
    cout << "\nTiempo total de procesamiento: " << duracion << " ms" 
         << " (" << (usarBuddy ? "usando buddy" : "usando new-delete") << ")" << endl;
    
    // Mostrar memoria utilizada
    mostrarMemoriaUtilizada();

    cout << "\n[INFO] Proceso completado con éxito." << endl;

    return 0;
}