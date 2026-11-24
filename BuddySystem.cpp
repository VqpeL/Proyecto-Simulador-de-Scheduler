#include "BuddySystem.h"
#include <iostream>
#include <stdexcept>

BloqueBuddy::BloqueBuddy(int inicio, int tamano)
    : inicio(inicio),
      tamano(tamano),
      libre(true),
      proceso(1, 1, 1)
{
}

int BloqueBuddy::getInicio() const
{
    return inicio;
}

int BloqueBuddy::getTamano() const
{
    return tamano;
}

bool BloqueBuddy::estaLibre() const
{
    return libre;
}

int BloqueBuddy::getIdProceso() const
{
    return proceso.getId();
}

const Proceso& BloqueBuddy::getProceso() const
{
    return proceso;
}

void BloqueBuddy::setTamano(int nuevoTamano)
{
    tamano = nuevoTamano;
}

void BloqueBuddy::ocupar(const Proceso &pros)
{
    libre = false;
    proceso = pros;
}

void BloqueBuddy::liberar()
{
    libre = true;
}

BuddySystem::BuddySystem(int memoriaMB)
{
    if (memoriaMB != 1 && memoriaMB != 4 && memoriaMB != 8)
    {
        throw std::invalid_argument(
            "La memoria debe ser de 1, 4 u 8 MB.");
    }

    memoriaTotalKB = memoriaMB * 1024;
    bloques.push_back(BloqueBuddy(0, memoriaTotalKB));
}

int BuddySystem::getMemoriaTotalKB() const
{
    return memoriaTotalKB;
}

int BuddySystem::calcularTamanoRequerido(int tamanoProcesoKB) const
{
    if (tamanoProcesoKB <= 0 || tamanoProcesoKB > memoriaTotalKB)
    {
        return -1;
    }

    int requerido = MIN_SPLIT_KB;
    while (requerido < tamanoProcesoKB)
    {
        requerido *= 2;
    }

    return requerido;
}

bool BuddySystem::asignarProceso(const Proceso &proceso)
{
    int idProceso = proceso.getId();
    int tamanoProceso = proceso.getTamano();

    for (const BloqueBuddy &bloque : bloques)
    {
        if (!bloque.estaLibre() &&
            bloque.getIdProceso() == idProceso)
        {
            return false;
        }
    }

    int tamReq = calcularTamanoRequerido(tamanoProceso);
    if (tamReq == -1)
    {
        return false;
    }

    int indiceElegido = -1;
    for (size_t i = 0; i < bloques.size(); i++)
    {
        if (bloques[i].estaLibre() &&
            bloques[i].getTamano() >= tamReq)
        {
            if (indiceElegido == -1 ||
                bloques[i].getTamano() < bloques[indiceElegido].getTamano())
            {
                indiceElegido = static_cast<int>(i);
            }
        }
    }

    if (indiceElegido == -1)
    {
        return false;
    }

    while (bloques[indiceElegido].getTamano() > tamReq)
    {
        int mitad = bloques[indiceElegido].getTamano() / 2;
        int inicioOriginal = bloques[indiceElegido].getInicio();

        BloqueBuddy izquierda(inicioOriginal, mitad);
        BloqueBuddy derecha(inicioOriginal + mitad, mitad);

        bloques[indiceElegido] = izquierda;
        bloques.insert(
            bloques.begin() + indiceElegido + 1,
            derecha);
    }

    bloques[indiceElegido].ocupar(proceso);
    return true;
}

void BuddySystem::fusionarBuddies()
{
    bool huboFusion = true;

    while (huboFusion)
    {
        huboFusion = false;

        for (size_t i = 0; i + 1 < bloques.size(); i++)
        {
            BloqueBuddy &actual = bloques[i];
            BloqueBuddy &siguiente = bloques[i + 1];

            if (actual.estaLibre() &&
                siguiente.estaLibre() &&
                actual.getTamano() == siguiente.getTamano())
            {
                int tam = actual.getTamano();
                int tamPadre = tam * 2;

                bool sonBuddies =
                    (actual.getInicio() % tamPadre == 0) &&
                    (siguiente.getInicio() == actual.getInicio() + tam);

                if (sonBuddies)
                {
                    actual.setTamano(tamPadre);
                    bloques.erase(bloques.begin() + static_cast<long>(i + 1));
                    huboFusion = true;
                    break;
                }
            }
        }
    }
}

bool BuddySystem::liberarProceso(int idProceso)
{
    bool encontrado = false;

    for (BloqueBuddy &bloque : bloques)
    {
        if (!bloque.estaLibre() &&
            bloque.getIdProceso() == idProceso)
        {
            bloque.liberar();
            encontrado = true;
            break;
        }
    }

    if (encontrado)
    {
        fusionarBuddies();
    }

    return encontrado;
}

void BuddySystem::actualizarProcesos(const std::list<Proceso> &listaProcesos)
{
    for (BloqueBuddy &bloque : bloques)
    {
        if (!bloque.estaLibre())
        {
            int idBuscado = bloque.getIdProceso();

            for (const Proceso &p : listaProcesos)
            {
                if (p.getId() == idBuscado)
                {
                    bloque.ocupar(p);
                    break;
                }
            }
        }
    }
}

void BuddySystem::mostrarMemoria() const
{
    for (size_t i = 0; i < bloques.size(); i++)
    {
        const BloqueBuddy &bloque = bloques[i];

        if (bloque.estaLibre())
        {
            std::cout << "[0, " << bloque.getTamano() << ", 0]";
        }
        else
        {
            const Proceso &p = bloque.getProceso();
            std::cout << "["
                      << p.getId() << ", "
                      << p.getTamano() << ", "
                      << p.getcuantoRestante() << "]";
        }
    }
    std::cout << "\n";
}