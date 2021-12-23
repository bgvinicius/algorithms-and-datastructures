// Nome: Vinicius Bernardo Gabriel
// Matrı́cula: 475210
// -----------------------------------------------------------------------------
// Universidade Federal do Ceará, Centro de Ciências, Departamento de Computação
// Disciplina: Laboratório de Programação - CK0215 2021.1 T01A
// Professor: Pablo Mayckon Silva Farias
// Trabalho 4
// Interface para a Implementação de Dicionário baseado em Árvore AVL.
// Autor: Pablo Mayckon Silva Farias, em 18/08/2021.
// -----------------------------------------------------------------------------


// Tudo o que está presente na interface abaixo deve ser mantido como está,
// pois é essa interface que será utilizada no programa do professor para a
// realização dos testes.
//
// A realização do trabalho consiste portanto em *completar* a implementação
// abaixo, tornando-a funcional e atendendo aos requisitos deste trabalho.

// Imports abaixos usados para debugging: na função percursoEmOrdem
// #include <iostream>
// using std::cout;
// #include <cassert>

template <typename TC, typename TV>
class DicioAVL
  {
   public: // ------------------------------------------------------------------

   // Representação Interna da Árvore ------------------------------------------

   // Numa implementação típica de dicionário,
   // a representação interna normalmente fica inacessível ao usuário;
   // neste trabalho, porém,
   // o professor precisa avaliar se a árvore está correta,
   // e por isso a estrutura do nó é exposta ao usuário ("public"):
   

   struct Noh
     {
      TC chave;  TV valor;

      Noh *esquerdo = nullptr;
      Noh *direito = nullptr;
      Noh *pai = nullptr;
      int altura = 1;

      Noh(TC chave, TV valor): chave(chave), valor(valor) {}
      Noh(TC chave, TV valor, Noh *pai): chave(chave), valor(valor), pai(pai) {}

      // Você deve completar a estrutura do nó com:
      //
      // 1. Ponteiros para filho esquerdo, filho direito e pai.
      //
      // 2. Se necessário, algum campo adicional que viabilize a implementação
      //    da árvore AVL, como a altura do nó, a informação do balanceamento
      //    (-1, 0, +1), etc.
      //
      // Você é livre para escolher como vai representar os elementos acima,
      // mas deve implementar as funções abaixo,
      // que permitirão que a estrutura da árvore seja percorrida
      // (quando o filho direito / filho esquerdo / pai não existir,
      //  a respectiva função deve retornar nullptr):

      /* Deve retornar ponteiro para filho direito.  */
      Noh* obter_dir() { return direito; }

      /* Deve retornar ponteiro para filho esquerdo. */
      Noh* obter_esq() {  return esquerdo;  }
      /* Deve retornar ponteiro para o pai.          */
      Noh* obter_pai() {  return pai; }

      bool eh_folha()  { return esquerdo == nullptr && direito == nullptr; }
     }; // Noh  ----------------------------------------------------------------

   Noh* obter_raiz () { /* Deve retornar um ponteiro para a raiz,
                           ou nullptr, se a árvore estiver vazia. */ 
                        return this->raiz;     
                      }
    
    static Noh* minimo(Noh *arvore) {
      if (arvore == nullptr) return nullptr;

      Noh *min = arvore;
      while (min->esquerdo != nullptr) min = min->esquerdo;
      return min;
    }

    static Noh* sucessor(Noh *no) {
      if (no->direito != nullptr) {
        return minimo(no->direito);
      }

      Noh *corrente = no;
      Noh *pai = no->pai; 

      while (pai != nullptr && corrente == pai->direito) {
        corrente = pai;
        pai = pai->pai; 
      }
      
      return pai;
    }

   private: // -----------------------------------------------------------------

   // Você pode incluir campos e métodos que importem para a implementação
   // mas que fiquem inacessíveis ao usuário da classe DicioAVL ("private").
    Noh* raiz = nullptr;

    bool eh_filho_esquerdo(Noh *x) { 
      return x -> pai -> esquerdo == x;
    }

    // ao realizar rotações e mudar a raiz da arvore, corrigimos o ponteiro do pai para a nova raiz.
    // assume que novo_filho ja aponta para o seu respectivo pai, mas o pai ainda aponta para o filho antigo.
    void corrige_pai_raiz(Noh *filho_antigo, Noh *novo_filho) {
      Noh *pai = novo_filho -> pai;

      if (pai == nullptr) {
        raiz = novo_filho;
        return;
      }

      bool filho_esquerdo = pai -> esquerdo == filho_antigo;

      if (filho_esquerdo) {
        pai -> esquerdo = novo_filho; 
        return;
      }

      pai -> direito = novo_filho;
    }

    // evita repetir o check de nullptr em todos os cantos
    // assume-se que novo_pai != nullptr
    void corrige_pai(Noh *candidato, Noh *novo_pai) {
      if (candidato != nullptr) {
        candidato -> pai = novo_pai;
      }
    }

    void corrige_altura(Noh *x) {
      if (x == nullptr) return;

      Noh *esq = x -> esquerdo;
      Noh *dir = x -> direito;

      int altura_esq = esq == nullptr ? 0 : esq -> altura;
      int altura_dir = dir == nullptr ? 0 : dir -> altura;

      int nova_altura = altura_esq > altura_dir ? altura_esq : altura_dir;

      x -> altura = 1 + nova_altura;
    }

    // assume que x != nullptr
    int calcula_balanco(Noh *x) {
      Noh *esq = x -> esquerdo;
      Noh *dir = x -> direito;

      int altura_esq = esq == nullptr ? 0 : esq -> altura;
      int altura_dir = dir == nullptr ? 0 : dir -> altura;

      return altura_dir - altura_esq;
    }

    int h(Noh *x) {
      if (x == nullptr) return 0;
      return x -> altura;
    }
    
    void rotacao_esquerda_simples(Noh *x) {
      // considera arvore enraizada em x; ao final do processo, nova raiz torna-se nova_raiz
      Noh *nova_raiz = x -> direito;
      Noh *aux = nova_raiz-> esquerdo;
      x -> direito = aux;
      nova_raiz->esquerdo = x;

      nova_raiz->pai = x->pai;
      x -> pai = nova_raiz;

      corrige_pai(aux, x);
      corrige_pai_raiz(x, nova_raiz);

      // corrige alturas
      corrige_altura(x);
      corrige_altura(nova_raiz);
    }

    void rotacao_direita_simples(Noh *x) {
      // considera arvore enraizada em x; ao final do processo, nova raiz torna-se nova_raiz
      Noh *nova_raiz = x -> esquerdo; 
      Noh *aux = nova_raiz-> direito; // T2
      x -> esquerdo = aux;
      nova_raiz -> direito = x;

      // corrigindo ponteiro para pais e de pais para filhos
      nova_raiz->pai = x->pai;
      x -> pai = nova_raiz;

      corrige_pai(aux, x);
      corrige_pai_raiz(x, nova_raiz);

      // corrige alturas
      corrige_altura(x);
      corrige_altura(nova_raiz);
    }

    void rotacao_esquerda_dupla(Noh *x) {
      Noh *p = x;
      Noh *z = p -> direito;
      Noh *y = z -> esquerdo;

      Noh *t2 = y -> esquerdo;
      Noh *t3 = y -> direito;

      p -> direito = t2;
      z -> esquerdo = t3;
      y -> esquerdo = p;
      y -> direito = z;

      y->pai = x->pai;

      corrige_pai(t2, p);
      corrige_pai(t3, z);
      corrige_pai(p, y);
      corrige_pai(z, y);
      corrige_pai_raiz(x, y);

      corrige_altura(p);
      corrige_altura(z);
      corrige_altura(y);
    }

    void rotacao_direita_dupla(Noh *x) {
      Noh *p = x;
      Noh *u = p -> esquerdo;
      Noh *v = u -> direito;

      Noh *t2 = v -> esquerdo;
      Noh *t3 = v -> direito;

      u -> direito = t2; 
      p -> esquerdo = t3;
      v -> esquerdo = u;
      v -> direito = p;

      v -> pai = x -> pai;

      corrige_pai(t2, u);
      corrige_pai(t3, p);
      corrige_pai(u, v);
      corrige_pai(p, v);
      corrige_pai_raiz(x, v);

      corrige_altura(u);
      corrige_altura(p);
      corrige_altura(v);
    }

    // remove no folha f;
    Noh* remove_folha(Noh *f) {
      Noh *pai = f -> pai;
      if (pai == nullptr){
        raiz = nullptr;
        return nullptr;
      }

      if (eh_filho_esquerdo(f)) {
        pai -> esquerdo = nullptr;
      } else {
        pai -> direito = nullptr;
      }

      return pai;
    }

    // remove no x que contem um unico filho z
    Noh* remove_unico_filho(Noh *x) {
      Noh *filho = x -> esquerdo;

      if (filho == nullptr) filho = x -> direito;

      Noh *pai = x -> pai;
      if (pai == nullptr) {
        filho -> pai = nullptr;
        raiz = filho;
        return nullptr;
      }

      if (eh_filho_esquerdo(x)) {
        pai -> esquerdo = filho;
      } else {
        pai -> direito = filho;
      }

      filho -> pai = pai;
      return pai;
    }

    Noh *remove_dois_filhos(Noh *x) {
      Noh *pai = x -> pai;
      Noh *aux = sucessor(x);

      // se x é filho imediato de x: x -> direito = aux, troque x por aux na arvore;
      if (aux == x -> direito) {
        // aux pega o esquerdo de x, aux aponta pro seu novo pai, pai antigo de x aponta para aux agora;
        aux -> esquerdo = x -> esquerdo;
        corrige_pai(x -> esquerdo, aux);
        aux -> pai = pai;
        corrige_pai_raiz(x, aux);
        return aux;
      } else {
        Noh *pai_aux = aux -> pai; // nao pode ser nulo

        // aux era filho esquerdo (por ser o sucessor); 
        pai_aux -> esquerdo = aux -> direito;
        corrige_pai(aux -> direito, pai_aux);

        aux -> esquerdo = x -> esquerdo;
        aux -> direito = x -> direito;

        aux -> pai = pai;

        corrige_pai(x -> esquerdo, aux);
        corrige_pai(x -> direito, aux);
        corrige_pai_raiz(x, aux);
        return pai_aux;
      }
    }

  void desalocaArvoreEmOrdem(Noh *raiz) {
    if (raiz == nullptr) return;

    desalocaArvoreEmOrdem(raiz -> esquerdo);
    Noh *direito = raiz -> direito;
    delete raiz;
    desalocaArvoreEmOrdem(direito);
  }

   public: // ------------------------------------------------------------------

   // Tudo o que está abaixo deve ser mantido público em DicioAVL,
   // pois será utilizado nos testes do professor.

   // Operações de Dicionário: -------------------------------------------------

   DicioAVL () { /* Deve criar um dicionário vazio. */ }

   ~DicioAVL () { /* Deve desalocar toda a memória dinamicamente alocada
                   * pelo dicionário (no caso, basicamente a árvore AVL). */ 
                desalocaArvoreEmOrdem(raiz);
                }

   class Iterador
     {
      private: // --------------------------------------------------------------

        Noh* atual = nullptr;
      // Você pode incluir campos e métodos que importem para a implementação
      // mas que fiquem inacessíveis ao usuário da classe Iterador.

      // Caso você queira dar a DicioAVL acesso aos membros privados de
      // Iterador, sem que esses membros sejam acessíveis em outros contextos,
      // basta descomentar a linha abaixo:
      friend DicioAVL;

      public: // ---------------------------------------------------------------

      Iterador(Noh *elemento): atual(elemento) { }

      // Tudo o que está abaixo deve ser mantido público em Iterador,
      // pois será utilizado nos testes do professor.

      // Os operadores "!=" e "==" abaixo podem ser chamados mesmo caso
      // o iterador esteja no "fim" do dicionário ("posição" em que o iterador
      // não se refere a nenhum elemento propriamente dito).

      bool operator != (Iterador j) { /* Deve retornar "true" se e somente se
                                         o iterador atual e "j" NÃO SE REFEREM
                                         ao mesmo elemento ou posição do
                                         dicionário. */ 
                                    return this->atual != j.atual;
                                    }

      bool operator == (Iterador j) { /* Deve retornar "true" se e somente se
                                         o iterador atual e "j" SE REFEREM
                                         ao mesmo elemento ou posição do
                                         dicionário. */ 
                                    return this->atual == j.atual;

                                         }

      // Os métodos abaixo possuem como PRÉ-CONDIÇÃO o fato de que o iterador
      // aponta para um elemento propriamente dito do dicionário
      // (será responsabilidade do usuário garantir que esse será o caso,
      //  e portanto que os métodos abaixo NÃO SERÃO CHAMADOS caso o iterador
      //  aponte para o "fim" do dicionário).
      // Portanto, esses métodos NÃO PRECISAM TESTAR se o iterador aponta
      // para o "fim" do dicionário.

      TC chave () { 
        return this->atual->chave;
        /* Deve retornar a chave do elemento em questão. */ 
      }

      /* Deve retornar o valor do elemento em questão. */
      TV valor () { 
        return this->atual->valor;
       }

      bool acabou() {
        return this->atual == nullptr;
      }

      /* Deve fazer o iterador passar ao próximo elemento do dicionário
      * (isto é, ao elemento da CHAVE SUCESSORA, na ordem crescente das
      * chaves), ou então ao "fim" do dicionário, caso não haja um
      * próximo elemento. */
      void operator ++ () {
          if (acabou()) return;

          if (atual->direito) {
            atual = minimo(atual->direito);
            return;
          }

          Noh *corrente = atual;
          Noh *pai = atual->pai; 

          while (pai != nullptr && corrente == pai->direito) {
            corrente = pai;
            pai = pai->pai; 
          }
          
          this->atual = pai;
        }

     }; // Iterador ------------------------------------------------------------

   Iterador inicio () { /* Deve retornar um iterador para o elemento de menor
                         * chave, caso exista um, ou então um iterador para o
                         * "fim", se o dicionário estiver vazio. */
                        return Iterador(minimo(this->raiz));
                       }

  /* Deve retornar um iterador num estado especial,
  * que seja diferente do estado de um iterador que aponte
  * para um elemento propriamente dito, e que dessa forma
  * sirva para caracterizar a situação em que o iterador
  * tenha atingido o "fim" do dicionário. */ 
  Iterador fim () {
    return Iterador(nullptr); 
  }

  bool vazia() {
    return this->raiz == nullptr;
  }

  // precondicao: atual deve ser o no "candidato" a ser pai do novo no
  // portanto, assume-se que atual != nullptr e pertence a arvore
  // equivalente a uma metodo iterativo, se o compilador tiver tail-call-optimization, pode ser rapido
  // PS: não é dever essa função corrigir as alturas (poderia ser, mas optei por separar os concerns)
  Noh* insereDesbalanceadoRecursivo(TC c, TC v, Noh *atual) {  
    Noh **proximo = &(atual->esquerdo);

    // se c deve estar na direita do atual, corrige o proximo
    if (c > atual->chave) proximo = &(atual -> direito);

    // se o proximo é nulo, então achamos a posição do novo nó
    if (*proximo == nullptr) {
      Noh *novo = new Noh(c, v, atual);
      *proximo = novo;
      return novo;
    }

    // proximo ainda não é nulo, portanto, continuamos descendo na arvore;
    return insereDesbalanceadoRecursivo(c, v, *proximo);
  }



  //  ----------- Debugging  -----------

  // void assertPai(Noh *x, bool esq) {
  //   if (x == nullptr || x -> pai == nullptr) return;

  //   if (esq) {
  //     assert(((x -> pai) -> esquerdo) == x);
  //   } else {
  //     assert(((x -> pai) -> direito) == x);
  //   }
  // }
  // void emOrdem(Noh *r) {
  //   if (r == nullptr) return;
    
  //   emOrdem(r -> esquerdo);
  //   assertPai(r -> esquerdo, true);
  //   cout << "(" << (r -> chave) << " -> " << (r -> valor) << ", h = " << (r -> altura) << ")\n";
  //   emOrdem(r -> direito);
  //   assertPai(r -> direito, false);
  // }

  // void emOrdem() { 
  //   cout << "raiz: " << raiz -> chave << "\n";  
  //   emOrdem(this->raiz); 
  // }

  //  ----------- Debugging  -----------

  /* Deve inserir a chave "c" e o valor "v" no dicionário, partindo da
    * PRÉ-CONDIÇÃO de que a chave "c" não existe no dicionário -- ou seja,
    * a função não precisa nem deve fazer esse teste.
    *
    * Em caso de falha de alocação de memória, deve retornar um iterador
    * para o "fim".
    *
    * ATENÇÃO: o iterador retornado deve continuar válido e relativo ao par
    * -------  (c,v) enquanto esse par não for removido do dicionário.
    *
    * A observação acima é particularmente importante para o caso da REMOÇÃO
    * em que o nó do par a ser removido possui dois filhos. Nesse caso, não
    * basta apenas copiar os campos do nó sucessor para o nó em questão, pois
    * isso faria com que o par sucessor mudasse de nó, invalidando algum
    * iterador relativo a esse par; portanto, nesse caso da remoção, é
    * necessário que o nó do sucessor realmente ocupe o lugar da árvore que
    * estava sendo ocupado pelo nó a ser removido. */
  Iterador inserir (TC c, TV v) {
    if (vazia()) {
      this->raiz = new Noh(c, v);
      return Iterador(this->raiz);
    }

    Noh *novo = insereDesbalanceadoRecursivo(c, v, this->raiz);

    for (Noh *pai = novo -> pai; pai != nullptr; pai = (pai -> pai)) { 
      corrige_altura(pai); 

      int b = calcula_balanco(pai);

      if (b == 2) {
        Noh *z = pai -> direito;
        if (h(z -> direito) > h(z -> esquerdo)) {
          rotacao_esquerda_simples(pai);
        } else {
          rotacao_esquerda_dupla(pai);
        }
      } else if (b == -2) {
        Noh *u = pai -> esquerdo;

        if (h(u -> esquerdo) > h(u -> direito)) {
          rotacao_direita_simples(pai);
        } else {
          rotacao_direita_dupla(pai);
        }
      }
    }

    return Iterador(novo);
  }

    Iterador buscaRecursiva(TC &c, Noh *atual) {
      if (atual == nullptr) return fim();
      
      if (atual->chave == c) return Iterador(atual);

      if (c < atual->chave) return buscaRecursiva(c, atual->esquerdo);

      return buscaRecursiva(c, atual->direito);
    }


  /* Deve retornar um iterador para o elemento de chave "c", caso essa
    * chave exista no dicionário; caso a chave não esteja presente, deve
    * ser retornado um iterador para o "fim". */
    Iterador buscar (TC c) {
      return buscaRecursiva(c, this->raiz);
    }

    /* Se o iterador apontar para o "fim", então a função deve simplesmente
      * retornar, deixando o dicionário inalterado. Em caso contrário, então
      * é PRÉ-CONDIÇÃO da função que o iterador estará apontando para algum
      * elemento do dicionário, o qual deverá ser removido. */

   void remover (Iterador i) {
     if (i.acabou()) return;

     Noh *no = i.atual;
     Noh *inicio_correcao;

     if (no->eh_folha()) {
       inicio_correcao = remove_folha(no);
     } else {
       if (no->esquerdo != nullptr && no->direito != nullptr) {
         inicio_correcao = remove_dois_filhos(no);
       } else {
         inicio_correcao = remove_unico_filho(no);
       }
     }
  
      for (Noh *pai = inicio_correcao; pai != nullptr; pai = (pai -> pai)) { 
        corrige_altura(pai); 

        int b_pai = calcula_balanco(pai);

        if (b_pai == 2) {
          int b = calcula_balanco(pai -> direito);
          if (b == 1 || b == 0) {
            rotacao_esquerda_simples(pai);
          } else {
            rotacao_esquerda_dupla(pai);
          }
        } else if (b_pai == -2) {
          int b = calcula_balanco(pai -> esquerdo);

          if (b == -1 || b == 0) {
            rotacao_direita_simples(pai);
          } else {
            rotacao_direita_dupla(pai);
          }
        }
      }

    }

  }; // DicioAVL  --------------------------------------------------------------
