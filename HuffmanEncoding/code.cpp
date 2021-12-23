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
#include <bitset>

#include <math.h>
using std::min;

#include <cassert>

#include <iostream>
#include <fstream>
using std::ifstream;
using std::ofstream;
using std::ios;

bool debug = false;

// ========================================== MIN HEAP IMPLEMENTATION ==================================
template <typename T>
void swap(T *v, int i, int j) {
    T copy = v[i];
    v[i] = v[j];
    v[j] = copy;
}

int left_children(int i) { return 2*i + 1; }
int right_children(int i) { return 2*i + 2; }

template <typename T>
int smallest_within_bounds(T *v, int i, int left, int right, int n) {
    int smallest = i;

    if (left < n && v[left] < v[smallest]) {
        smallest = left;
    }

    if (right < n && v[right] < v[smallest]) {
        smallest = right;
    }

    return smallest;
}

template <typename T>
void go_down(T *v, int i, int n) {
    int left_child_index = left_children(i);
    int right_child_index = right_children(i);

    int smallest = smallest_within_bounds(v, i, left_child_index, right_child_index, n);

    if (smallest != i) {
        swap(v, smallest, i);
        go_down(v, smallest, n);
    }
}

template <typename T>
T remove_min(T *v, int n) {
    T min = v[0];

    v[0] = v[n - 1];
    go_down(v, 0, n - 1);

    return min;
}

template <typename T>
void make_heap(T *v, int n) {
    for (int i = n - 1; i >= 0; i--) {
        go_down(v, i, n);
    }
}

int parent(int i) {
    return (i - 1) / 2;
}

template <typename T>
void go_up(T *v, int i) {
    if (i == 0) return;

    int parent_index = parent(i);
    
    if (v[i] < v[parent_index]) {
        swap(v, i, parent_index);
        go_up(v, parent_index);
    }
}

template <typename T>
void heap_insert(T *v, T &data, int i) {
    v[i] = data;
    go_up(v, i);
}

// ========================================== MIN HEAP IMPLEMENTATION ==================================

using byte = unsigned char;

// huffman tree nodes
typedef struct Node {
    Node *left = nullptr;
    Node *right = nullptr;
    int frequency;
    byte value;

    Node(int n = 0): frequency(n) {
        left = nullptr;
        right = nullptr;
    }

    bool operator<(const Node &other) const {
        return frequency < other.frequency;
    }

    bool is_leaf() {
        return left == nullptr && right == nullptr;
    }
} Node;

// code representation;
// Since we're using int to hold the code, we end assuming here that any code produced will fit into a int.
typedef struct Code {
    int code;
    int num_bits;
} Code;

// precondition: byte frequency table can be indexed by the byte values that appear in text,
// so, byte_frequency_table should be big enough to index the highest byte that appears in text.
// we can achieve that by using a byte_frequency_table with 256 positions.

// writes distinct bytes into distinct_bytes, number of bytes read and fills byte_frequency table.
// returns: number of distinct values in the file
int compute_frequencies(ifstream &file, int *byte_frequency_table, byte *distinct_bytes, int &bytes_read) {
    int num_distinct_values = 0;

    byte data = file.get();

    while (not file.eof()) {
        bytes_read++;
        if (byte_frequency_table[data] == 0) {
            // found a new distinct value
            distinct_bytes[num_distinct_values] = data;
            num_distinct_values++;
        }

        byte_frequency_table[data] += 1;

        data = file.get();
    }

    return num_distinct_values;
}

// given a heap of Nodes, construct a huffman tree.
Node make_huffman_tree(Node *heap, int heap_size) {
    int current_heap_size = heap_size;

    while (current_heap_size > 1) {
        Node *x = new Node;
        Node *y = new Node;

        *x = remove_min(heap, current_heap_size);

        current_heap_size--;
        *y = remove_min(heap, current_heap_size);

        current_heap_size--;

        Node *z = new Node(x->frequency + y->frequency);

        z->left = x;
        z->right = y;

        heap_insert(heap, *z, current_heap_size);
        current_heap_size++;
    }

    return heap[0];
}

