#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <chrono>
#include <iomanip>
#include "graph.h"

using namespace std;
using namespace std::chrono;

// Funkcja do czyszczenia ekranu konsoli
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Menu główne programu
void menu() {
    Graph* graph = nullptr;

    while (true) {
        clearScreen();
        cout << "=== PROGRAM DO ALGORYTMÓW GRAFOWYCH ===" << endl;
        cout << "1. Wczytaj graf z pliku" << endl;
        cout << "2. Wygeneruj losowy graf" << endl;
        cout << "3. Wyświetl graf" << endl;
        cout << "4. Algorytm Prima (macierzowo i listowo)" << endl;
        cout << "5. Algorytm Kruskala (macierzowo i listowo)" << endl;
        //cout << "6. Testy wydajności" << endl;
        cout << "0. Wyjście" << endl;
        cout << "Wybierz opcję: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 0: {
                if (graph) delete graph;
                return;
            }
            case 1: {
                string filename;
                cout << "Podaj nazwę pliku: ";
                cin >> filename;
                
                ifstream file(filename);
                if (!file.is_open()) {
                    cout << "Nie można otworzyć pliku " << filename << endl;
                    break;
                }
                
                int edges, vertices;
                file >> edges >> vertices;
                file.close();
                
                if (graph) delete graph;
                graph = new Graph(vertices);
                
                if (graph->loadFromFile(filename)) {
                    cout << "Graf wczytany z pliku." << endl;
                    // Wyświetl graf
                    graph->displayMatrix();
                    cout << endl;
                    graph->displayList();
                }
                break;
            }
            case 2: {
                int vertices;
                double density;
                cout << "Podaj liczbę wierzchołków: ";
                cin >> vertices;
                cout << "Podaj gęstość grafu (0.0-1.0): ";
                cin >> density;
                
                if (density < 0.0 || density > 1.0) {
                    cout << "Nieprawidłowa gęstość. Podaj wartość z zakresu 0.0-1.0." << endl;
                    break;
                }
                
                if (graph) delete graph;
                graph = new Graph(vertices);
                graph->generateRandom(density);
                
                cout << "Wygenerowano losowy graf o " << vertices << " wierzchołkach i gęstości " << density << endl;
                // Wyświetl graf
                graph->displayMatrix();
                cout << endl;
                graph->displayList();
                break;
            }
            case 3: {
                if (!graph) {
                    cout << "Najpierw wczytaj lub wygeneruj graf!" << endl;
                    break;
                }
                graph->displayMatrix();
                cout << endl;
                graph->displayList();
                break;
            }
            case 4: {
                if (!graph) {
                    cout << "Najpierw wczytaj lub wygeneruj graf!" << endl;
                    break;
                }
                graph->primMST_Matrix();
                graph->primMST_List();
                break;
            }
            case 5: {
                if (!graph) {
                    cout << "Najpierw wczytaj lub wygeneruj graf!" << endl;
                    break;
                }
                graph->kruskalMST_Matrix();
                graph->kruskalMST_List();
                break;
            }
            case 6: {
                cout << "Testy wydajności:" << endl;
                cout << "1. Test dla pojedynczego rozmiaru grafu" << endl;
                cout << "2. Pełne testy (7 rozmiarów x 3 gęstości x 50 instancji)" << endl;
                cout << "Wybierz opcję: ";
                
                int testChoice;
                cin >> testChoice;
                
                if (testChoice == 1) {
                    int size;
                    double density;
                    int numTests;
                    
                    cout << "Podaj liczbę wierzchołków: ";
                    cin >> size;
                    cout << "Podaj gęstość (0.0-1.0): ";
                    cin >> density;
                    cout << "Podaj liczbę testów: ";
                    cin >> numTests;
                    
                    if (graph) delete graph;
                    graph = new Graph(size);
                    graph->performanceTest(size, density, numTests);
                }
                else if (testChoice == 2) {
                    int sizes[] = {10, 20, 50, 100, 200, 500, 1000}; // 7 różnych rozmiarów
                    double densities[] = {0.2, 0.6, 0.99}; // 3 różne gęstości
                    int numTests = 50; // 50 instancji
                    
                    for (int s = 0; s < 7; s++) {
                        for (int d = 0; d < 3; d++) {
                            cout << "Testowanie dla " << sizes[s] << " wierzchołków i gęstości " << densities[d] * 100 << "%" << endl;
                            if (graph) delete graph;
                            graph = new Graph(sizes[s]);
                            graph->performanceTest(sizes[s], densities[d], numTests);
                            cout << "-------------------------------------" << endl;
                        }
                    }
                }
                break;
            }
            default:
                cout << "Nieprawidłowa opcja!" << endl;
        }
        
        cout << "\nNaciśnij Enter, aby kontynuować...";
        cin.ignore();
        cin.get();
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr))); // Inicjalizacja generatora liczb losowych

    menu();

    return 0;
}