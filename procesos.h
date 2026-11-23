#ifndef PROCESO_H
#define PROCESO_H

class Proceso
{
private:
    int id;
    int tamano;
    int cuantoInicial; //Tiempo inicial del proceso
    int cuantoRestante; // Tiempo que queda despues de ejecutar el proceso (Control para Round Robin)


public:
    Proceso(int id, int tamano, int cuanto);

    int getId() const;
    int getTamano() const;
    int getcuantoInicial() const;
    int getcuantoRestante() const;

    void ejecutar(int tiempo);
    bool procesoTerminado() const;
    void mostrar() const;
};

#endif