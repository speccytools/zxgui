public _zxgui_image
public _zxgui_clear

extern _screen_border
extern _screen_color

extern asm_zx_cxy2saddr
extern asm_zx_cxy2aaddr

_zxgui_image:
    di
    pop ix                  ; ret

    pop de                  ; colors
    pop bc                  ; source

    exx                     ; stash de / bc

    ld de, ix

    pop bc
    ld h, c                 ; h - height
    pop bc
    ld l, c                 ; l - width

    pop bc                  ; iyl - y
    ld a, c
    ld iyl, a

    pop bc                  ; ixl - x
    ld a, c
    ld ixl, a

    push de                 ; push ret back

    ld a, ixl
    add a, l
    ld ixh, a               ; ixh = x + w

    ld a, iyl
    add a, h
    ld iyh, a               ; iyh = y + h

    exx                     ; restore de / bc

    push de                 ; store colors

    ei                      ; good to interrupt

    ; ixl = x, iyl = y, ixh = x + w, iyh = y + h

    ; d tracks y
    ; e tracks x

    ld a, iyl               ; reset y to beginning
    ld d, a                 ; d - y

__y_loop:
    ld a, ixl               ; reset x to beginning
    ld e, a                 ; e - x

    ld hl, de
    call asm_zx_cxy2saddr   ; now hl contains pixel coordinates

__x_loop:

    ; spit out a character (8 lines)
    rept 8
    ld a, (bc)
    ld (hl), a
    inc h
    inc bc
    endr

    ld a, h                 ; go back 8 pixels
    sub 8
    ld h, a
    inc l                   ; got a tile right

    inc e                   ; move right a tile
    ld a, e
    cp ixh
    jr nz, __x_loop         ; not near right edge?

    inc d                   ; move bottom a tile
    ld a, d
    cp iyh
    jr nz, __y_loop         ; not near bottom edge?

    pop de                  ; restore colors int bc

    ld a, iyl               ; reset y to beginning
    ld b, a                 ; b - y

__y_color_loop:
    ld a, ixl               ; reset y to beginning
    ld c, a                 ; l - x

    ld hl, bc
    call asm_zx_cxy2aaddr   ; now hl contains color coordinates

__x_color_loop:

    ld a, (de)              ; modify color
    ld (hl), a
    inc de
    inc hl

    inc c                   ; move right a tile
    ld a, c
    cp ixh
    jr nz, __x_color_loop   ; not near right edge?

    inc b                   ; move bottom a tile
    ld a, b
    cp iyh
    jr nz, __y_color_loop   ; not near bottom edge?

    ret

_zxgui_clear:
    push de
    push bc
    push hl

    ; black border
    ld a, (_screen_border)
    out (254),a

    ; clear color data
    ld hl, $5800
    ld de, $5801
    ld bc, 767
    ld (hl), 0x44
    ldir

    ; clear screen
    ld hl, $4000
    ld de, $4001
    ld bc, 6911
    ld (hl), 0
    ldir

    pop hl
    pop bc
    pop de

    ret