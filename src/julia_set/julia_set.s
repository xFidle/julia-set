; Arguments passed:
; rdi - *julia_set_args
; rsi - *pixel_array

section .data
    two dq 2.0
    sq_two dq 4.0

section .text
global julia_set

julia_set:
    push        rbp
    mov         rbp,    rsp
    push        rbx

extract_args:
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
    mov         rcx,    rax         ; column
    mov         rdx,    rbx         ; row

new_pixel:
    xor         r8,     r8          ; iterations counter

; c_real = (column - real_centre) * (2 * radius / width) * zoom
calculate_real:
    cvtsi2sd    xmm8,   rcx
    subsd       xmm8,   xmm0
    mulsd       xmm8,   xmm5
    mulsd       xmm8,   xmm14
    mulsd       xmm8,   xmm4
    divsd       xmm8,   xmm6         

; c_imag = (row - imag_centre) * (2 * radius / height) * zoom
calculate_imag:
    cvtsi2sd    xmm9,   rdx
    subsd       xmm9,   xmm1
    mulsd       xmm9,   xmm5
    mulsd       xmm9,   xmm14
    mulsd       xmm9,   xmm4
    divsd       xmm9,   xmm7          

; z = z * z + c
increase_z:
    cmp         r8,     255    
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
    mulsd       xmm11,  xmm11
    addsd       xmm10,  xmm11
    inc         r8
    comisd      xmm10,  xmm15
    jbe         increase_z

; color = max_iterations - color_iterations
; pixel_address = 3 * (row * width + col)
store_color:
    dec         r8
    mov         r9,         rdx
    imul        r9,         rax
    add         r9,         rcx
    imul        r9,         3
    mov         [rsi+r9],   r8b         ; blue
    mov         [rsi+r9+1], r8b         ; green
    mov         [rsi+r9+2], r8b         ; red

next:
    dec         rcx
    cmp         rcx, 1
    jnz         new_pixel
    mov         rcx, rax
    dec         rdx
    cmp         rdx, 0
    jnz         new_pixel

end:
    pop         rbx
    mov         rsp, rbp
    pop         rbp
    ret
