#ifndef HANOI_GAME_H
#define HANOI_GAME_H

#include <time.h>

#define MAX_DISCOS 5
#define ALTURA_MAXIMA MAX_DISCOS
#define ESTACA_VAZIA 999

typedef struct Disco {
    int tamanho;
    struct Disco *prox;
} Disco;

typedef struct {
    Disco *topo;
    char nome;
} Estaca;

int empilhar(Disco **pilha, int tamanho);
int desempilhar(Disco **pilha);
int topo(Estaca *estaca);
int estacaEstaVazia(Estaca *estaca);
void liberarDiscosEstaca(Estaca *estaca);

void preencherMatriz(Estaca *estaca, char matriz[ALTURA_MAXIMA][20]);
void mostrarTorres(Estaca *A, Estaca *B, Estaca *C);
int venceu(Estaca *C, int total_discos);
Estaca* getEstaca(char c, Estaca *A, Estaca *B, Estaca *C);
void jogarHanoi();

typedef struct HistoricoPartida {
    int movimentos;
    char nomeJogador[50];
    int numDiscos;
    char dataHora[20];
    struct HistoricoPartida *prox;
} HistoricoPartida;

void gerarDataHora(char *buffer, int bufferSize);
void adicionarHistorico(HistoricoPartida **lista, int movimentos, const char *nome, int discos);
void exibirHistorico(HistoricoPartida *lista);
void buscarHistoricoPorNome(HistoricoPartida *lista, const char *nome);
void buscarHistoricoPorData(HistoricoPartida *lista, const char *data);
void salvarHistoricoEmArquivo(HistoricoPartida *lista, const char *nomeArquivo);
void carregarHistoricoDoArquivo(HistoricoPartida **lista, const char *nomeArquivo);
void liberarHistorico(HistoricoPartida **lista);

void exibirMenu();

#endif
