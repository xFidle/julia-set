; Arguments passed:
; rdi - *thread_args

section .data
    align 32
    two dq 2.0, 2.0, 2.0, 2.0
    align 32
    sq_two dq 4.0, 4.0, 4.0, 4.0

section .text
global julia_set

julia_set:
    push            rbp
    mov             rbp,    rsp
    push            rbx

extract_thread_args:
    mov             rax,    [rdi]       ; *julia_set_args
    mov             rsi,    [rdi+8]     ; *pixel_array
    mov             edx,    [rdi+16]    ; first_row
    mov             ecx,    [rdi+20]    ; last_row

extract_julia_set_args:
    mov             r8d,    [rax]       ; width
    mov             r9d,    [rax+4]     ; height
    vbroadcastsd    ymm1,   [rax+8]     ; real_centre
    vbroadcastsd    ymm2,   [rax+16]    ; imag_centre
    vbroadcastsd    ymm3,   [rax+25]    ; c_real
    vbroadcastsd    ymm4,   [rax+32]    ; c_imag
    vbroadcastsd    ymm5,   [rax+40]    ; zoom
    vbroadcastsd    ymm6,   [rax+48]    ; radius

prep:
    vcvtsi2sd       xmm0,   r8d         ; double width
    vbroadcastsd    ymm7,   xmm0           
    vcvtsi2sd       xmm0,   r9d         ; double height
    vbroadcastsd    ymm8,   xmm0

setup_columns:
    mov             eax,    r8d
    dec             eax
    vpinsrd         xmm9,   eax,    0 
    dec             eax
    vpinsrd         xmm9,   eax,    1 
    dec             eax
    vpinsrd         xmm9,   eax,    2
    dec             eax
    vpinsrd         xmm9,   eax,    3

setup_rows:
    mov             eax,    ecx
    dec             eax
    vpinsrd         xmm10,  ecx,    0 
    vpbroadcastd    xmm10,  xmm10

; iterations counter 
new_pixel:
    mov             eax,    255
    vpxor           xmm11,  xmm11 
    vpinsrd         xmm11,  eax,    0
    vpbroadcastd    xmm11,  xmm11

; z_real = (column - real_centre) * (2 * radius / width) * zoom
calculate_real:
    vcvtdq2pd       ymm12,  xmm9
    vsubpd          ymm12,  ymm1
    vmulpd          ymm12,  ymm14
    vmulpd          ymm12,  ymm6
    vmulpd          ymm12,  ymm5
    vdivpd          ymm12,  ymm7

; z_imag = (row - imag_centre) * (2 * radius / height) * zoom
calculate_imag:
    vcvtdq2pd       ymm13,  xmm10
    vsubpd          ymm13,  ymm2
    vmulpd          ymm13,  ymm14
    vmulpd          ymm13,  ymm6
    vmulpd          ymm13,  ymm5
    vdivpd          ymm13,  ymm8

; z = z * z + c
increase_z:
    vpxor           xmm0,   xmm0
    vpcmpeqd        xmm0,   xmm11  
    vptest          xmm0,   xmm0
    jnz             store_color
    vmovdqa         ymm0,   ymm12
    vmulpd          ymm0,   ymm13
    vmulpd          ymm0,   ymm14           
    vaddpd          ymm0,   ymm4            
    vmulpd          ymm13,  ymm13           
    vmulpd          ymm12,  ymm12           
    vsubpd          ymm12,  ymm13
    vaddpd          ymm12,  ymm3        
    vmovdqa         ymm13,  ymm0

check_condition:
    movsd           xmm10,  xmm8
    movsd           xmm11,  xmm9
    mulsd           xmm10,  xmm10
    inc             eax
    comisd          xmm10,  xmm15
    jbe             increase_z

; color = color_iterations
prep_color:
    mov         ebx,    255
    sub         ebx,    eax
    mov         eax,    0xFF 
    shl         eax,    8   
    or          eax,    ebx 
    shl         eax,    8
    or          eax,    ebx
    shl         eax,    8
    or          eax,    ebx

; pixel_addres = 4 * ((height - row) * width + column)
store_color:
    mov         rbx,         r9
    sub         rbx,         r11
    imul        rbx,         r8
    add         rbx,         r10
    imul        rbx,         4
    mov         [rsi+rbx],   eax

next:
    dec         r10d
    cmp         r10d, 0
    jg          new_pixel
    mov         r10d, r8d
    dec         r11d
    cmp         r11d, edx
    jg          new_pixel

end:
    pop         rbx
    mov         rsp, rbp
    pop         rbp
    ret
