#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
// Definindo os opcodes
#define PRINT 1
using namespace std;
#define HALT  0
#define LOOP 2

// Estrutura de instrução
typedef struct {
    int opcode;
    const char* arg;
    std::vector<std::string> val;
} Instruction;

void addInstruction(Instruction& inst, int opcode, std::string agr) {
    //inst.arg = agr;
    //inst.opcode = opcode;
    inst.val.push_back(agr);
}

// Função principl da VM
void run_vm(Instruction* program) {
    int pc = 0; // Program counter

    while (true) {
        Instruction instr = program[pc];
        switch (instr.opcode) {
            case PRINT:
                printf("%s", instr.arg);
                
                break;

            case HALT:
                return;

            case LOOP:
                return;
            default:
                printf("Unknown opcode at address: %d\n", instr.opcode);
                std::cout << instr.val[1] << std::endl;
                return;
        }
        pc++; // próxima instrução
    }
}

// Programa de exemplo
int main() {
    string formated_str = "\tHello World\n";
    Instruction arg;
    addInstruction(arg, PRINT, "Hello\n");
    addInstruction(arg, PRINT, "Hello\n");
    //addInstruction(arg, 1, NULL);
    run_vm(&arg);
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