#include <iostream>
using std::cout;
using std::cerr;

#include <cstdlib>
using std::rand; 
using std::srand;
using std::atoi;

#include <ctime>
using std::clock; 
using std::clock_t;

// ========================================== UTILITY ==================================

template <typename T>
T* makeCopy(T *v, int n) {
    T* copy = new T[n];
    for (int i = 0; i < n; i++) {
        copy[i] = v[i];
    }

    return copy;
}

template <typename T>
void swap(T *v, int i, int j) {
    T copy = v[i];
    v[i] = v[j];
    v[j] = copy;
}

void showVec(int *v, int n) {
    cout << "\n=======\n";
    for (int i = 0; i < n; i++) {
        cout << "v[" << i << "] = " << v[i] << "\n";
    }
}

template <typename T>
void ensureSorted(T *v, int n) {
    if (n <= 1) return;

    T last = *v;
    for (T *head = v + 1; head < v + n; head++) {
        if (last > *head) {
            cerr << "Erro! Vetor não está ordenado!";
            throw std::invalid_argument("Vetor de entrada não estava ordenado!");
        }
    }
}

int log2(int n) {
    int i = n;
    int r = 0;

    while (i > 1) {
        r++; i >>= 1;
    }

    return r;
}

int* increasing(int n) {
    int *v = new int[n];
    int value = rand() % n;

    for (int *head = v; head < v + n; head++, value++) *head = value;
    return v;
}

int* decreasing(int n) {
    int *v = new int[n];
    int value = rand() % n;

    for (int *head = v; head < v + n; head++, value--) *head = value;
    return v;
}

int* randomNumbers(int n) {
    int *v = new int[n];
    for (int *head = v; head < v + n; head++) *head = rand();
    return v;
}


// ========================================== UTILITY ==================================

// ========================================== HEAPSORT ==================================

int leftChildren(int i) { return 2*i + 1; }
int rightChildren(int i) { return 2*i + 2; }

template <typename T>
int highestWithinBounds(T *v, int i, int left, int right, int n) {
    int highest = i;

    if (left < n && v[left] > v[highest]) {
        highest = left;
    }

    if (right < n && v[right] > v[highest]) {
        highest = right;
    }

    return highest;
}

template <typename T>
void goDown(T *v, int i, int n) {
    int leftChildIndex = leftChildren(i);
    int rightChildIndex = rightChildren(i);

    int highest = highestWithinBounds(v, i, leftChildIndex, rightChildIndex, n);

    if (highest != i) {
        swap(v, highest, i);
        goDown(v, highest, n);
    }
}

template <typename T>
T removeMax(T *v, int n) {
    T max = v[0];

    v[0] = v[n - 1];
    goDown(v, 0, n - 1);

    return max;
}

template <typename T>
void makeHeap(T *v, int n) {
    for (int i = n - 1; i >= 0; i--) {
        goDown(v, i, n);
    }
}

template <typename T>
void heapsort(T *v, int n) {
    makeHeap(v, n);

    for (int i = 0; i < n - 1; i++) {
        T max = removeMax(v, n - i);
        v[n - 1 - i] = max;
    }
}

// ========================================== HEAPSORT ==================================

// ========================================== QUICK SORT ==================================

int pivo(int inicio, int fim) { 
    return inicio + (fim - inicio) / 2;
}

// idea: 
// we need to make sure every pivot selection will end with the pivot on one of the extremities
// this implementation makes sure that every pivot will end up at the beginning of the partitionated 
// slice, i.e, at the left of the slice at v[i, ..., j]
// So, we set up an array of the indices: 0 to n - 1, we call it pivotPositions
// then, since on the worst case we'll end up with n pivots, we must know where the first pivot must be, the 2nd one, the 3rd, so on
// to do that, we ask the index of the pivot going from i to n (remember: pivots will be at left, so i moves from left to right) 
// so, we swap the element at the pivot position with the beginning of the slice: pivotPositions[i] <-> pivotPositions[pivot_index]
// that way, we know that the i-th pivot was at position pivotPositions[i]
// now, we just have to populate the output array writing the pivots in positions indicated on pivotPositions..
template <typename T>
bool gerar_pior_caso (T *v, int n) { 
    int *pivotPositions = new int[n];

    for (int i = 0; i < n; i++) {
        pivotPositions[i] = i;
    }

    for (int i = 0; i < n; i++) {
        int currentPivot = pivo(i, n - 1);
        swap(pivotPositions, i, currentPivot);
    }

    for (int i = 0; i < n; i++) {
        // i-th pivot position 
        int ithPivotPos = pivotPositions[i];
        v[ithPivotPos] = i + 1;
    }

    delete[] pivotPositions;

    return true;
}

int* generateWorstCase(int n) {
    int *v = new int[n];
    gerar_pior_caso(v, n);
    return v;
}

