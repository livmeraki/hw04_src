#include "vmsim_main.h"


int main(int argc, char *argv[]) 
{
    if (argc < 2 || argc > MAX_PROCESSES) 
    {
        fprintf(stderr, "Usage: %s <process image files... up to %d files>\n", 
                argv[0], MAX_PROCESSES);
        exit(EXIT_FAILURE);
    }

    // Initialize
    initialize();

    // Load process 
    for (int i = 1; i < argc; i++) {
        load(argv[i], i - 1);
    }

    // Execute process 
    simulate();

    // TODO: free memory - process list, frame, ...

    return 0;
}


// Initialization 
void initialize() 
{
    int i;

    // Physical memory 
    phy_memory = (char *) malloc(PHY_MEM_SIZE);

    // Register set 
    for (i = 0; i < MAX_REGISTERS; i++) {
        register_set[i] = 0;
    }

    // TODO: Initialize frame list, process list, ... 
}


// Load process from file 
void load(const char *filename, int pid) 
{  
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file");
        exit(EXIT_FAILURE);
    }

    // TODO: Create a process  
    Process *process = ... 
    
    // TODO: Create a page table of the process 
    
    // TODO: Clear temporary register set of process 
    
    // TODO: Load instructions into page 
    
    // Set PC 
    process->pc = 0x00000000;

    // TODO: Insert process into process list 
    
    fclose(file);
}


// Simulation 
void simulate() 
{
    int finish; 

    // TODO: Repeat simulation until the process list is empty 
    while (process list is not empty) {
        // TODO: Select a process from process list using round-robin scheme 

        // TODO: Execue a processs 
        finish = execute(process); 

        // TODO: If the process is terminated then 
        //       call print_register_set(), 
        //       reclaim allocated frames, 
        //       and remove process from process list 

        clock++; 

    }
}


// Execute an instruction using program counter 
int execute(Process *process) 
{
    char instruction[INSTRUCTION_SIZE];
    char opcode;

    // TODO: Restore register set 
    
    // TODO: Fetch instruction and update program counter      
    
    // Execute instruction according to opcode 
    opcode = instruction[0];
    switch (opcode) {
        case 'M': 
            op_move(process, instruction);   
            break;
        case 'A':
            op_add(process, instruction);
            break;
        case 'L':   
            op_load(process, instruction);
            break; 
        case 'S': 
            op_store(process, instruction);
            break;
        default: 
            printf("Unknown Opcode (%c) \n", opcode); 
    }

    // TODO: Store register set     

    // TODO: When the last instruction is executed return 1, otherwise return 0 
}


// Read up to 'count' bytes from the 'virt_addr' into 'buf' 
void read_page(Process *process, int virt_addr, void *buf, size_t count)
{
    // TODO: Get a physical address from virtual address 
    
    // TODO: handle page fault -> just allocate page 
    
    // TODO: Read up to 'count' bytes from the physical address into 'buf' 
}


// Write 'buf' up to 'count' bytes at the 'virt_addr' 
void write_page(Process *process, int virt_addr, const void *buf, size_t count)
{
    // TODO: Get a physical address from virtual address 
    
    // TODO: handle page fault -> just allocate page 
   
    // TODO: Write 'buf' up to 'count' bytes at the physical address 

}


// Print log with format string 
void print_log(int pid, const char *format, ...)
{
    va_list args; 
    va_start(args, format);
    printf("[Clock=%2d][PID=%d] ", clock, pid); 
    vprintf(format, args);
    printf("\n");
    fflush(stdout);
    va_end(args);
}


// Print values in the register set 
void print_register_set(int pid) 
{
    int i;
    char str[256], buf[16]; 
    strcpy(str, "[RegisterSet]:");
    for (i = 0; i < MAX_REGISTERS; i++) {
        sprintf(buf, " R[%d]=%d", i, register_set[i]); 
        strcat(str, buf);
        if (i != MAX_REGISTERS-1)
            strcat(str, ",");
    }
    print_log(pid, "%s", str);
}