// does a in-order traversal of the huffman tree.
// when a leaf is found, we write the code into the code table
// this function assumes that any code for the given huffman tree can fit in a int
void compute_codes(Node *root, int code, int num_bits, Code* byte_to_code) {
    assert(root != nullptr);

    if (root->is_leaf()) {
        if (debug) {
            std::string binary = std::bitset<32>(code).to_string(); //to binary
            cout << root->value << ": " << root->frequency << " nbs: " << num_bits << " :  " << code << " :  " << binary << "\n";
        }

        byte_to_code[root->value].code = code;
        byte_to_code[root->value].num_bits = num_bits;
    } else {
        if (root->left != nullptr) compute_codes(root->left, code << 1, num_bits + 1, byte_to_code);
        if (root->right != nullptr) compute_codes(root->right, ((code << 1) | 0b1), num_bits + 1, byte_to_code);
    }
}

// decompress the file. Will read the file byte by byte, scanning every bit from the bytes for the codes
// after a leaf is reached, we return to the root
// the process ends when num_symbols are read, as expected (or if EOF happens first).
// PS: the reading process happens from most significant bits to less significant bits.
void decompress_iter(Node *root, ifstream &file, ofstream &output_file, int num_symbols) {
    byte current_byte = file.get();
    
    int symbols_to_read = num_symbols;
    Node *current_node = root;
    int byte_pos = 0;
    while (not file.eof() && symbols_to_read > 0) {
        if (current_node->is_leaf()) {
            output_file.put(current_node -> value);
            symbols_to_read--;
            current_node = root;
        }

        if (byte_pos == 8) {
            byte_pos = 0;
            current_byte = file.get();
        }

        int shift = 7 - byte_pos;
        int current_bit = (current_byte >> shift) & 0b1;
        
        if (current_bit) {
            current_node = current_node->right;
            byte_pos++;
        } else {
            current_node = current_node -> left;
            byte_pos++;
        }
    }

}

int make_mask(int n) {
    return (1 << n) - 1;
}

const int BYTE_SIZE = 8;
const int INT_SIZE = sizeof(int) * BYTE_SIZE;

void write_int_big_endian(ofstream &output_file, int data) {
    // most significant bits will come first - most significant byte will come first
    int mask = make_mask(BYTE_SIZE);
    byte b_1 = (data >> (BYTE_SIZE * 3)) & mask; // shift first 24 bits
    byte b_2 = (data >> (BYTE_SIZE * 2)) & mask; // shift first 16 bits 
    byte b_3 = (data >> (BYTE_SIZE * 1)) & mask; // shift first 8 bits
    byte b_4 = data & mask;

    output_file.put(b_1);
    output_file.put(b_2);
    output_file.put(b_3);
    output_file.put(b_4);
}

// writing data to the file. Here, i'll write data using ints, because it simplifies a bit of work
// when the current symbol can't fit into the current code, we just push it to the next int 
// and since we assume that a code can fit into a int (32 bits max code), we won't have to check
// if it indeed fits
// A problem emerges when we write ints to file: byte ordering.
// file.write((char*) &current_block, sizeof(int)) on my computer writes on Little Endian order,
// but the decompressor reads byte by byte, and it reads from most significant bits to less significant bits
// the decompressor could be adjusted to also use ints, but I decided to do it on the compressor, 
// using the write_int_big_endian function.
void compress_data(ifstream &input_file, ofstream &output_file, Code *byte_to_code) {
    int current_block = 0;
    int current_block_pos = 0;

    byte current_symbol = input_file.get();

    while (not input_file.eof()) {
        int remaining_bits = INT_SIZE - current_block_pos;
        
        Code code = byte_to_code[current_symbol];

        assert(code.num_bits <= INT_SIZE);
        
        int bits_to_write = min(remaining_bits, code.num_bits);

        if (debug) {
            cout << "writing: " << current_symbol << " - (" << std::bitset<32>(code.code).to_string() << ", " << code.num_bits << ')' << " before: " << std::bitset<32>(current_block).to_string();
            cout << "\n";
        }
        
        int code_to_write = code.code;

        // MSB will come first in the next byte
        if (bits_to_write < code.num_bits) {
            code_to_write >>= (code.num_bits - bits_to_write);
        }

        current_block = (current_block << bits_to_write) | (code_to_write & make_mask(bits_to_write));

        if (debug) {
            cout << "after: " << std::bitset<32>(current_block).to_string() << "\n";
        }

        current_block_pos += bits_to_write;
        remaining_bits -= bits_to_write;

        if (remaining_bits == 0) {
            // flush current byte to output; reset current byte.
            if (debug) cout << "flushing..\n";
            if (debug) cout << (int) current_block;
            // output_file.write((char*) &current_block, sizeof(int)); causes problems because of byte-ordering
            write_int_big_endian(output_file, current_block);
            current_block = 0;
            current_block_pos = 0;

            if (bits_to_write < code.num_bits) {
                // write remaining bits in next byte
                int extra_bits_count = code.num_bits - bits_to_write;

                // now we need to write remaining bits, which are the less significant bits
                // we accomplish this using a mask
                current_block = current_block | (code.code & make_mask(extra_bits_count));
                current_block_pos = extra_bits_count;
            }
        }

        // load next symbol
        current_symbol = input_file.get();
    }

    // flush last byte
    if (current_block_pos > 0) {
        int remaining_bits = INT_SIZE - current_block_pos;
        // moves written bits to most significant bits.
        current_block <<= remaining_bits;
        if (debug) cout << "final flush: " << std::bitset<32>(current_block).to_string() << "\n";

        write_int_big_endian(output_file, current_block);
    }
}

