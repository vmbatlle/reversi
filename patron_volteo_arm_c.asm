.section .text
.globl patron_volteo_asm_c
.extern ficha_valida

patron_volteo_asm_c:
  mov r12, sp
  # TODO: Guardar solo reg necesarios
  push {r4-r10, r11, r12, lr, pc}

  sub r11, r12, #4
  # 0x30 TODO: ¿Hace falta todo este espacio? -> Si, para llamar recursivamente a patron_volteo...?
  sub sp, sp, #48
  #str r0, [r11, #-32] &tablero
  mov r4, r0
  #str r1, [r11, #-36] &longitud    # 0x24
  mov r5, r1
  #strb r2, [r11, #-37] FA    # 0x25
  mov r6, r2
  #strb r3, [r11, #-38] CA   # 0x26
  mov r7, r3
  
  # FA = FA + SF
  ldrb r8, [r11, #4]
  add r6, r6, r8
  
  # CA = CA + SC
  ldrb r9, [r11, #8]
  add r7, r7, r9
  
  # casilla = ficha_valida(tablero, FA, CA, &posicion_valida)#
  # Preparar el paso de parámetros, r0 ya tiene &tablero
  mov r1, r6
  mov r2, r7
  sub r3, r11, #24
  bl ficha_valida
  # TODO: guardar el resultado (r0) en algún sitio?
  
  #- if ((posicion_valida == 1) && (casilla != color)) {
  # } else if ((posicion_valida == 1 && (casilla == color)) {
  # } else { }
  
  # Comprobar primero si posición válida es distinto de 1
  ldr r1, [r11, #-24]
  cmp r1, #1
  movne r3, #0
  # NO_HAY_PATRON
  bne patron_volteo_callback
  
  # Decidir qué hacer según el valor de casilla
  ldr r10, [r11, #12]
  cmp r0, r10
  beq patron_volteo_color_igual
  
  # casilla != color
  # *longitud = *longitud + 1
  ldr r1, [r5]
  add r1, r1, #1
  str r1, [r5]
  # patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color)
  mov r0, r4
  mov r1, r5
  mov r2, r6
  mov r3, r7
  # TODO: no estoy nada seguro de esto, he confiado mucho en eclipse para hacerlo
  str r8, [sp]
  # Guardar SF, SC y color para la llamada a patron_volteo
  str r9, [sp, #4]
  str r10, [sp, #8]
  bl patron_volteo_asm_c
  mov r3, r0
  b patron_volteo_callback
  
  # casilla == color
  patron_volteo_color_igual:
  #- if (*longitud > 0)
  ldr r1, [r5]
  cmp r1, #0
  movgt r3, #1
  # PATRON_ENCONTRADO
  movle r3, #0
  # NO_HAY_PATRON
  
  # Callback
  patron_volteo_callback:
  mov r0, r3
  sub sp, r11, #12
  ldm sp, {r11, sp, lr}
  bx lr

  .end
