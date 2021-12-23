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

#include "instancias_Reais_Trabalho_2.hpp"

// ========================================== UTILS ==================================

int compute_size(const char *s) {
    int i = 0;
    for (const char *p = s; *p != '\0'; p++) i++;
    return i;
}

void ensure_equal_results(const int *a, const int *b) {
    for (; *a != -1; a++,b++) {
        if (*a != *b) {
            cerr << "Erro! As saidas dos algoritmos não são iguais!";
            throw std::invalid_argument("As saidas dos algoritmos não é igual!");
        }
    }

    if (*b != -1) {
        cerr << "Erro! As saidas dos algoritmos não são iguais!";
        throw std::invalid_argument("As saidas dos algoritmos não é igual!");
    }
}

// ========================================== UTILS ==================================

// ========================================== KMP SEARCH ==================================

int* make_prefix_table(const char *P, int n) {
    int *prefix_table = new int[n];

    if (prefix_table == nullptr) return nullptr;

    prefix_table[0] = -1;
    for (int i = 1; i < n; i++) {
        // compute prefix_table[i]
        int j = prefix_table[i - 1];

        while (P[i] != P[j + 1] && j != -1) {
            j = prefix_table[j];
        }

        // if next letter from the candidate prefix matches, then we increment j
        // otherwise, there was no possible candidate, so we write - 1 
        if (P[i] == P[j+1]) j++;

        prefix_table[i] = j;
    }

    return prefix_table;
}

bool kmp_search(const char *P, const char *T, int *O) {
    // since the text itself is expected to be much bigger than the pattern, 
    // we can spend some extra O(pattern_size) here;
    // a possible optimization would be to have a global prefix_table buffer, 
    // that we could just write (and grow it as necessary)
    // this way, we don't waste O(pattern_size) on every call to kmp_search
    int pattern_size = compute_size(P);
    const int *prefix_table = make_prefix_table(P, pattern_size);

    if (prefix_table == nullptr) return false;

    // point to the next writable position in output (matches) vector
    int *output_head = O;

    // current position of the pattern, we move it foward as the text matches the pattern
    // otherwise, we move it backwards, back to some previous position according to the prefix
    // function
    const char *pattern_head = P;
    const char *text_head = T;

    while(*text_head != '\0') {
        if (*pattern_head == *text_head) {
            // check if we reached the end of the pattern (next position => \0)
            // PS: the line below may do pattern_head = P - 1
            // but that's not a problem due to the increment right below, thus, we would get P - 1 + 1.
            if (pattern_head - P == pattern_size - 1) {
                *output_head = (text_head - T) - pattern_size + 1;
                output_head++;
                pattern_head = P + prefix_table[pattern_head - P];
            }

            text_head++, pattern_head++;
        } else {
            if (pattern_head == P) text_head++;
            else pattern_head = P + prefix_table[pattern_head - P - 1] + 1;

            // PS: at the else above, we will never access a negative index/get a negative pointer (a state where pattern_head < P)
            // at the else branch, pattern head - P will always be >= 1 (if it was 0, we would hit the if, 
            // and it can't be negative since no such assignment to pattern_head makes it smaller than P - the base pointer)
            // thus, even if we get a -1 from the table, the +1 cancels it.
        }
    }

    *output_head = -1;
    return true;
}

// ========================================== KMP SEARCH ==================================

// ========================================== BRUTE FORCE SEARCH ==================================

// check if there is a match of the pattern on the start of this string
// so, if we'd like to see if there's a match at some index of the text,
// we just have to call this function using string + index, pattern
bool has_match_beginning(const char *string, const char *pattern) {
    const char* string_head = string;
    const char* pattern_head = pattern;
    for (; *pattern_head != '\0'; pattern_head++, string_head++) {
        if (*string_head != *pattern_head) {
            return false;
        }
    }
    
    return true;
}

bool brute_search(const char *pattern, const char *string, int *output) {
    int *output_head = output;

    for (const char *text_head = string; *text_head != '\0'; text_head++) {
        if (has_match_beginning(text_head, pattern)) {
            *output_head = text_head - string;
            output_head++;
        }
    }

    *output_head = -1;
    return true; // always return true, since this function doesn't do any dynamic allocation (only automatic variables)
}

// ========================================== BRUTE FORCE SEARCH ==================================

// ========================================== SIMULATION UTILS ==================================

// throws exception if there is no ocurrence of the pattern starting at positions 
// at the result array
void ensure_pattern_ocurrences(const char *string, const char *pattern, const int *result) {
    for(const int* result_head = result; *result_head != -1; result_head++) {
        if (!has_match_beginning(string + *result_head, pattern)) {
            cerr << "Erro! Um dos indices informados no array de saída não corresponde a uma ocorrência do padrão!";
            throw std::invalid_argument("Um dos indices informados no array de saída não corresponde a uma ocorrência do padrão!");
        }
    }
}

