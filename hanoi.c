#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // para sleep() (ou windows.h para Sleep no Windows)
#include "hanoi.h"


// Variável global para o histórico das partidas
HistoricoPartida *historicoGlobal = NULL;
const char *NOME_ARQUIVO_HISTORICO = "historico_hanoi.txt";

// --- Funções de Jogo (Implementação das funções de hanoi.h) ---

// Implementação das funções de pilha (empilhar, desempilhar, topo, estacaEstaVazia)
// (As mesmas do seu código original, com pequenas adaptações como o retorno int para empilhar)
int empilhar(Disco **pilha, int tamanho) {
    Disco *novoDisco = malloc(sizeof(Disco));
    if (novoDisco == NULL) {
        printf("Erro ao Alocar Memoria!!\n");
        return -1; // Indica falha
    }
    novoDisco->tamanho = tamanho;
    novoDisco->prox = *pilha;
    *pilha = novoDisco;
    return 0; // Indica sucesso
}

int desempilhar(Disco **pilha) {
    if (*pilha == NULL) {
        // printf("Pilha Vazia"); // Removido para evitar spam em caso de pilha vazia
        return -1; // Retorna -1 para indicar pilha vazia ou erro
    }
    Disco *NoTemporario = *pilha;
    int valorDesempilhado = NoTemporario->tamanho;
    *pilha = NoTemporario->prox;
    free(NoTemporario);
    return valorDesempilhado;
}

int topo(Estaca *estaca) {
    return estaca->topo ? estaca->topo->tamanho : ESTACA_VAZIA;
}

int estacaEstaVazia(Estaca *estaca) {
    return estaca->topo == NULL;
}

// Implementação das funções de exibição e lógica do jogo (preencherMatriz, mostrarTorres, venceu, getEstaca)
void preencherMatriz(Estaca *estaca, char matriz[ALTURA_MAXIMA][20]) {
    for (int i = 0; i < ALTURA_MAXIMA; i++) {
        for (int j = 0; j < 9; j++) matriz[i][j] = ' ';
        matriz[i][9] = '\0';
    }

    Disco *atual = estaca->topo;
    int linha = ALTURA_MAXIMA - 1; // Começa de baixo para preencher a matriz
    // Primeiro, conte os discos para saber onde começar a preencher de baixo
    int count = 0;
    Disco *temp = estaca->topo;
    while(temp != NULL) {
        count++;
        temp = temp->prox;
    }
    linha = ALTURA_MAXIMA - count; // A linha onde o primeiro disco (topo) vai

    atual = estaca->topo; // Reinicia o ponteiro atual
    while (atual != NULL && linha < ALTURA_MAXIMA) {
        int tam = atual->tamanho;
        int espacos = 5 - tam; // 5 é a metade da largura máxima do disco (9) -1
        int i = 0;

        for (; i < espacos; i++) matriz[linha][i] = ' ';
        for (int j = 0; j < tam * 2 - 1; j++, i++) matriz[linha][i] = '#';
        for (; i < 9; i++) matriz[linha][i] = ' ';
        matriz[linha][i] = '\0';

        atual = atual->prox;
        linha++;
    }
}

void mostrarTorres(Estaca *A, Estaca *B, Estaca *C) {
    char matrizA[ALTURA_MAXIMA][20];
    char matrizB[ALTURA_MAXIMA][20];
    char matrizC[ALTURA_MAXIMA][20];

    preencherMatriz(A, matrizA);
    preencherMatriz(B, matrizB);
    preencherMatriz(C, matrizC);

    system("cls || clear"); // Limpa a tela (Windows || Unix/Linux)
    printf("\n");

    for (int i = 0; i < ALTURA_MAXIMA; i++) {
        printf("%s\t%s\t%s\n", matrizA[i], matrizB[i], matrizC[i]);
    }

    printf("---------\t---------\t---------\n");
    printf("    A    \t    B    \t    C    \n\n");
}

int venceu(Estaca *C, int total_discos) {
    int count = 0;
    Disco *atual = C->topo;
    while (atual) {
        count++;
        atual = atual->prox;
    }
    return count == total_discos;
}

Estaca* getEstaca(char c, Estaca *A, Estaca *B, Estaca *C) {
    if (c == 'A' || c == 'a') return A;
    if (c == 'B' || c == 'b') return B;
    if (c == 'C' || c == 'c') return C;
    return NULL;
}

