; Arguments passed:
; rdi - *thread_args

section .data
    two dq 2.0
    sq_two dq 4.0

section .text
global julia_set

julia_set:
    push        rbp
    mov         rbp,    rsp
    push        rbx

extract_thread_args:
    mov         rsi,    [rdi+8]     ; *pixel_array
    mov         edx,    [rdi+16]    ; first_row
    mov         ecx,    [rdi+20]    ; last_row
    mov         rdi,    [rdi]       ; *julia_set_args

extract_julia_set_args:
    mov         eax,    [rdi]       ; width
    mov         ebx,    [rdi+4]     ; height
    movsd       xmm0,   [rdi+8]     ; real_centre
    movsd       xmm1,   [rdi+16]    ; imag_centre
    movsd       xmm2,   [rdi+24]    ; c_real
    movsd       xmm3,   [rdi+32]    ; c_imag
    movsd       xmm4,   [rdi+40]    ; zoom
    movsd       xmm5,   [rdi+48]    ; radius

prep:
    cvtsi2sd    xmm6,   rax         ; double width
    cvtsi2sd    xmm7,   rbx         ; double height
    movsd       xmm14,  [rel two]
    movsd       xmm15,  [rel sq_two]

loop_setup:
    mov         r8,    rax          ; column
    mov         r9,    rcx          ; row
    dec         r8
    dec         r9

new_pixel:
    xor         r10d,     r10d      ; iterations counter

; c_real = (column - real_centre) * (2 * radius / width) * zoom
calculate_real:
    cvtsi2sd    xmm8,   r8
    subsd       xmm8,   xmm0
    mulsd       xmm8,   xmm5
    mulsd       xmm8,   xmm14
    mulsd       xmm8,   xmm4
    divsd       xmm8,   xmm6         

; c_imag = (row - imag_centre) * (2 * radius / height) * zoom
calculate_imag:
    cvtsi2sd    xmm9,   r9
    subsd       xmm9,   xmm1
    mulsd       xmm9,   xmm5
    mulsd       xmm9,   xmm14
    mulsd       xmm9,   xmm4
    divsd       xmm9,   xmm7          

; z = z * z + c
increase_z:
    cmp         r10d,    255    
    je          store_color
    movsd       xmm10,  xmm9
    mulsd       xmm10,  xmm8
    mulsd       xmm10,  xmm14
    mulsd       xmm8,   xmm8
    mulsd       xmm9,   xmm9
    subsd       xmm8,   xmm9
    movsd       xmm9,   xmm10
    addsd       xmm8,   xmm2
    addsd       xmm9,   xmm3          

check_condition:
    movsd       xmm10,  xmm8
    movsd       xmm11,  xmm9
    mulsd       xmm10,  xmm10
    inc         r10d
    comisd      xmm10,  xmm15
    jbe         increase_z

; color = color_iterations
prep_color:
    mov         r11d,   255
    sub         r11d,   r10d
    mov         r10d,   r11d
    mov         r11d,   0xFF 
    shl         r11d,   8   
    or          r11d,   r10d 
    shl         r11d,   8
    or          r11d,   r10d
    shl         r11d,   8
    or          r11d,   r10d
    mov         r10d,   r11d

; pixel_addres = 4 * (row * width + column)
store_color:
    mov         r11,         r9
    imul        r11,         rax
    add         r11,         r8
    imul        r11,         4
    mov         [rsi+r11],   r10d

next:
    dec         r8
    cmp         r8, 0
    jg          new_pixel
    mov         r8, rax
    dec         r9
    cmp         r9, rdx
    jg          new_pixel

end:
    pop         rbx
    mov         rsp, rbp
    pop         rbp
    ret