// num_symbols is redundant here. We could compute it using the frequency table which is also written to the file.
typedef struct Header {
    unsigned int num_symbols; 
    unsigned int distinct_bytes;

    Header() {}

    Header(int data_bytes, int distinct_bytes): num_symbols(data_bytes), 
        distinct_bytes(distinct_bytes) {}
} Header;

// run length encoding block - write frequency + value;
// also used to write frequency after the header.
typedef struct ByteFrequency {
    int frequency;
    byte value;
} ByteFrequency;

void write_header_with_heap(ofstream &file, Header &header, Node *heap) {
    file.write((char *) &header, sizeof(header));
    // heap entry for each distinct char
    // since at this moment we haven't built the tree, there are no pointers between nodes.
    // to optimize space, we won't write the heap as is, first, we'll transform it into a smaller type,
    // ByteFrequency, which will also be used in RLE encoding of files with 1 distinct symbol

    ByteFrequency *freqs = new ByteFrequency[header.num_symbols];

    for (unsigned int i = 0; i < header.distinct_bytes; i++) {
        freqs[i].frequency = heap[i].frequency;
        freqs[i].value = heap[i].value;
    }

    file.write((char *) freqs, sizeof(ByteFrequency) * header.distinct_bytes);
}

void write_header_with_rle(ofstream &file, Header &header, ByteFrequency &rle) {
    file.write((char *) &header, sizeof(header));
    file.write((char *) &rle, sizeof(rle));
}

void compress_file(ifstream &input_file, ofstream &output_file) {
    // 1: compute frequencies - if empty file, then just return and do nothing
    // if only 1 distinct symbol, then just write frequency of the symbol

    // 2: make a heap using the frequencies
    // 3: write header + heap with frequencies
    // 4: from heap, make a huffman tree
    // 5: compute codes from tree
    // 6: read file, and for each symbol on file, write given code on output file.
    int TABLE_SIZE = 256;
    int *byte_frequency_table = new int[TABLE_SIZE];
    byte *distinct_bytes = new byte[TABLE_SIZE];

    int bytes_read = 0;
    int num_distinct_bytes = compute_frequencies(input_file, byte_frequency_table, distinct_bytes, bytes_read);
    
    if (debug) cout << "Bytes read: " << bytes_read << "\n";

    if (bytes_read == 0) {
        // no work is needed: leave an empty output file.
        return;
    }

    Node *heap = new Node[num_distinct_bytes];

    if (debug) cout << "Num distinct: " << num_distinct_bytes << "\n";

    Header file_header(bytes_read, num_distinct_bytes);

    if (num_distinct_bytes == 1) {
        // use RLE encoding
        byte value = distinct_bytes[0];
        int frequency = byte_frequency_table[value];
        ByteFrequency rle_block = { frequency, value };
        write_header_with_rle(output_file, file_header, rle_block);
        return;
    }

    // populate heap from frequency
    for (int i = 0; i < num_distinct_bytes; i++) {
        if (debug) cout << distinct_bytes[i] << ": " << byte_frequency_table[distinct_bytes[i]] << "\n";
        byte value = distinct_bytes[i];
        int frequency = byte_frequency_table[value];

        heap[i].value = value;
        heap[i].frequency = frequency;
    }

    // from vector with frequencies (as Nodes), turn it into a heap, then save on output file.
    make_heap(heap, num_distinct_bytes);
    write_header_with_heap(output_file, file_header, heap);
    
    Node root = make_huffman_tree(heap, num_distinct_bytes);
    Code* byte_to_code = new Code[TABLE_SIZE];

    // fill byte to code table
    compute_codes(&root, 0, 0, byte_to_code);

    // rewind
    input_file.clear();
    input_file.seekg(0);

    // write compressed file
    compress_data(input_file, output_file, byte_to_code);
}