// Função para liberar a memória dos discos em uma estaca
void liberarDiscosEstaca(Estaca *estaca) {
    Disco *atual = estaca->topo;
    Disco *proximo;
    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    estaca->topo = NULL; // Garante que o topo da estaca é NULL após a liberação
}

// --- Funções de Histórico (Implementação das funções de historico.h) ---

// Gera a string de data e hora atual
void gerarDataHora(char *buffer, int bufferSize) {
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", info);
}

void adicionarHistorico(HistoricoPartida **lista, int movimentos, const char *nome, int discos) {
    HistoricoPartida *novoRegistro = malloc(sizeof(HistoricoPartida));
    if (novoRegistro == NULL) {
        printf("Erro ao alocar memoria para o historico!\n");
        return;
    }

    novoRegistro->movimentos = movimentos;
    strncpy(novoRegistro->nomeJogador, nome, sizeof(novoRegistro->nomeJogador) - 1);
    novoRegistro->nomeJogador[sizeof(novoRegistro->nomeJogador) - 1] = '\0'; // Garante null-termination
    novoRegistro->numDiscos = discos;
    gerarDataHora(novoRegistro->dataHora, sizeof(novoRegistro->dataHora));
    
    // Adiciona no início da lista
    novoRegistro->prox = *lista;
    *lista = novoRegistro;
}

void exibirHistorico(HistoricoPartida *lista) {
    system("cls || clear");
    printf("--- HISTORICO DE PARTIDAS ---\n");
    if (lista == NULL) {
        printf("Nenhum historico disponivel.\n");
        return;
    }
    HistoricoPartida *atual = lista;
    while (atual != NULL) {
        printf("-----------------------------\n");
        printf("Jogador: %s\n", atual->nomeJogador);
        printf("Discos: %d\n", atual->numDiscos);
        printf("Movimentos: %d\n", atual->movimentos);
        printf("Data/Hora: %s\n", atual->dataHora);
        atual = atual->prox;
    }
    printf("-----------------------------\n");
    printf("Pressione ENTER para voltar ao menu...");
    getchar(); // Consome o \n pendente
    getchar(); // Espera por uma nova entrada
}

void buscarHistoricoPorNome(HistoricoPartida *lista, const char *nome) {
    system("cls || clear");
    printf("--- BUSCAR PARTIDA POR NOME ---\n");
    if (lista == NULL) {
        printf("Nenhum historico disponivel para busca.\n");
        printf("Pressione ENTER para voltar ao menu...");
        getchar(); getchar();
        return;
    }

    int encontrado = 0;
    HistoricoPartida *atual = lista;
    while (atual != NULL) {
        // strstr para buscar substring, strcasecmp para ignorar maiusculas/minusculas
        if (strstr(atual->nomeJogador, nome) != NULL) {
            printf("-----------------------------\n");
            printf("Jogador: %s\n", atual->nomeJogador);
            printf("Discos: %d\n", atual->numDiscos);
            printf("Movimentos: %d\n", atual->movimentos);
            printf("Data/Hora: %s\n", atual->dataHora);
            encontrado = 1;
        }
        atual = atual->prox;
    }
    if (!encontrado) {
        printf("Nenhuma partida encontrada para o jogador '%s'.\n", nome);
    }
    printf("-----------------------------\n");
    printf("Pressione ENTER para voltar ao menu...");
    getchar(); getchar();
}

void buscarHistoricoPorData(HistoricoPartida *lista, const char *data) {
    system("cls || clear");
    printf("--- BUSCAR PARTIDA POR DATA ---\n");
    if (lista == NULL) {
        printf("Nenhum historico disponivel para busca.\n");
        printf("Pressione ENTER para voltar ao menu...");
        getchar(); getchar();
        return;
    }

    int encontrado = 0;
    HistoricoPartida *atual = lista;
    while (atual != NULL) {
        // Compara apenas a parte da data (os 10 primeiros caracteres "YYYY-MM-DD")
        if (strncmp(atual->dataHora, data, 10) == 0) {
            printf("-----------------------------\n");
            printf("Jogador: %s\n", atual->nomeJogador);
            printf("Discos: %d\n", atual->numDiscos);
            printf("Movimentos: %d\n", atual->movimentos);
            printf("Data/Hora: %s\n", atual->dataHora);
            encontrado = 1;
        }
        atual = atual->prox;
    }
    if (!encontrado) {
        printf("Nenhuma partida encontrada para a data '%s'.\n", data);
    }
    printf("-----------------------------\n");
    printf("Pressione ENTER para voltar ao menu...");
    getchar(); getchar();
}

