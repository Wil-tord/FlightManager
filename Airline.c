#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Struct para Passenger Details
typedef struct {
    char cpf[15];
    char nome[20];
    char sobrenome[20];
    int dia, mes, ano;
    char numeroVoo[6];
    char numAssento[4];
    char classe[11];
    char origem[4];
    char destino[4];
    float Valor;
} Passageiro;

typedef struct {
    int qtdAssentos;
    int assentosEcon;
    int assentosExec;
    float valEcon;
    float valExec;
    bool vooAberto;
    float totalFechamento;
} VooState;

void abrirVoo(VooState *state);
void realizarReserva(Passageiro **passageiro, int *reservas, VooState *state);
void consultarReserva(Passageiro *passageiro, int reservas);
void modificarReserva(Passageiro *passageiro, int reservas);
void cancelarReserva(Passageiro **passageiro, int *reservas, VooState *state);
void fecharDia(int reservas, VooState *state);
void fecharVoo(Passageiro *passageiro, int reservas, float totalFechamento, VooState *state);
void escolheClasse(char *classe, VooState *state);
void salvarReservas(Passageiro *passageiro, int reservas);
void carregarReservas(Passageiro **passageiro, int *reservas);
void salvarEstadoVoo(VooState *state);
void carregarEstadoVoo(VooState *state);
void printHifens();

float calcularTotalFechamento(Passageiro *passageiros, int reservas, VooState *state);

int main(void) {
    int reservas = 0; // Contadores para realizar operacoes

    // Allocacao Inicial do Struct Passageiro
    Passageiro *passageiro = (Passageiro*)malloc(sizeof(Passageiro));
    if (passageiro == NULL) {
        printf("Sem Memoria");
        exit(1);
    }

    VooState state = {0};
    carregarEstadoVoo(&state);  // Load existing flight state from file if available
    carregarReservas(&passageiro, &reservas);  // Load existing reservations from file if available

    char comando[3]; // Comandos Inicias no loop infinito

    while (true) {
        scanf("%2s", comando);

        getchar(); // Clear the buffer to handle stray characters

        if (strcmp(comando, "AV") == 0) {
            if (state.vooAberto) {
            } else {
                abrirVoo(&state);
                state.vooAberto = true;
                salvarEstadoVoo(&state);  // Save state after opening the flight
            }
        } else if (strcmp(comando, "RR") == 0) {
            if (state.vooAberto) {
                realizarReserva(&passageiro, &reservas, &state);
                salvarReservas(passageiro, reservas);  // Save reservations to file after each new reservation

                // Calculate total value after each reservation
                state.totalFechamento = calcularTotalFechamento(passageiro, reservas, &state);
                salvarEstadoVoo(&state);  // Save state after reservation

                // Check if reservations reach plane capacity
                if (reservas == state.qtdAssentos) {
                    fecharVoo(passageiro, reservas, state.totalFechamento, &state);
                    state.vooAberto = false;
                    salvarEstadoVoo(&state);  // Save state after closing the flight
                }

            } else { 
                fecharVoo(passageiro, reservas, state.totalFechamento, &state);
            }

        } else if (strcmp(comando, "CR") == 0) {
            consultarReserva(passageiro, reservas);
        } else if (strcmp(comando, "MR") == 0) {
            if (state.vooAberto) {
                modificarReserva(passageiro, reservas);
                salvarReservas(passageiro, reservas);  // Save reservations to file after modification
                state.totalFechamento = calcularTotalFechamento(passageiro, reservas, &state);
                salvarEstadoVoo(&state);  // Save state after modification
            } else {
                fecharVoo(passageiro, reservas, state.totalFechamento, &state);
            }
        } else if (strcmp(comando, "CA") == 0) {
            if (state.vooAberto) {
                cancelarReserva(&passageiro, &reservas, &state);
                salvarReservas(passageiro, reservas);  // Save reservations to file after cancellation
                state.totalFechamento = calcularTotalFechamento(passageiro, reservas, &state);
                salvarEstadoVoo(&state);  // Save state after cancellation
            } else {
                fecharVoo(passageiro, reservas, state.totalFechamento, &state);
            }
        } else if (strcmp(comando, "FD") == 0) {
            fecharDia(reservas, &state);
            break;
        } else {
            if (strcmp(comando, "FV") == 0) {
            fecharVoo(passageiro, reservas, state.totalFechamento, &state);
            state.vooAberto = false;
            salvarEstadoVoo(&state);  // Save state after closing the flight
            } 
            break;
        }
    }

    free(passageiro);
    return 0;
}