void decompress_file(ifstream &input_file, ofstream &output_file) {
    // if file is empty, then assume original file was also empty, do nothing.
    // 1 - read header
    // if only one distinct byte, then file was frequency-encoded, then we get the symbol+frequency
    // and write the symbol frequency times
    // 
    // 2 - from header, find how many ByteFrequency elements we need to read, then read them, 
    // 3 - convert byte frequency into Node's, because we need a Heap of Node's for the huffman tree
    // 4 - since byte frequency is stored a heap (it obeys the heap property), we can compute the huffman tree
    // 5 - use the huffman tree to decode the file.

    // check if file is empty
    input_file.get();

    if (input_file.eof()) {
        // empty file compressed, so, nothing needs to be written on output file.
        return;
    }
    
    // rewind file.
    input_file.clear();
    input_file.seekg(0);

    Header header;
    input_file.read((char*) &header, sizeof(header));   

    if (header.distinct_bytes == 1) {
        // file has been compressed with RLE, no huffman tree has been used.
        ByteFrequency rle;
        input_file.read((char*) &rle, sizeof(rle));   

        for (int i = 0; i < rle.frequency; i++) output_file.put(rle.value);
        return;
    } 

    if (debug) cout << "(" << header.num_symbols << " - " << header.distinct_bytes << ")\n";

    Node *heap = new Node[header.distinct_bytes];
    ByteFrequency *freqs = new ByteFrequency[header.distinct_bytes];

    // freqs is a heap written to the file, we write it as ByteFrequency to optimize space.
    // we then transform it back to Node, since we need a Heap of Nodes. ByteFrequency saves spaces
    // because it doesn't use pointers like Node does.
    input_file.read((char*) freqs, sizeof(ByteFrequency) * header.distinct_bytes);  

    for (unsigned int i = 0; i < header.distinct_bytes; i++) {
        heap[i].value = freqs[i].value;
        heap[i].frequency = freqs[i].frequency;
    } 

    Node root = make_huffman_tree(heap, header.distinct_bytes);
    decompress_iter(&root, input_file, output_file, header.num_symbols);
}

void handle_compress(char *input_file_name, char *output_file_name) {
    ifstream input_file;
    ofstream output_file;

    input_file.open(input_file_name, ios::binary);
    output_file.open(output_file_name, ios::binary);

    compress_file(input_file, output_file);
}

void handle_decompress(char *input_file_name, char *output_file_name) {
    ifstream input_file;
    ofstream output_file;

    input_file.open(input_file_name, ios::binary);
    output_file.open(output_file_name, ios::binary);

    decompress_file(input_file, output_file);
}

void show_menu() {
    cout << "Bem vindo ao compactador-do-Vinicius-v5000!\n";
    cout << "O programa aceita as seguintes opções: \n";
    cout << "-c <entrada> <saida> ou c <entrada> <saida> para comprimir arquivos\n";
    cout << "-d <entrada> <saida> ou d <entrada> <saida> para descomprimir arquivos\n";
    cout << "Onde <entrada> e <saida> são o nome dos arquivos.\n";

    cout << "\nExemplo: -d texto.txt texto_comprimido.bin\n";
    cout << "Exemplo: -c texto_comprimido.bin texto_descomprimido.txt\n";
}

double get_seconds(clock_t &time) { return time / (double) CLOCKS_PER_SEC; }

int main(int argc, char **argv) {
    if (argc < 4) {
        show_menu();
        return 0;
    }

    char mode = argv[1][0];

    if (mode == '-') mode = argv[1][1];

    clock_t i = clock();
    switch (mode) {
        case 'c':
            cout << "Comprimindo..\n";
            cout << "Entrada: " << argv[2];
            cout << "\nSaida: " << argv[3] << "\n";
            handle_compress(argv[2], argv[3]);
            break;
        case 'd':
            cout << "Descomprimindo..\n";
            cout << "Entrada: " << argv[2];
            cout << "\nSaida: " << argv[3] << "\n";
            handle_decompress(argv[2], argv[3]);
            break;
        default:
            cout << "Opção inválida!\n";
            show_menu();
            break;
    }

    clock_t f = clock();
    clock_t took = f - i;
    cout << "Sucesso!\n";
    cout << "Tempo de execução: " << get_seconds(took) << " segundos.\n";
}   