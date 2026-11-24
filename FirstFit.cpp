#include "FirstFit.h"
#include <iostream>
#include <stdexcept>

// memorias fijas de 1, 4 u 8 MB.
FirstFit::FirstFit(int tamanoMemoria) : memoriaTotal(tamanoMemoria) {
    if (tamanoMemoria != 1024 && tamanoMemoria != 4096 &&tamanoMemoria != 8192) {
        throw std::invalid_argument("La memoria debe ser 1024, 4096 u 8192 KB"); // Si el tamanio no es valido detiene la creacion del objeto.
    }
    bloques.push_back({0, memoriaTotal, nullptr});
}

bool FirstFit::contiene(int idProceso) const {
    if (idProceso <= 0) return false;

    for (const BloqueMemoria& bloque : bloques) {
        if (bloque.proceso != nullptr &&
            bloque.proceso->getId() == idProceso) {
            return true;
        }
    }

    return false;
}

bool FirstFit::asignar(Proceso& proceso) {
    const int requerido = proceso.getTamano();

    if (proceso.getId() <= 0 || requerido <= 0 ||
        requerido > memoriaTotal || contiene(proceso.getId()) ||
        proceso.procesoTerminado()) {
        return false;
    }

    // First Fit: recorrer por direccion y usar el PRIMER hueco suficiente.
    for (std::size_t i = 0; i < bloques.size(); ++i) {

        if (bloques[i].proceso == nullptr &&
            bloques[i].tamano >= requerido) {

            if (bloques[i].tamano > requerido) {
                const BloqueMemoria sobrante = {
                    bloques[i].inicio + requerido,
                    bloques[i].tamano - requerido,
                    nullptr
                };

                bloques.insert(bloques.begin() + i + 1, sobrante);
                bloques[i].tamano = requerido;
            }

            bloques[i].proceso = &proceso;
            return true;
        }
    }

    // Puede haber memoria libre, pero no un hueco suficiente.
    return false;
}

bool FirstFit::liberar(int idProceso) {
    if (idProceso <= 0) return false;

    for (std::size_t i = 0; i < bloques.size(); ++i) {

        if (bloques[i].proceso != nullptr &&
            bloques[i].proceso->getId() == idProceso) {

            bloques[i].proceso = nullptr;

            // Fusionar con el siguiente hueco libre.
            if (i + 1 < bloques.size() &&
                bloques[i + 1].proceso == nullptr) {

                bloques[i].tamano += bloques[i + 1].tamano;
                bloques.erase(bloques.begin() + i + 1);
            }

            // Fusionar con el hueco anterior.
            if (i > 0 && bloques[i - 1].proceso == nullptr) {

                bloques[i - 1].tamano += bloques[i].tamano;
                bloques.erase(bloques.begin() + i);
            }

            return true;
        }
    }

    return false;
}

int FirstFit::memoriaLibre() const {
    int libre = 0;

    for (const BloqueMemoria& bloque : bloques) {
        if (bloque.proceso == nullptr) {
            libre += bloque.tamano;
        }
    }

    return libre;
}

int FirstFit::memoriaOcupada() const {
    return memoriaTotal - memoriaLibre();
}

int FirstFit::getMemoriaTotal() const {
    return memoriaTotal;
}

const std::vector<BloqueMemoria>& FirstFit::obtenerBloques() const {
    return bloques;
}

void FirstFit::mostrarMemoria() const {

    for (const BloqueMemoria& bloque : bloques) {
        std::cout << "[";

        if (bloque.proceso == nullptr) {
            std::cout << "0," << bloque.tamano << ",0]";
        }
        else {
            std::cout << bloque.proceso->getId() << ","
                      << bloque.tamano << ","
                      << bloque.proceso->getcuantoRestante()
                      << "]";
        }
    }
    std::cout << "\n";

    //std::cout << "Memoria libre: "
    //          << memoriaLibre() << "/"
    //          << memoriaTotal << " KB\n"; (esto, creo que quedaria bien meterlo)
}
