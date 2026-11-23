
#ifndef BUDDYSYSTEM_H
#define BUDDYSYSTEM_H

#include <vector>
#include "procesos.h"

class BloqueBuddy
{
private:
    int inicio;
    int tamano;
    bool libre;
    int idProceso;

public:
    BloqueBuddy(int inicio, int tamano);

    int getInicio() const;
    int getTamano() const;
    bool estaLibre() const;
    int getIdProceso() const;

    void setTamano(int nuevoTamano);

    void ocupar(int id);
    void liberar();
};

class BuddySystem
{
private:
    static const int MIN_SPLIT_KB = 32;

    int memoriaTotalKB;
    std::vector<BloqueBuddy> bloques;

    int calcularTamanoRequerido(int tamanoProcesoKB) const;

    void fusionarBuddies();

public:
    BuddySystem(int memoriaMB);

    int getMemoriaTotalKB() const;
    
    bool asignarProceso(const Proceso &proceso);
    bool liberarProceso(int idProceso);

    void mostrarMemoria() const;
};

#endif
