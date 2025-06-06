#ifndef IMAGEN_H
#define IMAGEN_H

#include <string>
#include "buddy_allocator.h"

class Imagen {
public:
    Imagen(const std::string &nombreArchivo, BuddyAllocator *allocador = nullptr);
    ~Imagen();

    void invertirColores();
    void guardarImagen(const std::string &nombreArchivo) const;
    void mostrarInfo(bool esrotada) const;
    void escalar(double factor);
    void liberarImagen(unsigned char*** &matriz , int h, int w); 
    void rotar(double angulo);
    unsigned char*** asignarMemoria(int ancho, int alto, int canales);
    void liberarMemoria(); 

private:
    int alto;
    int ancho;
    int canales;
    unsigned char ***pixeles;
    BuddyAllocator *allocador;

    void convertirBufferAMatriz(unsigned char* buffer);
};

#endif
