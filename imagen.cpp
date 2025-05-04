#include "imagen.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <cmath>


using namespace std;

Imagen::Imagen(const std::string &nombreArchivo, BuddyAllocator *allocador)
    : allocador(allocador) {
    unsigned char* buffer = stbi_load(nombreArchivo.c_str(), &ancho, &alto, &canales, 0);
    if (!buffer) {
        cerr << "Error: No se pudo cargar la imagen '" << nombreArchivo << "'.\n";
        exit(1);
    }
    convertirBufferAMatriz(buffer);
    stbi_image_free(buffer);
}

Imagen::~Imagen() {
    if (pixeles)
        liberarImagen(pixeles, alto, ancho);
}

void Imagen::convertirBufferAMatriz(unsigned char* buffer) {
    int indice = 0;
    pixeles = new unsigned char**[alto];
    for (int y = 0; y < alto; y++) {
        pixeles[y] = new unsigned char*[ancho];
        for (int x = 0; x < ancho; x++) {
            pixeles[y][x] = allocador ? (unsigned char*) allocador->alloc(canales) : new unsigned char[canales];
            if (!pixeles[y][x]) {
                cerr << "Error: Fallo en la asignación de memoria para píxeles[" << y << "][" << x << "]" << endl;
                exit(1);
            }
            for (int c = 0; c < canales; c++) {
                pixeles[y][x][c] = buffer[indice++];
            }
        }
    }
}

void Imagen::mostrarInfo(bool esrotada) const {
    if (esrotada) {
        cout << "[INFO] Imagen rotada." << endl;
    } else {
        cout << "[INFO] Imagen escalada." << endl;
    }
    cout << "Dimensiones: " << ancho << " x " << alto << endl;
    cout << "Canales: " << canales << endl;
}

void Imagen::guardarImagen(const std::string &nombreArchivo) const {
    unsigned char* buffer = new unsigned char[alto * ancho * canales];
    int indice = 0;
    
    // Agregar código de depuración
    cout << "Dimensiones al guardar: " << ancho << " x " << alto <<"pixeles" << " x " << canales << "canales"<< endl;
    
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            for (int c = 0; c < canales; c++) {
                // Verificar que el puntero es válido antes de acceder
                if (pixeles && pixeles[y] && pixeles[y][x]) {
                    buffer[indice++] = pixeles[y][x][c];
                } else {
                    buffer[indice++] = 0; // Valor por defecto en caso de error
                }
            }
        }
    }
    
    // Guardar la imagen usando stb_image_write
    
    if (!stbi_write_png(nombreArchivo.c_str(), ancho, alto, canales, buffer, ancho * canales)) {
        cerr << "Error: No se pudo guardar la imagen en '" << nombreArchivo << "'.\n";
        delete[] buffer;
        exit(1);
    }
    delete[] buffer;
    cout << "[INFO] Imagen guardada correctamente en '" << nombreArchivo << "'.\n";
}

void Imagen::invertirColores() {
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            for (int c = 0; c < canales; c++) {
                pixeles[y][x][c] = 255 - pixeles[y][x][c];
            }
        }
    }
}

void Imagen::liberarImagen(unsigned char***& matriz, int h, int w) {
    if (!matriz) return;
    
    for (int y = 0; y < h; ++y) {
        if (matriz[y]) {
            for (int x = 0; x < w; ++x) {
                if (matriz[y][x]) {
                    if (allocador)
                        allocador->free(matriz[y][x]);
                    else
                        delete[] matriz[y][x];
                    matriz[y][x] = nullptr;  // Importante para evitar uso después de liberación
                }
            }
            delete[] matriz[y];
            matriz[y] = nullptr;
        }
    }
    delete[] matriz;
    matriz = nullptr;
}