void salvarHistoricoEmArquivo(HistoricoPartida *lista, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "w");
    if (fp == NULL) {
        printf("Erro ao abrir arquivo para salvar historico!\n");
        return;
    }

    HistoricoPartida *atual = lista;
    while (atual != NULL) {
        fprintf(fp, "%d;%s;%d;%s\n", atual->movimentos, atual->nomeJogador, atual->numDiscos, atual->dataHora);
        atual = atual->prox;
    }
    fclose(fp);
}

void carregarHistoricoDoArquivo(HistoricoPartida **lista, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        // Arquivo não existe ou não pode ser aberto, é normal na primeira execução
        printf("Arquivo de historico nao encontrado ou erro ao abrir. Iniciando com historico vazio.\n");
        return;
    }

    char linha[256];
    int movimentos, numDiscos;
    char nomeJogador[50];
    char dataHora[20];

    // Libera qualquer histórico existente na memória antes de carregar
    liberarHistorico(lista);

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        // Usa sscanf para parsear a linha. O formato é "movimentos;nome;discos;dataHora"
        if (sscanf(linha, "%d;%49[^;];%d;%19[^\n]", &movimentos, nomeJogador, &numDiscos, dataHora) == 4) {
            // Cria um novo nó e preenche manualmente (para manter a ordem do arquivo)
            HistoricoPartida *novoRegistro = malloc(sizeof(HistoricoPartida));
            if (novoRegistro == NULL) {
                printf("Erro ao alocar memoria ao carregar historico!\n");
                fclose(fp);
                return;
            }
            novoRegistro->movimentos = movimentos;
            strcpy(novoRegistro->nomeJogador, nomeJogador);
            novoRegistro->numDiscos = numDiscos;
            strcpy(novoRegistro->dataHora, dataHora);
            novoRegistro->prox = NULL;

            // Adiciona no final da lista para manter a ordem do arquivo
            if (*lista == NULL) {
                *lista = novoRegistro;
            } else {
                HistoricoPartida *temp = *lista;
                while (temp->prox != NULL) {
                    temp = temp->prox;
                }
                temp->prox = novoRegistro;
            }
        }
    }
    fclose(fp);
}

// Libera toda a memória da lista encadeada de histórico
void liberarHistorico(HistoricoPartida **lista) {
    HistoricoPartida *atual = *lista;
    HistoricoPartida *proximo;
    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    *lista = NULL; // Garante que o ponteiro da lista é NULL após a liberação
}


// --- Funções de Lógica do Jogo ---

