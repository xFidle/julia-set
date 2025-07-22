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
    mov             rbp,        rsp
    push            rbx
    and             rsp,        -32

extract_thread_args:
    mov             rax,        [rdi]               ; *julia_set_args
    mov             rsi,        [rdi+8]             ; *pixel_array
    mov             edx,        [rdi+16]            ; first_row
    mov             ecx,        [rdi+20]            ; last_row

extract_julia_set_args:
    mov             r8d,        [rax]               ; width
    mov             r9d,        [rax+4]             ; height
    vbroadcastsd    ymm1,       [rax+8]             ; real_centre
    vbroadcastsd    ymm2,       [rax+16]            ; imag_centre
    vbroadcastsd    ymm3,       [rax+25]            ; c_real
    vbroadcastsd    ymm4,       [rax+32]            ; c_imag
    vbroadcastsd    ymm5,       [rax+40]            ; zoom
    vbroadcastsd    ymm6,       [rax+48]            ; radius
    vmovapd         ymm14,      [rel two]
    vmovapd         ymm15,      [rel sq_two]

prep:
    vcvtsi2sd       xmm0,       r8d                 ; double width
    vbroadcastsd    ymm7,       xmm0           
    vcvtsi2sd       xmm0,       r9d                 ; double height
    vbroadcastsd    ymm8,       xmm0

setup_columns:
    mov             eax,        0x03020100
    vmovd           xmm0,       eax
    vpmovzxbd       xmm0,       xmm0
    mov             eax,        r8d
    dec             eax
    vpbroadcastd    xmm9,       eax
    vpsubd          xmm9,       xmm0

setup_rows:
    mov             eax,        r9d
    dec             eax
    vpbroadcastd    xmm10,      eax
    jmp             new_pixel

pop_from_stack:
    vmovapd         ymm1,       [rsp]
    vmovapd         ymm2,       [rsp+32]
    add             rsp,        64

; iterations counter 
new_pixel:
    mov             eax,        255 
    vpxord          xmm11,      xmm11
    vpbroadcastd    xmm11,      eax

; z_real = (column - real_centre) * (2 * radius / width) * zoom
calculate_real:
    vcvtdq2pd       ymm12,      xmm9
    vsubpd          ymm12,      ymm1
    vmulpd          ymm12,      ymm14
    vmulpd          ymm12,      ymm6
    vmulpd          ymm12,      ymm5
    vdivpd          ymm12,      ymm7

; z_imag = (row - imag_centre) * (2 * radius / height) * zoom
calculate_imag:
    vcvtdq2pd       ymm13,      xmm10
    vsubpd          ymm13,      ymm2
    vmulpd          ymm13,      ymm14
    vmulpd          ymm13,      ymm6
    vmulpd          ymm13,      ymm5
    vdivpd          ymm13,      ymm8

push_unused_to_stack:
    sub             rsp,        64
    vmovapd         [rsp],      ymm1
    vmovapd         [rsp+32],   ymm2

first_mask:
    vpcmpeqq        ymm2,       ymm2,       ymm2

check_iter_counter:
    vpxord          xmm0,       xmm0
    vpcmpeqd        xmm0,       xmm11  
    vptest          xmm0,       xmm0
    jnz             prep_colors
    
; z = z * z + c
increase_z:
    vmovdqa         ymm0,       ymm12
    vmulpd          ymm0,       ymm0
    vmovdqa         ymm1,       ymm13
    vmulpd          ymm1,       ymm1
    vsubpd          ymm0,       ymm1
    vaddpd          ymm0,       ymm3
    vandpd          ymm0,       ymm2,       ymm0
    vandnpd         ymm1,       ymm2,       ymm12 
    vorpd           ymm12,      ymm0,       ymm1

    vmovdqa         ymm0,       ymm14
    vmulpd          ymm0,       ymm12
    vmulpd          ymm0,       ymm13 
    vaddpd          ymm0,       ymm4
    vandpd          ymm0,       ymm2,       ymm0
    vandnpd         ymm1,       ymm2,       ymm13
    vorpd           ymm13,      ymm0,       ymm1

dec_iter_counter:
    vextractf128    xmm0,       ymm2,       1
    vshufps         xmm0,       xmm2,       xmm0,       0x88
    mov             eax,        1
    vpbroadcastd    xmm1,       eax
    vpsubd          xmm1,       xmm11,      xmm1 
    vpandd          xmm1,       xmm0
    vpandnd         xmm0,       xmm0,       xmm11 
    vpord           xmm11,      xmm0,       xmm1 

check_condition:
    vmovdqa         ymm0,       ymm12
    vmovdqa         ymm1,       ymm13
    vmulpd          ymm0,       ymm0
    vmulpd          ymm1,       ymm1
    vaddpd          ymm0,       ymm1
    vcmppd          ymm2,       ymm0,       ymm15,      2       
    vptest          ymm2,       ymm2
    jnz             check_iter_counter

; color = color_iterations
prep_colors:
    mov             eax,        0xFF 
    vpbroadcastd    xmm0,       eax
    vmovdqa         xmm1,       xmm11        
    vpslld          xmm0,       8   
    vpord           xmm0,       xmm1
    vpslld          xmm0,       8
    vpord           xmm0,       xmm1
    vpslld          xmm0,       8
    vpord           xmm0,       xmm1

; pixel_addres = 4 * ((height - row) * width + column)
store_colors:
    mov             rax,        r9
    dec             rax
    vpextrd         rbx,        xmm10,      3       ; lowest_row 
    sub             rax,        rbx
    imul            rax,        r8
    vpextrd         rbx,        xmm9,       3       ; lowest_column
    add             rax,        rbx
    imul            rax,        4
    vmovdqu         [rsi+rax],  xmm0

set_next_columns:
    mov             eax,        4
    vpbroadcastd    xmm0,       eax
    vpsubd          xmm0,       xmm9,       xmm0
    vpbroadcastd    xmm1,       r8d
    vpaddd          xmm1,       xmm0
    vpxord          xmm2,       xmm2
    vpcmpgtd        xmm2,       xmm2,       xmm0        ; mask (columns that are lower than 0 are masked)
    vblendvps       xmm9,       xmm0,       xmm1,       xmm2

set_next_rows:
    mov             eax,        1
    vpbroadcastd    xmm0,       eax
    vpsubd          xmm0,       xmm10,      xmm0
    vblendvps       xmm10,      xmm10,      xmm0,       xmm2 
    vpbroadcastd    xmm0,       edx
    vpcmpgtd        xmm0,       xmm10,      xmm0
    vpcmpeqd        xmm1,       xmm0,       xmm10
    vpxord          xmm0,       xmm1
    vptest          xmm0,       xmm0
    jnz             pop_from_stack

end:
    pop         rbx
    mov         rsp, rbp
    pop         rbp
    ret
