
#ifndef FIRSTFIT_H
#define FIRSTFIT_H

#include <vector>
#include "procesos.h"

// Un bloque libre tiene proceso == nullptr.
// Todos los tamanos y direcciones estan expresados en KB.
struct BloqueMemoria {
    int inicio;
    int tamano;
    Proceso* proceso;
};

class FirstFit {
private:
    int memoriaTotal;
    std::vector<BloqueMemoria> bloques;

public:
    explicit FirstFit(int tamanoMemoria = 4096);

    bool asignar(Proceso& proceso);
    bool liberar(int idProceso);
    bool contiene(int idProceso) const;

    int memoriaLibre() const;
    int memoriaOcupada() const;
    int getMemoriaTotal() const;

    const std::vector<BloqueMemoria>& obtenerBloques() const;

    void mostrarMemoria() const;
};

#endif
