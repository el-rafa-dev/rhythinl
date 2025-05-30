#include <iostream>

int main() {
    std::cout << nullptr << std::endl;
    return 0;
}

/* codigo comentado 
#include <stdio.h>  // Biblioteca para entrada/saída (ex: printf)

// Definimos alguns códigos de operação (opcodes)
#define PRINT 1     // Imprime uma string
#define HALT  0     // Encerra a execução da VM

// Estrutura de uma instrução na nossa VM
typedef struct {
    int opcode;         // O tipo da instrução (ex: PRINT, HALT)
    const char* arg;    // O argumento da instrução (ex: a string a ser impressa)
} Instruction;

// Função que executa o programa da VM
void run_vm(Instruction* program) {
    int pc = 0; // Program Counter: índice da instrução atual

    // Loop principal da VM
    while (1) {
        Instruction instr = program[pc]; // Busca a instrução atual

        switch (instr.opcode) {
            case PRINT: // Se for uma instrução PRINT
                printf("%s\n", instr.arg); // Imprime o argumento
                break;

            case HALT: // Se for uma instrução HALT
                return; // Encerra a execução da VM

            default: // Se o opcode for desconhecido
                printf("Unknown opcode: %d\n", instr.opcode);
                return;
        }

        pc++; // Avança para a próxima instrução
    }
}

// Função principal do programa (entrypoint)
int main() {
    // Programa que será executado pela VM (array de instruções)
    Instruction program[] = {
        {PRINT, "Olá, mundo!"},
        {PRINT, "Essa é minha VM simples."},
        {HALT,  NULL} // Encerra o programa
    };

    run_vm(program); // Executa o programa
    return 0;
}*/