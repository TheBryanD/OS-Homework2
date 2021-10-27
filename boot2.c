// Main Driver for OS assignment 2 for class
//Include booleans
#include <stdbool.h>
//include uint16_t and uint32_t
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <string.h>


//Row global var
int currRow = 0;

//############################################ Scan Codes #################

enum CSET_1 {
  Q_PRESSED = 0x10, W_PRESSED = 0x11, E_PRESSED = 0x12, R_PRESSED = 0x13,
  T_PRESSED = 0x14, Y_PRESSED = 0x15, U_PRESSED = 0x16, I_PRESSED = 0x17,
  O_PRESSED = 0x18, P_PRESSED = 0x19
};
static char* cset_1_chars = "qwertyuiop";
//cset_1_enum_array[] = {Q_PRESSED, W_PRESSED, E_PRESSED, R_PRESSED, T_PRESSED, Y_PRESSED, U_PRESSED, I_PRESSED, O_PRESSED, P_PRESSED};

enum CSET_2 {
  A_PRESSED = 0x1E, S_PRESSED = 0x1F, D_PRESSED = 0x20, F_PRESSED = 0x21,
  G_PRESSED = 0x22, H_PRESSED = 0x23, J_PRESSED = 0x24, K_PRESSED = 0x25,
  L_PRESSED = 0x26
};
static char *cset_2_chars = "asdfghjkl";

enum CSET_3 {
  Z_PRESSED = 0x2C, X_PRESSED = 0x2D, C_PRESSED = 0x2E, V_PRESSED = 0x2F,
  B_PRESSED = 0x30, N_PRESSED = 0x31, M_PRESSED = 0x32,
};
static char *cset_3_chars = "zxcvbnm";

enum CSET_NUMBERS {
  ONE_PRESSED = 0x2, TWO_PRESSED = 0x3, THREE_PRESSED = 0x4,
  FOUR_PRESSED = 0x5, FIVE_PRESSED = 0x6, SIX_PRESSED = 0x7,
  SEVEN_PRESSED = 0x8, EIGHT_PRESSED = 0x9, NINE_PRESSED = 0xA
};

#define CSET_ZERO 0x0B

#define CSET_NL 0x1C
#define CSET_SPC 0x39
#define CSET_RET 0xE
#define CSET_POINT_PRESSED 0x34
#define CSET_SLASH_PRESSED 0x35


//############################################ /Scan Codes #################


//############################################  Char Buffer Ring Queue  #############################################3
//Global charbuffer
struct queue {
    int tail;
    int head;
    int size;
    int totSize;
    char* data;
};
typedef struct queue queue_type;


static queue_type create_queue(int totSize)
{
    queue_type queue;
        queue.head = 0;
        queue.tail = -1;
        queue.size = 0;
        queue.totSize = totSize;
        char data[totSize];
        queue.data = data;

        return queue;

}

//Queue Functions
bool isEmpty(queue_type* queue)
{
    //if queue doesnt exist/error
    if (queue == NULL)
    {
        return false;
    }
    //if isEmpty is true
    else if (queue->size = 0)
    {
        return true;
    }
    //if queue is not empty
    else 
    {
        return false;
    }
}

bool enqueue(queue_type* queue, char c)
{
    if (queue == NULL)
    {
        return false;
    }
    else 
    {
        queue->tail = (queue->tail + 1) % queue->totSize;
        queue->data[queue->tail] = c;
        queue->size = queue->size + 1;
        return true;
    }
}

char dequeue(queue_type* queue)
{
    char c = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->totSize;
    queue->size = queue->size - 1;
    return c;
}


    //char buffer
    queue_type char_buffer;

//############################################################< /Ring queue> ############################33

//global func for kbd_enter
extern void kbd_enter();

//IDT
struct idt_entry_s {
    uint16_t base_low16; //lower 16 bits of the ISR's addr (isr = interrupt service routine)
    uint16_t selector; // GDT segment selector that the CPU will load into CS before calling ISR
    uint8_t always0; //reserved, set to 0
    uint8_t access; //Type and attributes
    uint16_t base_hi16; //higher 16 bits of the ISR's addr
}__attribute__((packed));
typedef struct idt_entry_s idt_entry_t;

//create global array of idt entries
idt_entry_t idt[256];

