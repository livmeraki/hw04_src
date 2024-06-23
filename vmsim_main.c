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
    free(phy_memory);
    for (int i = 0; i < process_count; i++) {
        free(process_list[i].page_table);
    }

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
    for (i = 0; i < NUM_PAGES; i++) {
        frame_list[i] = -1;  // -1 indicates the frame is free
    }
    process_count = 0;
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
    Process *process = &process_list[pid]; 
    process->pid = pid;
    process->pc = 0x00000000;

    fscanf(file, "%d %d", &process->size, &process->num_inst);

    // TODO: Create a page table of the process 
    process->page_table = (PageTableEntry *) malloc(sizeof(PageTableEntry) * (process->size / PAGE_SIZE));
    for (int i = 0; i < (process->size / PAGE_SIZE); i++) {
        process->page_table[i].frame_number = -1;
        process->page_table[i].valid = 0;
    }

    // TODO: Clear temporary register set of process 
    for (int i = 0; i < MAX_REGISTERS; i++) {
        process->temp_reg_set[i] = 0;
    }

    // TODO: Load instructions into page 
    char instruction[INSTRUCTION_SIZE];
    int instruction_index = 0;
    while (fgets(instruction, INSTRUCTION_SIZE, file) != NULL) {
        int virt_addr = instruction_index * INSTRUCTION_SIZE;
        write_page(process, virt_addr, instruction, strlen(instruction));
        instruction_index++;
    }

    // Set PC 
    process->pc = 0x00000000;

    // TODO: Insert process into process list 
    process_list[process_count++] = *process;

    fclose(file);
}


// Simulation 
void simulate() 
{
    int finish; 

    // TODO: Repeat simulation until the process list is empty 
    while (process_count > 0) {
        // TODO: Select a process from process list using round-robin scheme 
        for (int i = 0; i < process_count; i++) {
            Process *process = &process_list[i];

            // TODO: Execue a processs 
            finish = execute(process); 

            // TODO: If the process is terminated then 
            //       call print_register_set(), 
            //       reclaim allocated frames, 
            //       and remove process from process list 
            if (finish) {
                print_register_set(process->pid);
                for (int j = 0; j < process->size / PAGE_SIZE; j++) {
                    if (process->page_table[j].valid) {
                        frame_list[process->page_table[j].frame_number] = -1;
                    }
                }
                for (int k = i; k < process_count - 1; k++) {
                    process_list[k] = process_list[k + 1];
                }
                process_count--;
                i--;
            }
        }

        clock++; 

    }
}


// Execute an instruction using program counter 
int execute(Process *process) 
{
    char instruction[INSTRUCTION_SIZE];
    char opcode;

    // TODO: Restore register set 
    for (int i = 0; i < MAX_REGISTERS; i++) {
        register_set[i] = process->temp_reg_set[i];
    }
    // TODO: Fetch instruction and update program counter      
    read_page(process, process->pc, instruction, INSTRUCTION_SIZE);
    process->pc += INSTRUCTION_SIZE;

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
    for (int i = 0; i < MAX_REGISTERS; i++) {
        process->temp_reg_set[i] = register_set[i];
    }
    // TODO: When the last instruction is executed return 1, otherwise return 0 
    if (process->pc >= process->num_inst * INSTRUCTION_SIZE) {
        return 1;
    } else {
        return 0;
    }
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
    int page_number = virt_addr / PAGE_SIZE;
    int offset = virt_addr % PAGE_SIZE;
    // TODO: Get a physical address from virtual address 
    if (!process->page_table[page_number].valid) {
        // TODO: handle page fault -> just allocate page 
        for (int i = 0; i < NUM_PAGES; i++) {
            if (frame_list[i] == -1) {
                process->page_table[page_number].frame_number = i;
                process->page_table[page_number].valid = 1;
                frame_list[i] = process->pid;
                break;
            }
        }
        
    }
    
    int frame_number = process->page_table[page_number].frame_number;
    int phys_addr = frame_number * PAGE_SIZE + offset;
   
    // TODO: Write 'buf' up to 'count' bytes at the physical address 
    memcpy(buf, &phy_memory[phys_addr], count);
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
