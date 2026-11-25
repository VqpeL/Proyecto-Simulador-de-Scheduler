
#include <iostream>
#include <list>
#include <deque>
#include <memory>
#include <random>
#include <limits>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h>

#include "procesos.h"
#include "firstFit.h"
#include "buddySystem.h"
#include "roundRobin.h"

using namespace std;

mt19937 generador(random_device{}());

int aleatorio(int minimo, int maximo)
{
    uniform_int_distribution<int> distribucion(minimo, maximo);
    return distribucion(generador);
}

// Limpiar buffer
void limpiarEntrada()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Espera a un enter
void pausar()
{
    cout << "\nPresiona enter para continuar";
    string entrada;
    getline(cin, entrada);
}

void mostrarPendientes(const std::deque<Proceso *> &pendientes)
{
    std::cout << "\nProcesos pendientes a entrar: ";

    if (pendientes.empty())
    {
        std::cout << "Ninguno";
    }
    else
    {
        for (const Proceso *proceso : pendientes)
        {
            proceso->mostrar();
            std::cout << " ";
        }
    }

    std::cout << '\n';
}

int main()
{

    int memoriaMB;
    int cuantoSistema;
    int tamanoMaximo;
    int cuantoMaximo;
    int algoritmo;

    cout << "SIMULADOR SCHEDULER CON ROUND ROBIN\n";

    cout << "\nMemoria maxima:\n";
    cout << "1. 1 MB\n";
    cout << "2. 4 MB\n";
    cout << "3. 8 MB\n";
    cout << "Opcion: ";
    cin >> memoriaMB;

    if (cin.fail() || memoriaMB < 1 || memoriaMB > 3)
    {
        cout << "Opcion invalida\n";
        return 1;
    }

    if (memoriaMB == 1)
        memoriaMB = 1;
    else if (memoriaMB == 2)
        memoriaMB = 4;
    else
        memoriaMB = 8;

    int memoriaKB = memoriaMB * 1024;

    // Config ROUND ROBIN
    cout << "\nCuanto del sistema: ";
    cin >> cuantoSistema;

    if (cin.fail() || cuantoSistema <= 0)
    {
        cout << "Cuanto invalido\n";
        return 1;
    }

    // Config PROCESOS
    cout << "\nTamanio maximo de un proceso (KB): ";
    cin >> tamanoMaximo;

    if (cin.fail() || tamanoMaximo < 32 ||
        tamanoMaximo > memoriaKB)
    {
        cout << "Tamanio maximo invalido\n";
        return 1;
    }

    cout << "\nCuanto maximo de proceso: ";
    cin >> cuantoMaximo;

    if (cin.fail() || cuantoMaximo <= 0)
    {
        cout << "Cuanto maximo invalido\n";
        return 1;
    }

    // SELECCION
    cout << "\nSelecciona el algoritmo:\n";
    cout << "1. Primer Ajuste\n";
    cout << "2. Buddy System\n";
    cout << "Opcion: ";
    cin >> algoritmo;

    if (cin.fail() || (algoritmo != 1 && algoritmo != 2))
    {
        cout << "Algoritmo invalido\n";
        return 1;
    }

    limpiarEntrada();

    unique_ptr<FirstFit> firstFit;
    unique_ptr<BuddySystem> buddy;

    if (algoritmo == 1)
    {
        firstFit = make_unique<FirstFit>(memoriaKB);
    }
    else
    {
        buddy = make_unique<BuddySystem>(memoriaMB);
    }

    // CREAR ROUND ROBIN
    RoundRobin roundRobin(cuantoSistema);

    // Almacenar procesos
    list<Proceso> procesos;

    // Procesos pendientes de entrar
    deque<Proceso *> pendientes;

    int siguienteId = 1;
    int ciclo = 0;
    int procesosTerminados = 0;

    // Generar prceso random
    auto generarProceso = [&]()
    {
        int tamano = aleatorio(32, tamanoMaximo);
        int cuanto = aleatorio(1, cuantoMaximo);

        procesos.emplace_back(siguienteId, tamano, cuanto);

        Proceso *nuevo = &procesos.back();

        cout << "Entra proceso  ";
        nuevo->mostrar();
        cout << "\n";

        pendientes.push_back(nuevo);

        siguienteId++;
    };

    cout << "\n------INICIO DE LA SIMULACION=------\n";

    bool continuar = true;
    int delayMs = 1000;

    while (continuar)
    {

        ciclo++;

        cout << "\n";
        // Generar un proceso nuevo en cada ciclo
        if(pendientes.empty()){
            generarProceso();
        }
        
        // intenta asignar
        if (!pendientes.empty())
        {

            Proceso *proceso = pendientes.front();

            bool asignado = false;

            if (algoritmo == 1)
            {
                asignado = firstFit->asignar(*proceso);
                firstFit->mostrarMemoria();
            }
            else
            {
                asignado = buddy->asignarProceso(*proceso);
                buddy->mostrarMemoria();
            }

            if (asignado)
            {

                roundRobin.agregarProceso(*proceso);

                pendientes.pop_front();
            }
            else
            {

                cout << "No hay espacio para P"
                     << proceso->getId() << ".\n";

                cout << "El proceso queda pendiente\n";
            }
        }

        // ejecutar rr
        // ejecutar rr
        if (!roundRobin.estaVacia())
        {
            ResultadoCiclo resultado = roundRobin.ejecutarCiclo();

            if (resultado.terminado)
            {
                // Liberar memoria
                if (algoritmo == 1)
                {
                    firstFit->liberar(resultado.idProceso);
                }
                else
                {
                    buddy->liberarProceso(resultado.idProceso);
                }

                procesosTerminados++;
            }
        }
        else
        {
            cout << "No hay procesos para ejecutar\n";
        }

        // Actualizar estado de los bloques antes de imprimir
        if (algoritmo != 1 && buddy != nullptr)
        {
            buddy->actualizarProcesos(procesos);
        }

        cout << "Lista Round Robin ";
        roundRobin.mostrarCola();

        if (algoritmo == 1)
        {
            firstFit->mostrarMemoria();
        }
        else
        {
            buddy->mostrarMemoria();
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));

        if (_kbhit())
        {
            char tecla = _getch();
            if (tecla == 's' || tecla == 'S')
            {
                continuar = false;
            }
        }
    }

    cout << "\nFIN DE LA SIMULACION\n";

    cout << "Procesos generados: "
         << siguienteId - 1 << "\n";

    cout << "Procesos atendidos: "
         << procesosTerminados << "\n";

    mostrarPendientes(pendientes);
    cout << "\n";

    cout << "Procesos pendientes:";
    roundRobin.mostrarCola();
    cout << "\n";

    cout << "\nEstado final de memoria:\n";

    if (algoritmo == 1)
    {
        firstFit->mostrarMemoria();
    }
    else
    {
        buddy->mostrarMemoria();
    }

    cout << "\nPresiona Enter para cerrar el programa...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}
