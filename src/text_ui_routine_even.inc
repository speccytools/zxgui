    ld a, (de)                      ; get next 8 bit row of the character data
    and $F0
    ld (hl), a                      ; put it on screen

    inc de