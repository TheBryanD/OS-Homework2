.intel_syntax noprefix

#
# k_print
#

.global k_print

k_print:
    # # push everything on stack
    push ebp
    mov ebp, esp
    pushad
    pushf


    # load parameters
    mov esi, [ebp+8]   # str 
    mov edx, [ebp+12]  # strlen
    mov ebx, [ebp+16]  # row
    mov ecx, [ebp+20]  # col


    
    # get offset
        # calculate offset into edi
    mov edi, 80 # edi = 80
    imul edi, ebx # 80 * row
    add edi, ecx  # 80 * row + col
    imul edi, 2 # (80 * row + col) * 2
    add edi, 0xb8000 # (80 * row + col) * 2 + 0xb8000


    loop:
        # # if str_len is 0 or edge of video mem
        cmp edx, 0
        # clean up and run
        je end
        # compare addr of edi to end of mem
        cmp edi, 0xB8000 +2 * (24 *80 +79)
        # if greater than jump to reset edi
        jg ediOutOfBounds

        # mov next char from string into mem and inc addr
        # (can be done with 1 instruction: movsb)
        movsb es:[edi], ds:[esi]

        # mov color byte into the video memory
        movb es:[edi], 91

        # inc addr
        inc edi
        # dec str_len
        dec edx

        # # if str_len > 0, jump to loop
        cmp edx, 0
        jg loop

        # # else clean up and exit
        jmp end

    ediOutOfBounds:
        # reset edi to beginning of mem
        mov edi, 0xB8000
        # return to loop
        jmp loop
    # # pop everything
    end:
        popf
        popad
        pop ebp
        ret


#
# k_scroll
#

.global k_scroll

k_scroll:
    pushad
    pushf
    mov esi, 80*2 + 0xb8000
    mov edi, 0xb8000
    mov ecx, 80 * 24 * 2
    rep movsb
    mov ecx, 80
    mov al, ' '
    mov ah, 91
    rep stosw
    popf
    popad
    ret

 .global k_clearscr

k_clearscr:
    pushad
    pushf

    # mov edi, 0xb8000
    mov edi, 0xb8000
    # mov colors
    mov al, ' '
    mov ah, 91
    
    # move ' ' 31 into edi addr
    mov [edi], ax
    # ecx = amount of times to rep stosw
    mov ecx, 2000
    # copy string
    rep stosw
    # pop and ret
    popf
    popad
    ret

#
# Kbd_enter
# 
.extern convert_num
.extern kbd_handler
.extern k_print

.global kbd_enter

stri: .asciz "Got a key: "

kbd_enter:
    # save eflags
    pushf
    # turn off interrupts
    cli
    # save all registers
    push ebp
    mov ebp, esp

    pushad

    # read scan code
    in al, 0x64
    and al, 0x01
    jz _kbd_skip
    in al, 0x60
    
    push ax
    call convert_num
    add ebp, 8


    # print got a key
    mov ebx, stri
    push ebx
    call println
    add esp, 8


    # call kbd_handler
    #mov eax, [ebp+8]
    #push eax
    #call kbd_handler
    #add esp, 8
_kbd_skip:
    mov al, 0x20
    # send EOI
    out 0x20, al
    # restore registers and eflags
    popad
    popf
    # iret
    iret
