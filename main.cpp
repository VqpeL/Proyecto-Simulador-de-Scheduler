
#include <iostream>
#include <list>
#include <deque>
#include <memory>
#include <random>
#include <limits>
#include <string>

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
    std::cout << "\nProcesos pendientes: ";

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

    cout << "       SIMULADOR SCHEDULER CON ROUND ROBIN\n";

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

        cout << "\nNuevo proceso: ";
        nuevo->mostrar();
        cout << "\n";

        pendientes.push_back(nuevo);

        siguienteId++;
    };

    // Generar el primer proceso
    generarProceso();
    cout << "------INICIO DE LA SIMULACION=------\n";

    bool continuar = true;

    while (continuar)
    {

        ciclo++;

        cout << "\n\n--------- CICLO " << ciclo << " ----------\n";

        // Generar un proceso nuevo en cada ciclo
        generarProceso();

        // intenta asignar
        if (!pendientes.empty())
        {

            Proceso *proceso = pendientes.front();

            cout << "\nIntentando ingresar: ";
            proceso->mostrar();
            cout << "\n";

            bool asignado = false;

            if (algoritmo == 1)
            {
                asignado = firstFit->asignar(*proceso);
            }
            else
            {
                asignado = buddy->asignarProceso(*proceso);
            }

            if (asignado)
            {

                cout << "Proceso P" << proceso->getId() << " asignado correctamente\n";

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
        if (!roundRobin.estaVacia())
        {

            cout << "\nAtendiendo proceso...\n";

            ResultadoCiclo resultado =
                roundRobin.ejecutarCiclo();

            cout << "\nProceso ejecutado: P"
                 << resultado.idProceso << "\n";

            cout << "Tiempo consumido: "
                 << resultado.tiempoConsumido << "\n";

            cout << "Tiempo restante: "
                 << resultado.tiempoRestante << "\n";

            if (resultado.terminado)
            {

                cout << "Proceso terminado\n";

                // Liberar memori
                if (algoritmo == 1)
                {
                    firstFit->liberar(resultado.idProceso);
                }
                else
                {
                    buddy->liberarProceso(resultado.idProceso);
                }

                cout << "Memoria liberada\n";

                procesosTerminados++;
            }
            else
            {
                cout << "El proceso regresa al final de la cola\n";
            }
        }
        else
        {
            cout << "\nNo hay procesos para ejecutar\n";
        }

        // MOSTRAR INFO
        cout << "\n---------- ESTADO DEL SISTEMA ----------\n";

        roundRobin.mostrarCola();

        mostrarPendientes(pendientes);

        cout << "\n---------- ESTADO DE MEMORIA ----------\n";

        if (algoritmo == 1)
        {
            firstFit->mostrarMemoria();
        }
        else
        {
            buddy->mostrarMemoria();
        }

        cout << "\nProcesos terminados: "
             << procesosTerminados << "\n";

        // CONTROLAR
        cout << "\nENTER = Siguiente ciclo";
        cout << "\nS = Detener simulacion";
        cout << "\nOpcion: ";

        char opcion;
        cin >> opcion;

        if (opcion == 'S' || opcion == 's')
        {
            continuar = false;
        }
    }

    cout << "       FIN DE LA SIMULACION\n";

    cout << "Procesos generados: "
         << siguienteId - 1 << "\n";

    cout << "Procesos terminados: "
         << procesosTerminados << "\n";

    cout << "Procesos pendientes de ingreso: "
         << pendientes.size() << "\n";

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
