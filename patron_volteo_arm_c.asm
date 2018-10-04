
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_c.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de la función patron_volteo en ARM
# Versión:     1.0.0
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo_asm_c
.extern ficha_valida

patron_volteo_asm_c:
  #----- Inicio subrutina
  # r0 = &tablero
  # r1 = &longitud
  # r2 = FA
  # r3 = CA
  # *sp = SF
  # *(sp+4) = SC
  # *(sp+8) = color
  
  mov ip, sp
  push {r4-r10, fp, ip, lr, pc}
  sub fp, ip, #4
  # Solo hace falta guardar espacio para posicion_valida
  # casilla y patron no necesitan guardarse en memoria al ser devueltas en r0 por sus respectivas funciones
  sub sp, sp, #4
  
  #----- Cuerpo de la subrutina
  # Cargar SF en r4, SC en r5 y color en r6
  ldmib fp, {r4-r6}
  
  # FA = FA + SF
  add r2, r2, r4
  # Para restar 1 suma 255, por lo tanto solo los 8 primeros bits de r2 son válidos
  and r2, r2, #255
  
  # CA = CA + SC
  add r3, r3, r5
  and r2, r2, #255
  
  # casilla = ficha_valida(tablero, FA, CA, &posicion_valida)
  # Preparar el paso de parámetros, r0 ya tiene &tablero
  # Salvar &tablero (r0) en r7 y &longitud (r1) en r8
  mov r7, r0
  mov r8, r1
  # FA en r1 y CA en r2, guardarlos en r9 y r10 respectivamente, pueden cambiar al llamar a ficha_valida
  mov r1, r2
  mov r2, r3
  mov r9, r1
  mov r10, r2
  # &posicion_valida en r3
  sub r3, fp, #44
  bl ficha_valida
  
  #--- Bloque condicional principal
  #- if ((posicion_valida == 1) && (casilla != color)) {
  #- } else if ((posicion_valida == 1 && (casilla == color)) {
  #- } else { }
  
  # Comprobar primero si posicion_valida es distinto de 1
  ldr r3, [fp, #-44]
  cmp r3, #1
  # posicion_valida = 0, devolver #NO_HAY_PATRON
  movne r0, #0
  bne patron_volteo_callback
  
  # Comparar casilla (r0) con color (r6)
  # Cargar longitud en r1 ya que lo vamos a usar independientemente del salto
  ldr r1, [r8]
  cmp r0, r6
  beq patron_volteo_color_igual
  
  # Caso casilla != color
  # *longitud = *longitud + 1
  # ldr r1, [r8] (ya está cargado antes del branch)
  add r1, r1, #1
  str r1, [r8]
  # patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color)
  # r0 = tablero (r7), r1 = &longitud (r8), r2 = FA (r9), r3 = CA (r10)
  mov r0, r7
  mov r1, r8
  mov r2, r9
  mov r3, r10
  # Guardar en la pila SF (r4 -> sp), SC (r5 -> sp+4) y color (r6 -> sp+8) para la llamada recursiva
  push {r4-r6}
  bl patron_volteo_asm_c
  # Devolver el resultado que ha devuelto en r0
  b patron_volteo_callback
  
  # Caso casilla == color
  patron_volteo_color_igual:
  #- if (*longitud > 0)
  # ldr r1, [r8] (ya está cargado antes del branch)
  cmp r1, #0
  # Si longitud > 0 devuelve PATRON_ENCONTRADO, si no NO_HAY_PATRON
  movgt r0, #1
  movle r0, #0
  
  #----- Fin subrutina
  
  # Callback, r0 contiene el valor a devolver
  patron_volteo_callback:
  sub sp, fp, #40
  pop {r4-r10, fp, ip, lr}
  # No cargar el valor de PC y devolver el sp al que tenía al iniciar la función
  add sp, sp, #4
  bx lr

.end