// assume: start, end and pivotIndex are positions within array bounds;
template <typename T>
int partitionHoare(T *v, int start, int end, int pivotIndex) {
    if (start == end) {
        return start;
    }

    T pivot = v[pivotIndex];

    // put pivot at beginning of given slice
    swap(v, pivotIndex, start);

    // positions to the left of low are less than or equal to the pivot; positions to the right of high are greater than the pivot.
    int low = start;

    int high = end;

    while (low <= high) {
        // check if a swap is necessary
        // this check must NOT come after the 2 following if's, 
        // because the two if's mutate the loop condition, if this check came after the if's, we could enter this
        // branch in a situation where low > high. This detail was the cause of some bugs on this program.
        if (v[low] > pivot && v[high] <= pivot) {
            swap(v, low, high);
        }

        if (v[low] <= pivot) low++;        
        if (v[high] > pivot) high--;
    }

    int pivotPos = low - 1;

    swap(v, start, pivotPos);

    return pivotPos;
}

// assume: start, end are positions within array bounds;
// sort using some pivot implementation: random, fixed pivot, etc
template <typename T, int pivot(int, int)>
void quicksort(T *v, int start, int end) {
    if (start >= end) return;

    int pivotIndex = pivot(start, end);
    int pivotCorrectIndex = partitionHoare(v, start, end, pivotIndex);

    // pivot will be at pivotCorrectIndex, so:
    // left partition goes from start to pivotCorrectIndex - 1;
    // right partition goes from pivotCorrectIndex + 1 until end.
    quicksort<T, pivot>(v, start, pivotCorrectIndex - 1);
    quicksort<T, pivot>(v, pivotCorrectIndex + 1, end);
}

template <typename T>
void quicksort(T *v, int n) {
    quicksort<T, pivo>(v, 0, n - 1);
}

// +1 so we can cover all indexes between end and start. If we choose end - start only, we'll never get the "end" index.
int randomPivot(int start, int end) { return (rand() % (end - start + 1)) + start; }

template <typename T>
void quicksortRandomPivot(T *v, int n) {
    quicksort<T, randomPivot>(v, 0, n - 1);
}

// ========================================== QUICK SORT ==================================

// ========================================== INTRO SORT ==================================

template <typename T>
void insertionSort(T *v, int n) {
    for (int i = 1; i < n; i++) {
        for (int j = i; j > 0 && v[j] < v[j - 1]; j--) swap(v, j, j - 1);
    }
}


template <typename T, int pivot(int, int)>
void introsort(T *v, int start, int end, int depth) {
    if (start >= end) return;

    if (!depth) {
        // max depth reached. Revert to heapsort
        heapsort(v + start, end - start + 1);
        return;
    }

    int pivotIndex = pivot(start, end);
    int pivotCorrectIndex = partitionHoare(v, start, end, pivotIndex);

    // pivot will be at pivotCorrectIndex, so:
    // left partition goes from start to pivotCorrectIndex - 1;
    // right partition goes from pivotCorrectIndex + 1 until end.
    introsort<T, pivot>(v, start, pivotCorrectIndex - 1, depth - 1);
    introsort<T, pivot>(v, pivotCorrectIndex + 1, end, depth - 1);
}

template <typename T>
void introsort(T *v, int n) {
    if (n == 1) return;
    introsort<T, pivo>(v, 0, n - 1, 1.3 * log2(n));
}

template <typename T, int pivot(int, int), int INS_SORT_SIZE=16>
void introsortWithInsertion(T *v, int start, int end, int depth) {
    if (start >= end) return;

    int size = end - start + 1;

    if (size <= INS_SORT_SIZE) {
        insertionSort(v + start, size);
        return;
    }

    if (!depth) {
        // max depth reached. Revert to heapsort
        heapsort(v + start, size);
        return;
    }

    int pivotIndex = pivot(start, end);
    int pivotCorrectIndex = partitionHoare(v, start, end, pivotIndex);

    // pivot will be at pivotCorrectIndex, so:
    // left partition goes from start to pivotCorrectIndex - 1;
    // right partition goes from pivotCorrectIndex + 1 until end.
    introsortWithInsertion<T, pivot>(v, start, pivotCorrectIndex - 1, depth - 1);
    introsortWithInsertion<T, pivot>(v, pivotCorrectIndex + 1, end, depth - 1);
}

template <typename T>
void introsortWithInsertion(T *v, int n) {
    if (n == 1) return;
    introsortWithInsertion<T, pivo>(v, 0, n - 1, 1.3 * log2(n));
}