const char MIN_CHAR = 'a';

// we write exactly total_size - 1 symbols, plus a \0 in the end.
void write_random_text(char* output_array, char max_char, int total_size) {
    int char_range = max_char - MIN_CHAR + 1;

    char* output_head = output_array;
    for (; output_head < output_array + total_size - 1; output_head++) {
        *output_head = MIN_CHAR + (rand() % char_range);
    }

    *output_head = '\0';
}

double get_seconds(clock_t &time) { return time / (double) CLOCKS_PER_SEC; }

template <bool search_algorithm (const char*, const char*, int*)>
clock_t search_and_measure_time(const char *pattern, const char *string, int *output) {
    clock_t i = clock();
    bool successful_alocations = search_algorithm(pattern, string, output);

    clock_t f = clock();

    if (!successful_alocations) {
        cerr << "Erro durante alocações na execução dos algoritmos. Abortando o programa.";
        throw std::invalid_argument("Falha durante alocações na execução do programa.");
    }

    return f - i;
}

void handle_random_instances(char max_char, int pattern_size, int text_size, int num_instances) {
    // assume: max_char is a char between 'a' and 'z' (inclusive)
    // pattern_size >= 1, text_size >= 1, num_instances >= 1
    cout << "Tipo de instância escolhida: Instâncias aleatórias\n";
    cout << "Caractere máximo (inclusivo): " << max_char << "\n";
    cout << "Tamanho do padrão: " << pattern_size << "\n";
    cout << "Tamanho do texto: " << text_size << "\n";
    cout << "Número de instâncias: " << num_instances << "\n\n";

    char* pattern_array = new char[pattern_size + 1];
    char* text_array = new char[text_size + 1];
    int* kmp_output = new int[text_size + 1];
    int* brute_force_output = new int[text_size + 1];

    clock_t kmp_time = 0;
    clock_t brute_force_time = 0;

    for (int i = 0; i < num_instances; i++) {
        write_random_text(pattern_array, max_char, pattern_size + 1);
        write_random_text(text_array, max_char, text_size + 1);

        kmp_time += search_and_measure_time<kmp_search>(pattern_array, text_array, kmp_output);
        brute_force_time += search_and_measure_time<brute_search>(pattern_array, text_array, brute_force_output);

        ensure_equal_results(brute_force_output, kmp_output);
        // since outputs are equal, we can pass brute_force_output or kmp_output here
        ensure_pattern_ocurrences(text_array, pattern_array, kmp_output);
    }

    cout << "Tempo por algoritmo: \n\n";
    cout << "\t\tKMP: " << get_seconds(kmp_time) << " segundos\n";
    cout << "\t\tForça Bruta: " << get_seconds(brute_force_time) << " segundos\n";

    delete[] pattern_array, delete[] text_array, delete[] kmp_output, delete[] brute_force_output;
}

void handle_real_instances(int x, int y) {
    cout << "Tipo de instância escolhida: Instâncias reais\n";
    cout << "X (inclusivo): " << x << "\n";
    cout << "Y (inclusivo): " << y << "\n";
    int text_size = compute_size(Texto_Livros);

    int* kmp_output = new int[text_size + 1];
    int* brute_force_output = new int[text_size + 1];
    clock_t kmp_time = 0;
    clock_t brute_force_time = 0;

    for (int i = x; i <= y; i++) {
        kmp_time += search_and_measure_time<kmp_search>(Padroes_Palavras[i], Texto_Livros, kmp_output);
        brute_force_time += search_and_measure_time<brute_search>(Padroes_Palavras[i], Texto_Livros, brute_force_output);

        ensure_equal_results(brute_force_output, kmp_output);
        // since outputs are equal, we can pass brute_force_output or kmp_output here
        ensure_pattern_ocurrences(Texto_Livros, Padroes_Palavras[i], brute_force_output);
    }

    cout << "Tempo por algoritmo: \n\n";
    cout << "\t\tKMP: " << get_seconds(kmp_time) << " segundos\n";
    cout << "\t\tForça Bruta: " << get_seconds(brute_force_time) << " segundos\n";

    delete[] kmp_output, delete[] brute_force_output;
}

// ========================================== SIMULATION UTILS ==================================

int main(int argc, char **argv) {
    // for simplicity, assume program was called with right parameters
    if (argc == 1) {
        cout << "Por favor, informe os parâmetros necessários para execução do programa.\n";
        return 0;
    }

    srand(42);

    char instance_type = argv[1][0];

    switch (instance_type) {
        case 'A': {
            char max_char = argv[2][0];
            int pattern_size = atoi(argv[3]);
            int text_size = atoi(argv[4]);
            int num_instances = atoi(argv[5]);
            handle_random_instances(max_char, pattern_size, text_size, num_instances);
            break;
        }

        default: {
            int x = atoi(argv[2]);
            int y = atoi(argv[3]);
            handle_real_instances(x, y);
            break;
        }
    }
}