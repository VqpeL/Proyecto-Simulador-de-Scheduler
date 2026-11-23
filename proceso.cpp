#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <stdexcept>
#include "procesos.h"


Proceso::Proceso(int id, int tamano, int cuanto){

    if (id <= 0 || tamano <= 0 || cuanto <= 0){
        throw std::invalid_argument("Los datos deben de ser positivos"); // // Throw genera una excepcion ante un valor no valido (stdexcept)
    }

    this->id = id;
    this->tamano = tamano;
    this->cuantoInicial = cuanto;
    this->cuantoRestante = cuanto;
}

int Proceso::getId() const{
    return id;
}

int Proceso::getTamano() const{
    return tamano;
}
int Proceso::getcuantoInicial() const{
    return cuantoInicial;
}
int Proceso::getcuantoRestante() const{
    return cuantoRestante;
}

void Proceso::ejecutar(int tiempo){
    if(tiempo <= 0){
        throw std::invalid_argument("Tiempo debe ser positivo");
    }
    else if(tiempo >= cuantoRestante){
        cuantoRestante = 0;
    }
    else{
        cuantoRestante -= tiempo;
    }
}

bool Proceso::procesoTerminado() const{
    return cuantoRestante == 0;
}

void Proceso::mostrar() const{
    std::cout << "[" << id << ", " << tamano << ", " << cuantoRestante << "]";
}