void Imagen::rotar(double anguloEnGrados) {
    double angulo = anguloEnGrados * M_PI / 180.0;
    int diagonal = (int)ceil(sqrt(ancho * ancho + alto * alto));
    int nuevoAncho = diagonal;
    int nuevoAlto = diagonal;
    double cxOrig = ancho / 2.0;
    double cyOrig = alto / 2.0;
    double cxNuevo = nuevoAncho / 2.0;
    double cyNuevo = nuevoAlto / 2.0;

    // Crear una nueva matriz para la imagen rotada
    unsigned char*** nuevaImagen = new unsigned char**[nuevoAlto];
    for (int y = 0; y < nuevoAlto; y++) {
        nuevaImagen[y] = new unsigned char*[nuevoAncho];
        for (int x = 0; x < nuevoAncho; x++) {
            // Asignar memoria para los canales
            nuevaImagen[y][x] = allocador ? 
                (unsigned char*)allocador->alloc(canales) : 
                new unsigned char[canales];
            
            if (!nuevaImagen[y][x]) {
                cerr << "Error: Fallo en la asignación de memoria para nuevaImagen[" << y << "][" << x << "]" << endl;
                exit(1);
            }
            
            // Inicializar con color de fondo (blanco o transparente)
            for (int c = 0; c < canales; c++) {
                nuevaImagen[y][x][c] = (c < 3) ? 255 : 255; // Blanco
            }
        }
    }

    // Aplicar la rotación a cada píxel
    for (int y = 0; y < nuevoAlto; y++) {
        for (int x = 0; x < nuevoAncho; x++) {
            // Calcular la posición relativa al centro de la nueva imagen
            double x0 = x - cxNuevo;
            double y0 = y - cyNuevo;
            
            // Aplicar la matriz de rotación inversa
            double origX = cos(-angulo) * x0 + sin(-angulo) * y0 + cxOrig;
            double origY = -sin(-angulo) * x0 + cos(-angulo) * y0 + cyOrig;
            
            // Verificar si el punto está dentro de la imagen original
            if (origX >= 0 && origX < ancho-1 && origY >= 0 && origY < alto-1) {
                // Coordenadas de los cuatro píxeles más cercanos
                int x1 = (int)floor(origX);
                int y1 = (int)floor(origY);
                int x2 = x1 + 1;
                int y2 = y1 + 1;
                
                // Calcular los factores de interpolación
                double dx = origX - x1;
                double dy = origY - y1;
                
                // Realizar interpolación bilineal para cada canal
                for (int c = 0; c < canales; c++) {
                    double p1 = pixeles[y1][x1][c];
                    double p2 = pixeles[y1][x2][c];
                    double p3 = pixeles[y2][x1][c];
                    double p4 = pixeles[y2][x2][c];
                    
                    // Interpolación bilineal
                    double valor = p1 * (1-dx) * (1-dy) + 
                                  p2 * dx * (1-dy) + 
                                  p3 * (1-dx) * dy + 
                                  p4 * dx * dy;
                    
                    nuevaImagen[y][x][c] = (unsigned char)round(valor);
                }
            }
        }
    }

    // Liberar la memoria de la imagen original
    liberarImagen(pixeles, alto, ancho);
    
    // Actualizar los atributos de la imagen
    pixeles = nuevaImagen;
    ancho = nuevoAncho;
    alto = nuevoAlto;
}

void Imagen::escalar(double factor) {
    int nuevoAncho = (int)(ancho * factor);
    int nuevoAlto = (int)(alto * factor);
    unsigned char*** nuevaImagen = new unsigned char**[nuevoAlto];
    for (int y = 0; y < nuevoAlto; ++y) {
        nuevaImagen[y] = new unsigned char*[nuevoAncho];
        for (int x = 0; x < nuevoAncho; ++x) {
            nuevaImagen[y][x] = allocador ? 
                (unsigned char*)allocador->alloc(canales) : 
                new unsigned char[canales];
            
            if (!nuevaImagen[y][x]) {
                cerr << "Error: Fallo en la asignación de memoria para nuevaImagen[" << y << "][" << x << "]" << endl;
                exit(1);
            }
            
            // Inicializar en blanco
            for (int c = 0; c < canales; ++c) {
                nuevaImagen[y][x][c] = (c < 3) ? 255 : 255; // Blanco
            }
        }
    }
    
    for (int y = 0; y < nuevoAlto; ++y) {
        for (int x = 0; x < nuevoAncho; ++x) {
            double origX = x / factor;
            double origY = y / factor;
            
            // Verificar límites
            if (origX >= 0 && origX < ancho-1 && origY >= 0 && origY < alto-1) {
                int x1 = (int)floor(origX);
                int y1 = (int)floor(origY);
                int x2 = x1 + 1;
                int y2 = y1 + 1;
                
                double dx = origX - x1;
                double dy = origY - y1;

                for (int c = 0; c < canales; ++c) {
                    unsigned char Q11 = pixeles[y1][x1][c];
                    unsigned char Q21 = pixeles[y1][x2][c];
                    unsigned char Q12 = pixeles[y2][x1][c];
                    unsigned char Q22 = pixeles[y2][x2][c];
                    
                    double valor = Q11 * (1-dx) * (1-dy) + 
                                  Q21 * dx * (1-dy) + 
                                  Q12 * (1-dx) * dy + 
                                  Q22 * dx * dy;
                    
                    nuevaImagen[y][x][c] = (unsigned char)round(valor);
                }
            }
        }
    }

    liberarImagen(pixeles, alto, ancho);
    pixeles = nuevaImagen;
    ancho = nuevoAncho;
    alto = nuevoAlto;
}