template <typename T, int pivot(int, int), int INS_SORT_SIZE=16>
void introsortChangePivot(T *v, int start, int end, int depth) {
    if (start >= end) return;

    int size = end - start + 1;

    if (size <= INS_SORT_SIZE) {
        insertionSort(v + start, size);
        return;
    }

    if (!depth) {
        // max depth reached. Change pivot to random.
        quicksortRandomPivot(v + start, size);
        return;
    }

    int pivotIndex = pivot(start, end);
    int pivotCorrectIndex = partitionHoare(v, start, end, pivotIndex);

    // pivot will be at pivotCorrectIndex, so:
    // left partition goes from start to pivotCorrectIndex - 1;
    // right partition goes from pivotCorrectIndex + 1 until end.
    introsortChangePivot<T, pivot>(v, start, pivotCorrectIndex - 1, depth - 1);
    introsortChangePivot<T, pivot>(v, pivotCorrectIndex + 1, end, depth - 1);
}

// Uses introsort with some kind of pivot until depth factor is reached 
// after depth is depleted, try quicksort again but with a random pivot, but with less chances
template <typename T>
void introsortChangePivot(T *v, int n) {
    if (n == 1) return;
    introsortChangePivot<T, pivo>(v, 0, n - 1, 1.3 * log2(n));
}

// ========================================== INTRO SORT ==================================

// ========================================== SIMULATION UTILS ==================================

double getSeconds(clock_t &time) { return time / (double) CLOCKS_PER_SEC; }

template <typename T, void sort (T*, int)>
clock_t sortAndMeasureTime(T *v, int n) {
    T *copy = makeCopy(v, n);

    clock_t i = clock();
    sort(copy, n);
    clock_t f = clock();

    ensureSorted(copy, n);
    delete[] copy;

    return f - i;
}

void displayInfo(int numInstances, int instanceSize) { cout << "Numero de instancias selecionado: " << numInstances << "\nTamanho de cada instancia: " << instanceSize << "\n"; }

template <int* generate (int)>
void generateAndRun(int numInstances, int instanceSize) {
    int NUM_ALGORITHMS = 6;
    clock_t* timePerAlgorithm = new clock_t[NUM_ALGORITHMS] { 0 };
    for (int i = 0; i < numInstances; i++) {
        int *instance = generate(instanceSize);
        timePerAlgorithm[0] += sortAndMeasureTime<int, heapsort>(instance, instanceSize);
        timePerAlgorithm[1] += sortAndMeasureTime<int, quicksort>(instance, instanceSize);
        timePerAlgorithm[2] += sortAndMeasureTime<int, quicksortRandomPivot>(instance, instanceSize);
        timePerAlgorithm[3] += sortAndMeasureTime<int, introsort>(instance, instanceSize);
        timePerAlgorithm[4] += sortAndMeasureTime<int, introsortWithInsertion>(instance, instanceSize);
        timePerAlgorithm[5] += sortAndMeasureTime<int, introsortChangePivot>(instance, instanceSize);
        delete[] instance;
    }

    cout << "Tempo por algoritmo: \n\n";
    cout << "\t\tHeapsort: " << getSeconds(timePerAlgorithm[0]) << " segundos\n";
    cout << "\t\tQuicksort (pivo fixo): " << getSeconds(timePerAlgorithm[1]) << " segundos\n";
    cout << "\t\tQuicksort (pivo aleatorio) " << getSeconds(timePerAlgorithm[2]) << " segundos\n";
    cout << "\t\tIntrosort (sem insertion sort): " << getSeconds(timePerAlgorithm[3]) << " segundos\n";
    cout << "\t\tIntrosort (com insertion sort): " << getSeconds(timePerAlgorithm[4]) << " segundos\n";
    cout << "\t\tQuicksort (troca pivo do quicksort + insertion sort): " << getSeconds(timePerAlgorithm[5]) << " segundos\n";
}

// ========================================== SIMULATION UTILS ==================================

int main(int argc, char **argv) {
    srand(42);

    if (argc < 4) {
        cerr << "Erro: Numero insuficiente de parametros para o programa!\n";
        cerr << "Os seguintes parametros devem ser informados: [Tipo de Instancia] [Tamanho do Vetor] [Numero de Instancias], conforme especificado.\n";
        return 1;
    }

    char instanceType = argv[1][0];
    int instanceSize = atoi(argv[2]);
    int numInstances = atoi(argv[3]);

    switch (instanceType) {
        case 'C':
            cout << "Tipo de instância selecionada: Instâncias Crescentes\n";
            displayInfo(numInstances, instanceSize);
            generateAndRun<increasing>(numInstances, instanceSize);
            break;
        case 'D':
            cout << "Tipo de instância selecionada: Instâncias Decrescentes\n";
            displayInfo(numInstances, instanceSize);
            generateAndRun<decreasing>(numInstances, instanceSize);
            break;
        case 'P':
            cout << "Tipo de instância selecionada: Instâncias de Pior Caso\n";
            displayInfo(numInstances, instanceSize);
            generateAndRun<generateWorstCase>(numInstances, instanceSize);
            break;
        default:
            cout << "Tipo de instância selecionada: Instâncias Aleatórias\n";
            displayInfo(numInstances, instanceSize);
            generateAndRun<randomNumbers>(numInstances, instanceSize);
            break;
    }
}