// Abertura de Voo
void abrirVoo(VooState *state) {
    scanf("%d", &state->qtdAssentos);
    scanf("%f", &state->valEcon);
    scanf("%f", &state->valExec);

}

// Realizar Reserva
void realizarReserva(Passageiro **passageiro, int *reservas, VooState *state) {
    *passageiro = (Passageiro*)realloc(*passageiro, (*reservas + 1) * sizeof(Passageiro));

    // Esse aqui e como acessar a posisao do struct na mem, pq funciona como um array...
    Passageiro *novoPassageiro = &(*passageiro)[*reservas];

    scanf(" %19s", novoPassageiro->nome);
    scanf(" %19s", novoPassageiro->sobrenome);
    scanf(" %14s", novoPassageiro->cpf);
    scanf("%d %d %d", &novoPassageiro->dia, &novoPassageiro->mes, &novoPassageiro->ano);
    scanf(" %5s", novoPassageiro->numeroVoo);
    scanf(" %3s", novoPassageiro->numAssento);
    scanf(" %10s", novoPassageiro->classe);
    scanf("%f", &novoPassageiro->Valor);
    scanf(" %3s", novoPassageiro->origem);
    scanf(" %3s", novoPassageiro->destino);

    // Call escolheClasse based on the class chosen
    escolheClasse(novoPassageiro->classe, state);
    (*reservas)++; // Contador do Passageiros.
}

// Função para escolher classe e incrementar contadores
void escolheClasse(char *classe, VooState *state) {
    if (strcmp(classe, "economica") == 0) {
        state->assentosEcon++;
    } else if (strcmp(classe, "executiva") == 0) {
        state->assentosExec++;
    }
}

// Modificar Reserva
void modificarReserva(Passageiro *passageiro, int reservas) {
    char cpfModificar[15];
    scanf("%14s", cpfModificar);

    // Procura pelo CPF na lista de reservas
    for (int i = 0; i < reservas; i++) {
        if (strcmp(passageiro[i].cpf, cpfModificar) == 0) {
            scanf(" %19s", passageiro[i].nome);
            scanf(" %19s", passageiro[i].sobrenome);
            scanf(" %14s", passageiro[i].cpf);
            scanf(" %3s", passageiro[i].numAssento);

            printf("\nReserva Modificada:");
            printf("%s\n", passageiro[i].cpf);
            printf("%s %s\n", passageiro[i].nome, passageiro[i].sobrenome);
            printf("%02d/%02d/%02d\n", passageiro[i].dia, passageiro[i].mes, passageiro[i].ano);
            printf("Voo: %s\n", passageiro[i].numeroVoo);
            printf("Assento: %s\n", passageiro[i].numAssento);
            printf("Classe: %s\n", passageiro[i].classe);
            printf("Trencho: %s %s\n", passageiro[i].origem, passageiro[i].destino);
            printf("Valor: %.2f\n", passageiro[i].Valor);

            printHifens(); // Call to printHifens function
            break;
        }
    }
}


void cancelarReserva(Passageiro **passageiro, int *reservas, VooState *state) {
    char cpfCancelar[15];
    scanf("%14s", cpfCancelar);

    for (int i = 0; i < *reservas; i++) {
        if (strcmp((*passageiro)[i].cpf, cpfCancelar) == 0) {
            // Adjust the total value based on the class of the canceled reservation
            if (strcmp((*passageiro)[i].classe, "economica") == 0) {
                state->assentosEcon--;
            } else if (strcmp((*passageiro)[i].classe, "executiva") == 0) {
                state->assentosExec--;
            }

            for (int j = i; j < *reservas - 1; j++) {
                (*passageiro)[j] = (*passageiro)[j + 1];
            }

            (*reservas)--; // Reduz o número de reservas

            *passageiro = (Passageiro*)realloc(*passageiro, (*reservas) * sizeof(Passageiro));
            break;
        }
    }
}


