    ld a, (bc)                      ; load the char num

    ex de, hl

    ld h, 0
    ld l, a                         ; hl now holds character num - 32

    add hl, hl
    add hl, hl
    add hl, hl                      ; multiply by 8
    add hl, font - 32 * 8

    ex de, hl                       ; de now holds a pointer to characted data
