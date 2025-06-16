#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hanoi.h"

HistoricoPartida *historicoGlobal = NULL;
const char *NOME_ARQUIVO_HISTORICO = "historico_hanoi.txt";

int empilhar(Disco **pilha, int tamanho) {
    Disco *novoDisco = malloc(sizeof(Disco));
    if (novoDisco == NULL) {
        printf("Erro ao Alocar Memoria!!\n");
        return -1;
    }
    novoDisco->tamanho = tamanho;
    novoDisco->prox = *pilha;
    *pilha = novoDisco;
    return 0;
}

int desempilhar(Disco **pilha) {
    if (*pilha == NULL) {
        return -1;
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

void preencherMatriz(Estaca *estaca, char matriz[ALTURA_MAXIMA][20]) {
    for (int i = 0; i < ALTURA_MAXIMA; i++) {
        for (int j = 0; j < 9; j++) matriz[i][j] = ' ';
        matriz[i][9] = '\0';
    }

    Disco *atual = estaca->topo;
    int count = 0;
    Disco *temp = estaca->topo;
    while(temp != NULL) {
        count++;
        temp = temp->prox;
    }
    int linha = ALTURA_MAXIMA - count;

    atual = estaca->topo;
    while (atual != NULL && linha < ALTURA_MAXIMA) {
        int tam = atual->tamanho;
        int espacos = 5 - tam;
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

    system("cls");
    printf("\n");

    for (int i = 0; i < ALTURA_MAXIMA; i++) {
        printf("%s\t%s\t%s\n", matrizA[i], matrizB[i], matrizC[i]);
    }

    printf("---------\t---------\t---------\n");
    printf("     A   \t     B   \t     C   \n\n");
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

void liberarDiscosEstaca(Estaca *estaca) {
    Disco *atual = estaca->topo;
    Disco *proximo;
    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    estaca->topo = NULL;
}

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
    novoRegistro->nomeJogador[sizeof(novoRegistro->nomeJogador) - 1] = '\0';
    novoRegistro->numDiscos = discos;
    gerarDataHora(novoRegistro->dataHora, sizeof(novoRegistro->dataHora));

    novoRegistro->prox = *lista;
    *lista = novoRegistro;
}

void exibirHistorico(HistoricoPartida *lista) {
    system("cls || clear");
    printf("--- HISTORICO DE PARTIDAS ---\n");
    if (lista == NULL) {
        printf("Nenhum historico disponivel.\n");
        printf("Pressione ENTER para voltar ao menu...");
        getchar();
        getchar();
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
    getchar();
    getchar();
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
        printf("Arquivo de historico nao encontrado ou erro ao abrir. Iniciando com historico vazio.\n");
        return;
    }

    char linha[256];
    int movimentos, numDiscos;
    char nomeJogador[50];
    char dataHora[20];

    liberarHistorico(lista);

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        if (sscanf(linha, "%d;%49[^;];%d;%19[^\n]", &movimentos, nomeJogador, &numDiscos, dataHora) == 4) {
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

void liberarHistorico(HistoricoPartida **lista) {
    HistoricoPartida *atual = *lista;
    HistoricoPartida *proximo;
    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    *lista = NULL;
}


void jogarHanoi() {
    Estaca A = {NULL, 'A'};
    Estaca B = {NULL, 'B'};
    Estaca C = {NULL, 'C'};
    int movimentos = 0;
    char nomeJogador[50];

    system("cls || clear");
    printf("--- NOVO JOGO DE TORRE DE HANOI ---\n");
    printf("Digite seu nome de jogador: ");
    scanf("%49s", nomeJogador);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    int n;
    do {
        printf("Quantos discos (1 a %d)? ", MAX_DISCOS);
        scanf("%d", &n);
        while ((c = getchar()) != '\n' && c != EOF);
    } while (n < 1 || n > MAX_DISCOS);

    for (int i = n; i >= 1; i--) {
        if (empilhar(&A.topo, i) != 0) {
            printf("Falha critica: nao foi possivel alocar discos. Encerrando o jogo.\n");
            liberarDiscosEstaca(&A); liberarDiscosEstaca(&B); liberarDiscosEstaca(&C);
            return;
        }
    }

    char jogada[5];
    while (1) {
        mostrarTorres(&A, &B, &C);
        printf("Movimentos: %d\n", movimentos);
        printf("Digite a jogada (ex: AB para mover de A para B, ou 'sair' para desistir): ");
        scanf("%4s", jogada);
        while ((c = getchar()) != '\n' && c != EOF);

        if (strcmp(jogada, "sair") == 0) {
            printf("Voce desistiu do jogo.\n");
            liberarDiscosEstaca(&A); liberarDiscosEstaca(&B); liberarDiscosEstaca(&C);
            sleep(2);
            return;
        }

        if (strlen(jogada) != 2) {
            printf("Entrada invalida! Formato esperado: 'XY' para movimentos.\n");
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
            if (movido == -1) {
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
    carregarHistoricoDoArquivo(&historicoGlobal, NOME_ARQUIVO_HISTORICO);

    int escolha;
    char termoBusca[50];
    int c;

    do {
        exibirMenu();
        if (scanf("%d", &escolha) != 1) {
            printf("Entrada invalida. Por favor, digite um numero.\n");
            while ((c = getchar()) != '\n' && c != EOF);
            sleep(1);
            continue;
        }
        while ((c = getchar()) != '\n' && c != EOF);

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
    } while (escolha != 5);

    salvarHistoricoEmArquivo(historicoGlobal, NOME_ARQUIVO_HISTORICO);
    liberarHistorico(&historicoGlobal);

    return 0;
}
