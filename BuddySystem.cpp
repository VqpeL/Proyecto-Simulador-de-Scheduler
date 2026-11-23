#include <iostream>
#include <vector>
#include <stdexcept>

class BloqueBuddy {
private:
    int inicio;     // Direccion/offset inicial en KB
    int tamano;     // Tamano del bloque en KB (potencia de 2, >= 32)
    bool libre;     // true = libre, false = ocupado
    int idProceso;  // ID del proceso si esta ocupado, 0 si esta libre

public:
    BloqueBuddy(int inicio, int tamano, bool libre = true, int idProceso = 0)
        : inicio(inicio), tamano(tamano), libre(libre), idProceso(idProceso) {}

    int getInicio() const { return inicio; }
    int getTamano() const { return tamano; }
    bool estaLibre() const { return libre; }
    int getIdProceso() const { return idProceso; }

    void setInicio(int nuevoInicio) { inicio = nuevoInicio; }
    void setTamano(int nuevoTamano) { tamano = nuevoTamano; }

    void ocupar(int id) {
        libre = false;
        idProceso = id;
    }

    void liberar() {
        libre = true;
        idProceso = 0;
    }
};

class BuddySystem {
private:
    static const int MIN_SPLIT_KB = 32; // Tamano minimo de split: 32 KB
    int memoriaTotalKB;                 // 1 MB = 1024, 4 MB = 4096, 8 MB = 8192
    std::vector<BloqueBuddy> bloques;

    // Calcula la potencia de 2 necesaria respetando el minimo de 32 KB
    int calcularTamanoRequerido(int tamanoProcesoKB) const {
        if (tamanoProcesoKB <= 0 || tamanoProcesoKB > memoriaTotalKB) {
            return -1;
        }
        int requerido = MIN_SPLIT_KB;
        while (requerido < tamanoProcesoKB && requerido * 2 <= memoriaTotalKB) {
            requerido *= 2;
        }
        if (requerido < tamanoProcesoKB) {
            return -1;
        }
        return requerido;
    }

    // Fusiona bloques buddy adyacentes que esten libres y sean del mismo tamano
    void fusionarBuddies() {
        bool huboFusion = true;

        while (huboFusion) {
            huboFusion = false;

            for (size_t i = 0; i + 1 < bloques.size(); ++i) {
                BloqueBuddy& actual = bloques[i];
                BloqueBuddy& siguiente = bloques[i + 1];

                // Deben estar ambos libres y tener exactamente el mismo tamano
                if (actual.estaLibre() && siguiente.estaLibre() &&
                    actual.getTamano() == siguiente.getTamano()) {

                    int tam = actual.getTamano();
                    int tamPadre = tam * 2;

                    // Verificamos que nazcan del mismo bloque padre de tamano (2 * tam):
                    // El hijo izquierdo siempre empieza en un multiplo exacto de (2 * tam)
                    bool esParBuddy = (actual.getInicio() % tamPadre == 0) &&
                                      (siguiente.getInicio() == actual.getInicio() + tam);

                    if (esParBuddy) {
                        actual.setTamano(tamPadre);
                        bloques.erase(bloques.begin() + static_cast<long>(i + 1));
                        huboFusion = true;
                        break; // Reiniciamos el recorrido tras fusionar
                    }
                }
            }
        }
    }

public:
    // memoriaMB debe ser 1, 4 o 8, luego se convierten en KB
    BuddySystem(int memoriaMB) {
        if (memoriaMB != 1 && memoriaMB != 4 && memoriaMB != 8) {
            throw std::invalid_argument("La memoria fija debe ser de 1, 4 o 8 MB.");
        }
        memoriaTotalKB = memoriaMB * 1024;
        bloques.push_back(BloqueBuddy(0, memoriaTotalKB, true, 0));
    }

    int getMemoriaTotalKB() const { return memoriaTotalKB; }

    bool asignarProceso(int idProceso, int tamanoProcesoKB) {
        int tamReq = calcularTamanoRequerido(tamanoProcesoKB);
        if (tamReq == -1) {
            return false;
        }

        // Buscamos el bloque libre mas pequeno que pueda contener tamReq
        int indiceElegido = -1;
        for (size_t i = 0; i < bloques.size(); ++i) {
            if (bloques[i].estaLibre() && bloques[i].getTamano() >= tamReq) {
                if (indiceElegido == -1 || bloques[i].getTamano() < bloques[indiceElegido].getTamano()) {
                    indiceElegido = static_cast<int>(i);
                }
            }
        }

        if (indiceElegido == -1) {
            return false; // No hay espacio disponible suficiente
        }

        // Dividir a la mitad (split) hasta llegar al tamano requerido
        while (bloques[indiceElegido].getTamano() > tamReq) {
            int mitad = bloques[indiceElegido].getTamano() / 2;
            if (mitad < MIN_SPLIT_KB) {
                break;
            }

            int inicioOriginal = bloques[indiceElegido].getInicio();
            BloqueBuddy izq(inicioOriginal, mitad, true, 0);
            BloqueBuddy der(inicioOriginal + mitad, mitad, true, 0);

            bloques[indiceElegido] = izq;
            bloques.insert(bloques.begin() + indiceElegido + 1, der);
        }

        if (bloques[indiceElegido].getTamano() < tamReq) {
            return false;
        }

        bloques[indiceElegido].ocupar(idProceso);
        return true;
    }

    bool liberarProceso(int idProceso) {
        bool encontrado = false;

        for (size_t i = 0; i < bloques.size(); ++i) {
            if (!bloques[i].estaLibre() && bloques[i].getIdProceso() == idProceso) {
                bloques[i].liberar();
                encontrado = true;
            }
        }

        if (encontrado) {
            fusionarBuddies();
        }
        return encontrado;
    }

    void mostrarMemoria() const {
        std::cout << "[Memoria Buddy (" << memoriaTotalKB << " KB)]: ";
        for (size_t i = 0; i < bloques.size(); ++i) {
            const BloqueBuddy& b = bloques[i];
            std::cout << "(" << b.getTamano() << "KB | ";
            if (b.estaLibre()) {
                std::cout << "LIBRE";
            } else {
                std::cout << "P" << b.getIdProceso();
            }
            std::cout << ")";
            if (i + 1 < bloques.size()) {
                std::cout << " - ";
            }
        }
        std::cout << "\n";
    }
};

int main() {
    // Prueba rapida con 1 MB (1024 KB)
    BuddySystem buddy(1);
    buddy.mostrarMemoria();

    buddy.asignarProceso(1, 100); //Todo esta en KB
    buddy.mostrarMemoria();

    buddy.asignarProceso(2, 240); 
    buddy.mostrarMemoria();

    buddy.asignarProceso(3, 64);  
    buddy.mostrarMemoria();

    buddy.asignarProceso(4, 256); 
    buddy.mostrarMemoria();

    buddy.liberarProceso(2);
    std::cout << "Tras liberar P2:\n";
    buddy.mostrarMemoria();

    buddy.liberarProceso(1);
    std::cout << "Tras liberar P1:\n";
    buddy.mostrarMemoria();

    return 0;
}