void jogarHanoi() {
    Estaca A = {NULL, 'A'};
    Estaca B = {NULL, 'B'};
    Estaca C = {NULL, 'C'};
    int movimentos = 0;
    char nomeJogador[50];

    system("cls");
    printf("--- NOVO JOGO DE TORRE DE HANOI ---\n");
    printf("Digite seu nome de jogador: ");
    scanf("%49s", nomeJogador); // Limita a leitura para evitar buffer overflow
    // Limpa o buffer de entrada
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    int n;
    do {
        printf("Quantos discos (1 a %d)? ", MAX_DISCOS);
        scanf("%d", &n);
        while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer
    } while (n < 1 || n > MAX_DISCOS);

    for (int i = n; i >= 1; i--) {
        if (empilhar(&A.topo, i) != 0) {
            printf("Falha critica: nao foi possivel alocar discos. Encerrando o jogo.\n");
            liberarDiscosEstaca(&A); liberarDiscosEstaca(&B); liberarDiscosEstaca(&C);
            return;
        }
    }

    char jogada[3];
    while (1) {
        mostrarTorres(&A, &B, &C);
        printf("Movimentos: %d\n", movimentos);
        printf("Digite a jogada (ex: AB para mover de A para B, ou 'sair' para desistir): ");
        scanf("%2s", jogada);
        while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer

        if (strcmp(jogada, "sair") == 0) {
            printf("Voce desistiu do jogo.\n");
            liberarDiscosEstaca(&A); liberarDiscosEstaca(&B); liberarDiscosEstaca(&C);
            sleep(2);
            return;
        }

        if (strlen(jogada) != 2) {
            printf("Entrada invalida! Formato esperado: 'XY'\n");
            sleep(1);
            continue;
        }

        Estaca *origem = getEstaca(jogada[0], &A, &B, &C);
        Estaca *destino = getEstaca(jogada[1], &A, &B, &C);

        if (!origem || !destino || origem == destino) {
            printf("Estacas de origem/destino invalidas ou iguais!\n");
            sleep(1);
            continue;
        }

        if (estacaEstaVazia(origem)) {
            printf("A estaca de origem esta vazia!\n");
            sleep(1);
            continue;
        }

        int discoOrigemTam = topo(origem);
        int discoDestinoTam = topo(destino);

        if (discoDestinoTam == ESTACA_VAZIA || discoOrigemTam < discoDestinoTam) {
            int movido = desempilhar(&origem->topo);
            if (movido == -1) { // Erro ao desempilhar (nao deveria acontecer aqui se a origem nao esta vazia)
                printf("Erro interno ao desempilhar.\n");
                sleep(1);
                continue;
            }
            if (empilhar(&destino->topo, movido) != 0) {
                printf("Falha critica: nao foi possivel empilhar o disco. Jogo interrompido.\n");
                liberarDiscosEstaca(&A); liberarDiscosEstaca(&B); liberarDiscosEstaca(&C);
                return;
            }
            movimentos++;
        } else {
            printf("Movimento invalido: nao pode colocar disco maior sobre menor!\n");
            sleep(1);
        }

        if (venceu(&C, n)) {
            mostrarTorres(&A, &B, &C);
            printf("Parabens %s! Voce venceu em %d movimentos!\n", nomeJogador, movimentos);
            adicionarHistorico(&historicoGlobal, movimentos, nomeJogador, n);
            salvarHistoricoEmArquivo(historicoGlobal, NOME_ARQUIVO_HISTORICO);
            printf("Historico salvo.\n");
            liberarDiscosEstaca(&A); liberarDiscosEstaca(&B); liberarDiscosEstaca(&C);
            sleep(3);
            break;
        }
    }
}

// --- Menu Principal ---
void exibirMenu() {
    system("cls || clear");
    printf("--- SIMULADOR TORRE DE HANOI ---\n");
    printf("1. Novo Jogo\n");
    printf("2. Historico de Partidas\n");
    printf("3. Buscar por Partida (Nome)\n");
    printf("4. Buscar por Partida (Data)\n");
    printf("5. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    // Carrega o histórico ao iniciar o programa
    carregarHistoricoDoArquivo(&historicoGlobal, NOME_ARQUIVO_HISTORICO);

    int escolha;
    char termoBusca[50];
    int c; // Para limpar o buffer

    do {
        exibirMenu();
        if (scanf("%d", &escolha) != 1) { // Lê a escolha e verifica se é um número
            printf("Entrada invalida. Por favor, digite um numero.\n");
            while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer de entrada
            sleep(1);
            continue;
        }
        while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer de entrada

        switch (escolha) {
            case 1:
                jogarHanoi();
                break;
            case 2:
                exibirHistorico(historicoGlobal);
                break;
            case 3:
                printf("Digite o nome do jogador para buscar: ");
                scanf("%49s", termoBusca);
                while ((c = getchar()) != '\n' && c != EOF);
                buscarHistoricoPorNome(historicoGlobal, termoBusca);
                break;
            case 4:
                printf("Digite a data para buscar (YYYY-MM-DD): ");
                scanf("%10s", termoBusca);
                while ((c = getchar()) != '\n' && c != EOF);
                buscarHistoricoPorData(historicoGlobal, termoBusca);
                break;
            case 5:
                printf("Saindo do simulador. Ate mais!\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                sleep(1);
                break;
        }
        sleep(1); // Pequena pausa para o usuário ver a mensagem antes do menu limpar
    } while (escolha != 5);

    // Salva o histórico final antes de sair (caso alguma partida tenha sido jogada e não salva)
    salvarHistoricoEmArquivo(historicoGlobal, NOME_ARQUIVO_HISTORICO);
    // Libera toda a memória alocada para o histórico
    liberarHistorico(&historicoGlobal);

    return 0;
}