//gdt_r_s
struct gdt_r_s {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct gdt_r_s gtd_r_t;

//function prototypes
void k_clearscr();
void println(char *string);
void k_print(char *string, int string_length, int row, int col);
void k_scroll();

//function prototypes for assignment 2
char k_getchar();
void kbd_hanlder();
void initIDT();
void initIDTEntry(idt_entry_t *entry, uint32_t base, uint16_t selector, uint8_t access);
void printDefaultErrorMsg();
void setupPIC();
void enableInterrupts();
void lidtr(gtd_r_t *idtr);
void outportb(uint16_t port, uint32_t data);
char translate_scancode(int what);

//convert num provided
int convert_num_h(unsigned int num, char buf[])
{
    if (num == 0)
    {
        return 0;
    }
    int idx = convert_num_h(num/10, buf);
    buf[idx] = num % 10 + '0';
    buf[idx+1] = '\0';
    return idx + 1;
}

//convert number provided
void convert_num(unsigned int num, char buf[])
{
    if (num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
    }
    else
    {
        convert_num_h(num, buf);
    }
}

int main()
{

    char_buffer = create_queue(100);

    //Clear Screen
    k_clearscr();
    char* str;

    //initialize idt
    initIDT();

    //setup PIC
    setupPIC();

    //enable interrupts
    enableInterrupts();

    //infinite while loop to continue OS running
    while(1)
    {
        continue;
    }
}

void println(char *string)
{
    //count amount of items
    int count =0;
    while(string[count])
    {
        count = count +1 ;
    }
    //set num_to_print to the length of the message (it is 0 term, so you must count)
        int num_to_print = count;
    //while num_to_print != 0
    while (num_to_print != 0)
    {
        //if num_to_print < 80
        if(num_to_print < 80)
        {
            //call k_print(msg, row, 0, num_to_print)
            k_print(string, num_to_print, currRow, 0);
            //set num_to_print = 0
            num_to_print = 0;
        }
        //else
        else
        {
            //call k_print(msg, row, 0, 80)
            k_print(string, 80, currRow, 0);
            //num_to_print - 80
            num_to_print = num_to_print - 80;
        }
        //row = row + 1
        currRow = currRow + 1;
        //if row > 24
        if (currRow > 24)
        {
            //k_scroll()
            k_scroll();
            //set row = 24
            currRow = 24;
        }
    }
}

char k_getchar()
{
    //if kbd_buffer is empty them return 0;
    if (isEmpty(&char_buffer) == true)
    {
        return 0;
    }
    //get next character in kbd_buffer(with wrap)
    char c = dequeue(&char_buffer);
    //Return the character
    return c;
}

//extern void kdb_handler();
void kbd_handler(unsigned int scancode)
{
    //if scan code is 0 or kbd_buffer is full then return
    if (scancode == 0 || char_buffer.size == char_buffer.totSize)
    {
        return;
    }
    //translate the scan code into a character
    char letter = translate_scancode(scancode);
    //add character to kbd_buffer (with wrap)
    enqueue(&char_buffer, letter);
    //return
    return;
}

void initIDT()
{
    //call initIDTEntry() for entries 0-31 setting these to point to the default handler
    for (int i =0; i < 31; i++)
    {
        initIDTEntry((idt + i), (uint32_t)printDefaultErrorMsg, 16, 0x8e);
    }
    //call initIDTEntry() for entry 32, setting to point to 0
    initIDTEntry((idt + 32), 0, 16, 0x8e);
    //call initIDTEntry() for entry 33, setting it to point to your keyboard handler (kbd_enter())
    initIDTEntry((idt + 33), (uint32_t)kbd_enter, 16, 0x8e);
    //call initIDTEntry() for entries 34-255, setting these to point to 0
    for (int i =34; i < 255; i++)
    {
        initIDTEntry((idt+i), 0, 16, 0x8e);
    } 
    //call lidtr(idtr)
    //create gdt_r_s to pass to lidtr
    gtd_r_t idtr;
    idtr.limit = (uint16_t)(sizeof(idt) * 256-1);
    idtr.base = (uint32_t)idt;
    lidtr(&idtr);
}

void initIDTEntry(idt_entry_t *entry, uint32_t base, uint16_t selector, uint8_t access)
{
    entry->selector = selector;
    entry->access = access;
    entry->always0 = 0;
    entry->base_low16 = (uint16_t)base;
    entry->base_hi16 = (uint16_t)(base >> 16);
}

void printDefaultErrorMsg()
{
    println("A Bad Problem has occured.");
    kbd_enter();
}

void setupPIC()
{
    //set up cascading mode:
    outb(0x20, 0x11); //Start 8259 master initialization
    outb(0xA0, 0x11); //Start 8259 slave initialization
    outb(0x21, 0x20); //set master base interrupt vector (idt 32-38)
    outb(0xA1, 0x28); //Set slave base interrupt vector (idt 39-45)
    // Tell the master that he has a slave
    outb(0x21, 0x04); //set cascade...
    outb(0xA1, 0x02); //on IRQ2
    // Enabled 8086 mode:
    outb(0x21, 0x01); // finish 8259 initialization
    outb(0xA1, 0x01);
    // Reset the IRQ masks
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
    // Now, enable the keyboard IRQ only
    outb(0x21, 0xfd); // Turn on the keyboard IRQ
    outb(0xA1, 0xff); // Turn off all others
}

void enableInterrupts()
{
    __asm__ __volatile__ ("sti");
}

void lidtr(gtd_r_t* idtr)
{
    __asm__ ("lidt %0" :: "m"(*idtr));
}

void outportb(uint16_t port, uint32_t data)
{
    __asm__ __volatile__ ("out %%al, %%dx" :: "dN" (port), "aN" (data));
}

char translate_scancode(int what)
{
    /*for (int i =0; i < strlen(cset_1_chars); i++)
    {
        if (what == cset_1_chars[i])
        {
            return cset_1_enum_array[i];
        }
    }*/
}