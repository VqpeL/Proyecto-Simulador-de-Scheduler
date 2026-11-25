#include "roundRobin.h"
#include <iostream>
#include <stdexcept>

RoundRobin::RoundRobin(int cuanto)
    : cuantoSistema(cuanto)
{
    if (cuanto <= 0)
    {
        throw std::invalid_argument("El quantum debe ser positivo.");
    }
}

void RoundRobin::agregarProceso(Proceso &proceso)
{
    if (proceso.procesoTerminado())
    {
        throw std::invalid_argument(
            "No se puede agregar un proceso terminado.");
    }

    colaProcesos.push_back(&proceso);
}

ResultadoCiclo RoundRobin::ejecutarCiclo()
{
    if (colaProcesos.empty())
    {
        throw std::runtime_error("No hay procesos en la cola.");
    }

    Proceso *actual = colaProcesos.front();
    colaProcesos.pop_front();
    std::cout << "Atendiendo proceso ";
    actual->mostrar();
    std::cout << "\n";

    int consumido = actual->ejecutar(cuantoSistema);
    bool terminado = actual->procesoTerminado();

    if (!terminado)
    {
        colaProcesos.push_back(actual);
    }else{
        std::cout << "Descargando proceso ";
        actual->mostrar();
        std::cout << "\n";
        
    }

    ResultadoCiclo resultado;

    resultado.idProceso = actual->getId();
    resultado.tiempoConsumido = consumido;
    resultado.tiempoRestante = actual->getcuantoRestante();
    resultado.terminado = terminado;

    return resultado;
}

void RoundRobin::mostrarCola() const
{

    if (colaProcesos.empty())
    {
        std::cout << "Vacia";
    }
    else
    {
        for (const Proceso *proceso : colaProcesos)
        {
            proceso->mostrar();
            std::cout << " ";
        }
    }

    std::cout << std::endl;
}

bool RoundRobin::estaVacia() const
{
    return colaProcesos.empty();
}