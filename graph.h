#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <chrono>
#include <iomanip>
#include <climits>

using namespace std;
using namespace std::chrono;

// Struktura reprezentująca krawędź grafu
struct Edge {
    int source;
    int destination;
    int weight;
};

// Struktura reprezentująca node w liście sąsiedztwa
struct AdjListNode {
    int dest;
    int weight;
    AdjListNode* next;
};

// Struktura do reprezentowania grafu
class Graph {
private:
    int V; // liczba wierzchołków
    int E; // liczba krawędzi
    double density; // gęstość grafu

    // Reprezentacja macierzowa
    int** adjMatrix;

    // Reprezentacja listowa
    AdjListNode** adjList;

    // Pomocnicza funkcja do losowania liczb
    int random(int min, int max) {
        return min + rand() % (max - min + 1);
    }

public:
    // Konstruktor
    Graph(int vertices) {
        this->V = vertices;
        this->E = 0;
        this->density = 0.0;

        // Inicjalizacja macierzy sąsiedztwa
        adjMatrix = new int*[V];
        for (int i = 0; i < V; i++) {
            adjMatrix[i] = new int[V];
            for (int j = 0; j < V; j++) {
                adjMatrix[i][j] = 0; // 0 oznacza brak krawędzi
            }
        }

        // Inicjalizacja listy sąsiedztwa
        adjList = new AdjListNode*[V];
        for (int i = 0; i < V; i++) {
            adjList[i] = nullptr;
        }
    }

    // Destruktor
    ~Graph() {
        // Zwolnienie pamięci macierzy sąsiedztwa
        for (int i = 0; i < V; i++) {
            delete[] adjMatrix[i];
        }
        delete[] adjMatrix;

        // Zwolnienie pamięci listy sąsiedztwa
        for (int i = 0; i < V; i++) {
            AdjListNode* current = adjList[i];
            while (current) {
                AdjListNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] adjList;
    }

    // Dodanie krawędzi do grafu
    void addEdge(int src, int dest, int weight) {
        // Aktualizacja macierzy sąsiedztwa
        adjMatrix[src][dest] = weight;
        adjMatrix[dest][src] = weight; // Graf nieskierowany dla MST

        // Dodanie do listy sąsiedztwa dla źródła
        AdjListNode* newNode = new AdjListNode;
        newNode->dest = dest;
        newNode->weight = weight;
        newNode->next = adjList[src];
        adjList[src] = newNode;

        // Dodanie do listy sąsiedztwa dla celu (graf nieskierowany)
        newNode = new AdjListNode;
        newNode->dest = src;
        newNode->weight = weight;
        newNode->next = adjList[dest];
        adjList[dest] = newNode;

        E++; // Zwiększ liczbę krawędzi
        // Aktualizacja gęstości
        int maxEdges = V * (V - 1) / 2; // Dla grafu nieskierowanego
        density = (double)E / maxEdges;
    }

    // Wczytaj graf z pliku
    bool loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Nie można otworzyć pliku " << filename << endl;
            return false;
        }

