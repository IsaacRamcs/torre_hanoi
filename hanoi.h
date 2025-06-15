#ifndef HANOI_GAME_H
#define HANOI_GAME_H

#include <time.h> // Necessário para a parte de histórico (time_t, struct tm)

// --- Constantes Globais ---
#define MAX_DISCOS 5
#define ALTURA_MAXIMA MAX_DISCOS
#define ESTACA_VAZIA 999 // Valor sentinela para estaca vazia

// --- Estruturas de Dados do Jogo de Hanói ---
typedef struct Disco {
    int tamanho;
    struct Disco *prox;
} Disco;

typedef struct {
    Disco *topo;
    char nome;
} Estaca;

// --- Funções de Manipulação da Pilha (Hanói) ---
int empilhar(Disco **pilha, int tamanho);
int desempilhar(Disco **pilha);
int topo(Estaca *estaca);
int estacaEstaVazia(Estaca *estaca);
void liberarDiscosEstaca(Estaca *estaca); // Para liberar a memória dos discos de uma estaca

// --- Funções de Exibição e Lógica do Jogo de Hanói ---
void preencherMatriz(Estaca *estaca, char matriz[ALTURA_MAXIMA][20]);
void mostrarTorres(Estaca *A, Estaca *B, Estaca *C);
int venceu(Estaca *C, int total_discos);
Estaca* getEstaca(char c, Estaca *A, Estaca *B, Estaca *C);
void jogarHanoi(); // Função principal para iniciar um novo jogo

// --- Estrutura de Dados do Histórico de Partidas ---
typedef struct HistoricoPartida {
    int movimentos;
    char nomeJogador[50];
    int numDiscos;
    char dataHora[20]; // Formato "YYYY-MM-DD HH:MM:SS"
    struct HistoricoPartida *prox;
} HistoricoPartida;

// --- Funções de Manipulação do Histórico ---
void gerarDataHora(char *buffer, int bufferSize);
void adicionarHistorico(HistoricoPartida **lista, int movimentos, const char *nome, int discos);
void exibirHistorico(HistoricoPartida *lista);
void buscarHistoricoPorNome(HistoricoPartida *lista, const char *nome);
void buscarHistoricoPorData(HistoricoPartida *lista, const char *data);
void salvarHistoricoEmArquivo(HistoricoPartida *lista, const char *nomeArquivo);
void carregarHistoricoDoArquivo(HistoricoPartida **lista, const char *nomeArquivo);
void liberarHistorico(HistoricoPartida **lista); // Função para liberar a memória da lista de histórico

// --- Funções do Menu Principal ---
void exibirMenu();


#endif // HANOI_GAME_H