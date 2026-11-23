#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include <deque>
#include "procesos.h"

struct ResultadoCiclo
{
    int idProceso;
    int tiempoConsumido;
    int tiempoRestante;
    bool terminado;
};

class RoundRobin
{
private:
    std::deque<Proceso*> colaProcesos;
    int cuantoSistema;

public:
    RoundRobin(int quantum);

    void agregarProceso(Proceso& proceso);
    ResultadoCiclo ejecutarCiclo();

    void mostrarCola() const;
    bool estaVacia() const;
};

#endif