        // Wyczyszczenie poprzednich danych
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                adjMatrix[i][j] = 0;
            }

            AdjListNode* current = adjList[i];
            while (current) {
                AdjListNode* temp = current;
                current = current->next;
                delete temp;
            }
            adjList[i] = nullptr;
        }

        int edges, vertices;
        file >> edges >> vertices;

        // Sprawdzenie czy liczba wierzchołków się zgadza
        if (vertices != V) {
            cout << "Liczba wierzchołków w pliku (" << vertices << ") różni się od aktualnej (" << V << ")." << endl;
            file.close();
            return false;
        }

        E = 0; // Zerowanie liczby krawędzi
        
        for (int i = 0; i < edges; i++) {
            int src, dest, weight;
            file >> src >> dest >> weight;
            
            if (src >= V || dest >= V || src < 0 || dest < 0) {
                cout << "Błędne indeksy wierzchołków w pliku: " << src << " lub " << dest << endl;
                file.close();
                return false;
            }
            
            addEdge(src, dest, weight);
        }

        file.close();
        return true;
    }

    // Wygeneruj losowy graf o zadanej gęstości
    void generateRandom(double targetDensity) {
        // Wyczyszczenie poprzednich danych
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                adjMatrix[i][j] = 0;
            }

            AdjListNode* current = adjList[i];
            while (current) {
                AdjListNode* temp = current;
                current = current->next;
                delete temp;
            }
            adjList[i] = nullptr;
        }

        E = 0; // Zerowanie liczby krawędzi

        // Najpierw generujemy drzewo rozpinające (V-1 krawędzi)
        bool* visited = new bool[V];
        for (int i = 0; i < V; i++) {
            visited[i] = false;
        }

        visited[0] = true; // Zaczynamy od wierzchołka 0
        int visitedCount = 1;

        // Generujemy drzewo rozpinające
        while (visitedCount < V) {
            int src = random(0, V - 1);
            if (!visited[src]) continue;

            int dest = random(0, V - 1);
            if (visited[dest] || src == dest) continue;

            int weight = random(1, 100); // Losowa waga od 1 do 100
            addEdge(src, dest, weight);
            visited[dest] = true;
            visitedCount++;
        }

        delete[] visited;

        // Oblicz ile jeszcze krawędzi trzeba dodać, aby osiągnąć zadaną gęstość
        int maxEdges = V * (V - 1) / 2; // Maksymalna liczba krawędzi w grafie nieskierowanym
        int targetEdges = (int)(targetDensity * maxEdges);
        int remainingEdges = targetEdges - E;

        // Dodawanie pozostałych krawędzi
        while (remainingEdges > 0) {
            int src = random(0, V - 1);
            int dest = random(0, V - 1);
            
            // Pomijamy pętle i już istniejące krawędzie
            if (src == dest || adjMatrix[src][dest] != 0) {
                continue;
            }
            
            int weight = random(1, 100); // Losowa waga od 1 do 100
            addEdge(src, dest, weight);
            remainingEdges--;
        }
    }

    // Wyświetl graf w reprezentacji macierzowej
    void displayMatrix() {
        cout << "Reprezentacja macierzowa grafu:" << endl;
        cout << "  ";
        for (int i = 0; i < V; i++) {
            cout << setw(4) << i;
        }
        cout << endl;
        
        for (int i = 0; i < V; i++) {
            cout << setw(2) << i << " ";
            for (int j = 0; j < V; j++) {
                cout << setw(4) << adjMatrix[i][j];
            }
            cout << endl;
        }
    }

    // Wyświetl graf w reprezentacji listowej
    void displayList() {
        cout << "Reprezentacja listowa grafu:" << endl;
        for (int i = 0; i < V; i++) {
            cout << "Wierzchołek " << i << ": ";
            AdjListNode* current = adjList[i];
            while (current) {
                cout << "(" << current->dest << ", " << current->weight << ") ";
                current = current->next;
            }
            cout << endl;
        }
    }

    // Algorytm Prima - wersja macierzowa
    void primMST_Matrix() {
        cout << "\nWyniki algorytmu Prima (macierzowo):" << endl;
        
        int* parent = new int[V]; // Tablica przechowująca MST
        int* key = new int[V];    // Tablica kluczy (min waga dla każdego wierzchołka)
        bool* mstSet = new bool[V]; // Wierzchołki włączone do MST
        
        // Inicjalizacja
        for (int i = 0; i < V; i++) {
            key[i] = INT_MAX;
            mstSet[i] = false;
        }
        
        // Zawsze zaczynamy od wierzchołka 0
        key[0] = 0;
        parent[0] = -1; // Korzeń MST
        
        // Konstruujemy MST z V wierzchołków
        for (int count = 0; count < V - 1; count++) {
            // Wybierz wierzchołek o najmniejszym kluczu spośród niewybranych
            int u = -1;
            int min = INT_MAX;
            for (int v = 0; v < V; v++) {
                if (!mstSet[v] && key[v] < min) {
                    min = key[v];
                    u = v;
                }
            }
            
            // Jeśli nie znaleziono wierzchołka (graf rozłączny)
            if (u == -1) break;
            
            mstSet[u] = true; // Dodaj do MST
            
            // Zaktualizuj klucze sąsiadów
            for (int v = 0; v < V; v++) {
                if (adjMatrix[u][v] && !mstSet[v] && adjMatrix[u][v] < key[v]) {
                    parent[v] = u;
                    key[v] = adjMatrix[u][v];
                }
            }
        }
        
        // Wyświetl krawędzie MST i oblicz sumę wag
        int totalWeight = 0;
        cout << "Krawędź \tWaga\n";
        for (int i = 1; i < V; i++) {
            if (parent[i] != -1) {
                cout << parent[i] << " - " << i << " \t" << adjMatrix[i][parent[i]] << endl;
                totalWeight += adjMatrix[i][parent[i]];
            }
        }
        cout << "Suma wag MST: " << totalWeight << endl;
        
        delete[] parent;
        delete[] key;
        delete[] mstSet;
    }

    // Algorytm Prima - wersja listowa
    void primMST_List() {
        cout << "\nWyniki algorytmu Prima (listowo):" << endl;
        
        int* parent = new int[V]; // Tablica przechowująca MST
        int* key = new int[V];    // Tablica kluczy (min waga dla każdego wierzchołka)
        bool* mstSet = new bool[V]; // Wierzchołki włączone do MST
        
        // Inicjalizacja
        for (int i = 0; i < V; i++) {
            key[i] = INT_MAX;
            mstSet[i] = false;
        }
        
        // Zawsze zaczynamy od wierzchołka 0
        key[0] = 0;
        parent[0] = -1; // Korzeń MST
        
        // Konstruujemy MST z V wierzchołków
        for (int count = 0; count < V - 1; count++) {
            // Wybierz wierzchołek o najmniejszym kluczu spośród niewybranych
            int u = -1;
            int min = INT_MAX;
            for (int v = 0; v < V; v++) {
                if (!mstSet[v] && key[v] < min) {
                    min = key[v];
                    u = v;
                }
            }
            
            // Jeśli nie znaleziono wierzchołka (graf rozłączny)
            if (u == -1) break;
            
            mstSet[u] = true; // Dodaj do MST
            
            // Zaktualizuj klucze sąsiadów
            AdjListNode* current = adjList[u];
            while (current) {
                int v = current->dest;
                if (!mstSet[v] && current->weight < key[v]) {
                    parent[v] = u;
                    key[v] = current->weight;
                }
                current = current->next;
            }
        }
        
        // Wyświetl krawędzie MST i oblicz sumę wag
        int totalWeight = 0;
        cout << "Krawędź \tWaga\n";
        for (int i = 1; i < V; i++) {
            if (parent[i] != -1) {
                // Znajdź wagę krawędzi w liście sąsiedztwa
                int weight = 0;
                AdjListNode* current = adjList[parent[i]];
                while (current) {
                    if (current->dest == i) {
                        weight = current->weight;
                        break;
                    }
                    current = current->next;
                }
                
                cout << parent[i] << " - " << i << " \t" << weight << endl;
                totalWeight += weight;
            }
        }
        cout << "Suma wag MST: " << totalWeight << endl;
        
        delete[] parent;
        delete[] key;
        delete[] mstSet;
    }

    // Struktura pomocnicza do algorytmu Kruskala
    struct Subset {
        int parent;
        int rank;
    };

    // Funkcje pomocnicze dla algorytmu Kruskala
    int find(Subset subsets[], int i) {
        if (subsets[i].parent != i)
            subsets[i].parent = find(subsets, subsets[i].parent);
        return subsets[i].parent;
    }

    void Union(Subset subsets[], int x, int y) {
        int rootX = find(subsets, x);
        int rootY = find(subsets, y);

        if (subsets[rootX].rank < subsets[rootY].rank)
            subsets[rootX].parent = rootY;
        else if (subsets[rootX].rank > subsets[rootY].rank)
            subsets[rootY].parent = rootX;
        else {
            subsets[rootY].parent = rootX;
            subsets[rootX].rank++;
        }
    }

    // Algorytm Kruskala - wersja macierzowa
    void kruskalMST_Matrix() {
        cout << "\nWyniki algorytmu Kruskala (macierzowo):" << endl;
        
        // Utwórz tablicę krawędzi
        Edge* edges = new Edge[E];
        int edgeCount = 0;
        
        // Wypełnij tablicę krawędzi z macierzy sąsiedztwa
        for (int i = 0; i < V; i++) {
            for (int j = i + 1; j < V; j++) {  // j = i+1 bo graf nieskierowany i macierz symetryczna
                if (adjMatrix[i][j] != 0) {
                    edges[edgeCount].source = i;
                    edges[edgeCount].destination = j;
                    edges[edgeCount].weight = adjMatrix[i][j];
                    edgeCount++;
                }
            }
        }
        
        // Sortowanie krawędzi (bubble sort dla prostoty)
        for (int i = 0; i < edgeCount - 1; i++) {
            for (int j = 0; j < edgeCount - i - 1; j++) {
                if (edges[j].weight > edges[j + 1].weight) {
                    Edge temp = edges[j];
                    edges[j] = edges[j + 1];
                    edges[j + 1] = temp;
                }
            }
        }
        
        // Alokacja tablicy wynikowej MST
        Edge* result = new Edge[V - 1];
        
        // Tworzenie podzbiorów dla union-find
        Subset* subsets = new Subset[V];
        for (int i = 0; i < V; i++) {
            subsets[i].parent = i;
            subsets[i].rank = 0;
        }
        
        int resultIndex = 0;  // Indeks dla tablicy wynikowej
        int i = 0;  // Indeks dla tablicy posortowanych krawędzi
        
        // Budowanie MST
        while (resultIndex < V - 1 && i < edgeCount) {
            Edge nextEdge = edges[i++];
            
            int x = find(subsets, nextEdge.source);
            int y = find(subsets, nextEdge.destination);
            
            // Jeśli dodanie krawędzi nie tworzy cyklu, dodaj ją do MST
            if (x != y) {
                result[resultIndex++] = nextEdge;
                Union(subsets, x, y);
            }
        }
        
        // Wyświetl krawędzie MST i oblicz sumę wag
        int totalWeight = 0;
        cout << "Krawędź \tWaga\n";
        for (i = 0; i < resultIndex; i++) {
            cout << result[i].source << " - " << result[i].destination << " \t" << result[i].weight << endl;
            totalWeight += result[i].weight;
        }
        cout << "Suma wag MST: " << totalWeight << endl;
        
        delete[] result;
        delete[] subsets;
        delete[] edges;
    }

    // Algorytm Kruskala - wersja listowa
    void kruskalMST_List() {
        cout << "\nWyniki algorytmu Kruskala (listowo):" << endl;
        
        // Utwórz tablicę krawędzi
        Edge* edges = new Edge[E];
        int edgeCount = 0;
        
        // Wypełnij tablicę krawędzi z listy sąsiedztwa
        for (int i = 0; i < V; i++) {
            AdjListNode* current = adjList[i];
            while (current) {
                // Dodaj krawędź tylko jeśli dest > source (unikamy duplikatów w grafie nieskierowanym)
                if (current->dest > i) {
                    edges[edgeCount].source = i;
                    edges[edgeCount].destination = current->dest;
                    edges[edgeCount].weight = current->weight;
                    edgeCount++;
                }
                current = current->next;
            }
        }
        
        // Sortowanie krawędzi (bubble sort dla prostoty)
        for (int i = 0; i < edgeCount - 1; i++) {
            for (int j = 0; j < edgeCount - i - 1; j++) {
                if (edges[j].weight > edges[j + 1].weight) {
                    Edge temp = edges[j];
                    edges[j] = edges[j + 1];
                    edges[j + 1] = temp;
                }
            }
        }
        
        // Alokacja tablicy wynikowej MST
        Edge* result = new Edge[V - 1];
        
        // Tworzenie podzbiorów dla union-find
        Subset* subsets = new Subset[V];
        for (int i = 0; i < V; i++) {
            subsets[i].parent = i;
            subsets[i].rank = 0;
        }
        
        int resultIndex = 0;  // Indeks dla tablicy wynikowej
        int i = 0;  // Indeks dla tablicy posortowanych krawędzi
        
        // Budowanie MST
        while (resultIndex < V - 1 && i < edgeCount) {
            Edge nextEdge = edges[i++];
            
            int x = find(subsets, nextEdge.source);
            int y = find(subsets, nextEdge.destination);
            
            // Jeśli dodanie krawędzi nie tworzy cyklu, dodaj ją do MST
            if (x != y) {
                result[resultIndex++] = nextEdge;
                Union(subsets, x, y);
            }
        }
        
        // Wyświetl krawędzie MST i oblicz sumę wag
        int totalWeight = 0;
        cout << "Krawędź \tWaga\n";
        for (i = 0; i < resultIndex; i++) {
            cout << result[i].source << " - " << result[i].destination << " \t" << result[i].weight << endl;
            totalWeight += result[i].weight;
        }
        cout << "Suma wag MST: " << totalWeight << endl;
        
        delete[] result;
        delete[] subsets;
        delete[] edges;
    }

    // Metoda do testowania wydajności algorytmów
    void performanceTest(int numVertices, double density, int numTests) {
        Graph graph(numVertices);

        // Zmienne do mierzenia czasu
        high_resolution_clock::time_point startTime, endTime;
        double primMatrixTotal = 0, primListTotal = 0, kruskalMatrixTotal = 0, kruskalListTotal = 0;

        for (int test = 0; test < numTests; test++) {
            // Generuj nowy losowy graf dla każdego testu
            graph.generateRandom(density);

            // Mierz czas dla algorytmu Prima (macierzowo)
            startTime = high_resolution_clock::now();
            graph.primMST_Matrix();
            endTime = high_resolution_clock::now();
            primMatrixTotal += duration_cast<microseconds>(endTime - startTime).count();

            // Mierz czas dla algorytmu Prima (listowo)
            startTime = high_resolution_clock::now();
            graph.primMST_List();
            endTime = high_resolution_clock::now();
            primListTotal += duration_cast<microseconds>(endTime - startTime).count();

            // Mierz czas dla algorytmu Kruskala (macierzowo)
            startTime = high_resolution_clock::now();
            graph.kruskalMST_Matrix();
            endTime = high_resolution_clock::now();
            kruskalMatrixTotal += duration_cast<microseconds>(endTime - startTime).count();

            // Mierz czas dla algorytmu Kruskala (listowo)
            startTime = high_resolution_clock::now();
            graph.kruskalMST_List();
            endTime = high_resolution_clock::now();
            kruskalListTotal += duration_cast<microseconds>(endTime - startTime).count();
        }

        // Oblicz średnie czasy
        double primMatrixAvg = primMatrixTotal / numTests;
        double primListAvg = primListTotal / numTests;
        double kruskalMatrixAvg = kruskalMatrixTotal / numTests;
        double kruskalListAvg = kruskalListTotal / numTests;

        // Wyświetl wyniki
        cout << "Testy wydajności dla grafu o " << numVertices << " wierzchołkach i gęstości " << density * 100 << "%:" << endl;
        cout << "Algorytm Prima (macierzowo): " << primMatrixAvg << " mikrosekund" << endl;
        cout << "Algorytm Prima (listowo): " << primListAvg << " mikrosekund" << endl;
        cout << "Algorytm Kruskala (macierzowo): " << kruskalMatrixAvg << " mikrosekund" << endl;
        cout << "Algorytm Kruskala (listowo): " << kruskalListAvg << " mikrosekund" << endl;
    }

    // Gettery
    int getVertices() { return V; }
    int getEdges() { return E; }
    double getDensity() { return density; }
};