void consultarReserva(Passageiro *passageiro, int reservas) {
    char cpfConsulta[15];
    scanf("%14s", cpfConsulta);

    for (int i = 0; i < reservas; i++) {
        if (strcmp(passageiro[i].cpf, cpfConsulta) == 0) {
            printf("%s\n", passageiro[i].cpf);
            printf("%s %s\n", passageiro[i].nome, passageiro[i].sobrenome);
            printf("%02d/%02d/%02d\n", passageiro[i].dia, passageiro[i].mes, passageiro[i].ano);
            printf("Voo: %s\n", passageiro[i].numeroVoo);
            printf("Assento: %s\n", passageiro[i].numAssento);
            printf("Classe: %s\n", passageiro[i].classe);
            printf("Trencho: %s %s\n", passageiro[i].origem, passageiro[i].destino);
            printf("Valor: %.2f\n", passageiro[i].Valor);

            printHifens(); // Call to printHifens function
            break;
        }
    }
}

void fecharDia(int reservas, VooState *state) {
    float fechamentoEcon = state->assentosEcon * state->valEcon;
    float fechamentoExec = state->assentosExec * state->valExec;
    float fechamentoTotal = fechamentoEcon + fechamentoExec;

    printf("\nFechamento do dia:\n");
    printf("Numero de Reservas: %d\n", reservas);
    printf("Posicao: %.2f\n", fechamentoTotal);

    printHifens(); // Call to printHifens function
}

// Fechar Voo
void fecharVoo(Passageiro *passageiro, int reservas, float totalFechamento, VooState *state) {
    printf("\nVoo Fechado!\n");
    for (int i = 0; i < reservas; i++) {    
        printf("\n%s\n", passageiro[i].cpf);
        printf("%s %s\n", passageiro[i].nome, passageiro[i].sobrenome);
        printf("%s\n", passageiro[i].numAssento);
    }
    printf("\nValor Total: %.2f\n", totalFechamento);

    printHifens(); // Call to printHifens function
}

void salvarReservas(Passageiro *passageiro, int reservas) {
    FILE *file = fopen("reservas.txt", "wb");
    if (file != NULL) {
        fwrite(&reservas, sizeof(int), 1, file);
        fwrite(passageiro, sizeof(Passageiro), reservas, file);
        fclose(file);
    }
}

void carregarReservas(Passageiro **passageiro, int *reservas) {
    FILE *file = fopen("reservas.txt", "rb");
    if (file != NULL) {
        fread(reservas, sizeof(int), 1, file);
        *passageiro = (Passageiro*)realloc(*passageiro, (*reservas) * sizeof(Passageiro));
        fread(*passageiro, sizeof(Passageiro), *reservas, file);
        fclose(file);
    }
}

void salvarEstadoVoo(VooState *state) {
    FILE *file = fopen("estadoVoo.txt", "wb");
    if (file != NULL) {
        fwrite(state, sizeof(VooState), 1, file);
        fclose(file);
    }
}

void carregarEstadoVoo(VooState *state) {
    FILE *file = fopen("estadoVoo.txt", "rb");
    if (file != NULL) {
        fread(state, sizeof(VooState), 1, file);
        fclose(file);
    }
}

void printHifens() {
    printf("--------------------------------------------------\n");
}

float calcularTotalFechamento(Passageiro *passageiros, int reservas, VooState *state) {
    float total = 0.0;
    for (int i = 0; i < reservas; i++) {
        if (strcmp(passageiros[i].classe, "economica") == 0) {
            total += state->valEcon;
        } else if (strcmp(passageiros[i].classe, "executiva") == 0) {
            total += state->valExec;
        }
    }
    return total;
}
