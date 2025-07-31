; Requirements:
; WIDTH must be divisible by 4

; Arguments passed:
; rdi - *thread_args

section .text
global julia_set

julia_set:
    push            rbx
    push            rbp
    mov             rbp,        rsp
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
    vbroadcastsd    ymm3,       [rax+24]            ; c_real
    vbroadcastsd    ymm4,       [rax+32]            ; c_imag
    vbroadcastsd    ymm5,       [rax+40]            ; zoom
    vbroadcastsd    ymm6,       [rax+48]            ; radius

vectorize_arguments:
    vcvtsi2sd       xmm0,       r8d
    vbroadcastsd    ymm7,       xmm0                ; vector of widths
    vcvtsi2sd       xmm0,       r9d
    vbroadcastsd    ymm8,       xmm0                ; vector of heights

prep_constant_numbers:
    mov             eax,        2
    vcvtsi2sd       xmm0,       eax
    vbroadcastsd    ymm14,      xmm0                ; vector of 2s
    mov             eax,        4
    vcvtsi2sd       xmm0,       eax
    vbroadcastsd    ymm15,      xmm0                ; vector of 4s

prep_loop:
    mov             r10d,       r8d
    sub             r10d,       4
    mov             r11d,       ecx
    dec             r11d
    jmp             new_pixel

pop_from_stack:
    vmovapd         ymm1,       [rsp]
    vmovapd         ymm2,       [rsp+32]
    add             rsp,        64

; iterations counter
new_pixel:
    mov             eax,        256 
    vpxord          xmm11,      xmm11
    vpbroadcastd    xmm11,      eax

; real_scale = (2 * radius / width) * zoom
; z_real = real_centre + (column - width / 2) * real_scale
calculate_real:
    vpbroadcastd    xmm12,      r10d
    mov             eax,        0x00010203   
    vmovd           xmm0,       eax
    vpmovzxbd       xmm0,       xmm0
    vpaddd          xmm12,      xmm0 
    mov             eax,        r8d
    shr             eax,        1
    vpbroadcastd    xmm0,       eax
    vpsubd          xmm12,      xmm0
    vcvtdq2pd       ymm12,      xmm12
    vmulpd          ymm12,      ymm14
    vmulpd          ymm12,      ymm6
    vdivpd          ymm12,      ymm7
    vmulpd          ymm12,      ymm5
    vaddpd          ymm12,      ymm1

; imag_scale = (2 * radius / width) * zoom
; z_imag = imag_centre + (row - height / 2) * imag_scale;
calculate_imag:
    vpbroadcastd    xmm13,      r11d
    mov             eax,        r9d
    shr             eax,        1
    vpbroadcastd    xmm0,       eax
    vpsubd          xmm13,      xmm0
    vcvtdq2pd       ymm13,      xmm13
    vmulpd          ymm13,      ymm14
    vmulpd          ymm13,      ymm6
    vdivpd          ymm13,      ymm8
    vmulpd          ymm13,      ymm5
    vaddpd          ymm13,      ymm2

; real_centre (ymm1) and imag_centre (ymm2) are not needed for next calculations
push_unused_to_stack:
    sub             rsp,        64
    vmovapd         [rsp],      ymm1
    vmovapd         [rsp+32],   ymm2

first_mask:
    vpcmpeqq        ymm2,       ymm2

check_iter_counter:
    vpxord          xmm0,       xmm0
    vpcmpeqd        xmm0,       xmm11
    vptest          xmm0,       xmm0
    jnz             concat_whole_color
    
; z = z * z + c
increase_z:
    vmovdqa         ymm0,       ymm12
    vmulpd          ymm0,       ymm0
    vmovdqa         ymm1,       ymm13
    vmulpd          ymm1,       ymm1
    vsubpd          ymm0,       ymm1
    vaddpd          ymm0,       ymm3
    vmovdqa         ymm1,       ymm14
    vmulpd          ymm1,       ymm12
    vmulpd          ymm1,       ymm13 
    vaddpd          ymm1,       ymm4
    vblendvpd       ymm12,      ymm0,       ymm2
    vblendvpd       ymm13,      ymm1,       ymm2

dec_iter_counter:
    vextractf128    xmm0,       ymm2,       1
    vshufps         xmm0,       xmm2,       0x88
    mov             eax,        1
    vpbroadcastd    xmm1,       eax
    vpsubd          xmm1,       xmm11,      xmm1
    vblendvps       xmm11,      xmm1,       xmm0

check_escape_condition:
    vmovdqa         ymm0,       ymm12
    vmulpd          ymm0,       ymm0
    vmovdqa         ymm1,       ymm13
    vmulpd          ymm1,       ymm1
    vaddpd          ymm0,       ymm1
    vcmppd          ymm2,       ymm0,       ymm15,      2
    vtestpd         ymm2,       ymm2
    jnz             check_iter_counter

concat_whole_color:
    mov             eax,        0xFF
    vpbroadcastd    xmm1,       eax
    vmovdqa         xmm2,       xmm1
    vpslld          xmm1,       8
    vpaddd          xmm1,       xmm11
    vpslld          xmm1,       8
    vpaddd          xmm1,       xmm11
    vpslld          xmm1,       8
    vpaddd          xmm1,       xmm11

; pixel_addres = 4 * ((height - 1 - row) * width + column)
store_colors:
    mov             eax,        r9d
    dec             eax
    sub             eax,        r11d
    imul            eax,        r8d
    add             eax,        r10d
    imul            eax,        4
    vmovdqa         [rsi+rax],  xmm1

next_pixels:
    sub             r10d,       4
    cmp             r10d,       0
    jge             pop_from_stack
    mov             r10d,       r8d
    sub             r10d,       4
    sub             r11d,       1
    cmp             r11d,       edx
    jge             pop_from_stack

end:
    mov             rsp,        rbp
    pop             rbp
    pop             rbx
    ret
