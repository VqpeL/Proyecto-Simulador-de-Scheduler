#ifndef BUDDYSYSTEM_H
#define BUDDYSYSTEM_H

#include <vector>
#include <list>
#include "procesos.h"

class BloqueBuddy
{
private:
    int inicio;
    int tamano;
    bool libre;
    Proceso proceso;

public:
    BloqueBuddy(int inicio, int tamano);

    int getInicio() const;
    int getTamano() const;
    bool estaLibre() const;
    int getIdProceso() const;
    const Proceso& getProceso() const;
    

    void setTamano(int nuevoTamano);

    void ocupar(const Proceso &pros);
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
    void actualizarProcesos(const std::list<Proceso> &listaProcesos);

    void mostrarMemoria() const;
};

#endif