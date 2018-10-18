
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_iter.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de las funciones patron_volteo y ficha_valida en ARM, versión iterativa
# Versión:     1.0.0
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo_arm_iter
.globl patron_volteo_arm_iter_v2

# Dimension del tablero (DIM x DIM casillas)
.equ DIM, 8

# Valor almacenado en memoria en una casilla (byte) del tablero para indicar que está vacía
.equ CASILLA_VACIA, 0

# Posibles valores que puede devolver patron_volteo_arm_arm
.equ PATRON_ENCONTRADO, 1
.equ NO_HAY_PATRON, 0



patron_volteo_arm_iter:

  #----- Inicio subrutina
  # r0 = &tablero
  # r1 = &longitud
  # r2 = FA
  # r3 = CA
  # *sp = SF
  # *(sp+4) = SC
  # *(sp+8) = color

  mov ip, sp
  push {r4-r7, fp, ip, lr, pc}
  sub fp, ip, #4
  # No hace falta guardar espacio para variables locales



  #----- Cuerpo de la subrutina
  # Cargar SF en r4, SC en r5 y color en r6
  ldmib fp, {r4-r6}

  patron_volteo_arm_iter_loop:
  # FA = FA + SF
  add r2, r2, r4
  and r2, r2, #255

  # CA = CA + SC
  add r3, r3, r5
  and r3, r3, #255

  #- if ((f < DIM) && (f >= 0) && (c < DIM) && (c >= 0) && (tablero[f][c] != CASILLA_VACIA))
  # Asumimos que fila y columna son unsigned char, es decir, no pueden ser < 0
  # Se compara con DIM - 1 para poder hacer uso de la comparación unsigned lower or same (ls)
  # f < DIM
  cmp r2, #DIM - 1
  # c < DIM
  cmpls r3, #DIM - 1

  # Caso: Posición no válida (fila o columna se salen del tablero)
  movhi r0, #NO_HAY_PATRON
  # Devolver NO_HAY_PATRON
  bhi patron_volteo_arm_iter_callback

  # Caso: Posición dentro del tablero
  # Cargar en r7 el valor de tablero[f][c]
  add r7, r0, r2, lsl #3
  ldrb r7, [r7, r3]

  # Decidir qué hacer según el valor de tablero[f][c]
  cmp r7, #CASILLA_VACIA

  # Caso: CASILLA_VACIA
  moveq r0, #NO_HAY_PATRON
  beq patron_volteo_arm_iter_callback
  # Comparar tablero[f][c] (r7) con color (r6)
  cmp r7, r6
  # Cargar longitud (factor común ya que se emplea en ambos caminos)
  ldr r7, [r1]
  beq patron_volteo_arm_iter_color_igual

  # Caso: tablero[f][c] != color
  # *longitud = *longitud + 1
  # ldr r7, [r1] (ya está cargado antes del branch)
  add r7, r7, #1
  str r7, [r1]

  # Seguir iterando
  b patron_volteo_arm_iter_loop

  # Caso: tablero[f][c] == color
  patron_volteo_arm_iter_color_igual:
  #- if (*longitud > 0)
  # ldr r7, [r1] (ya está cargado antes del branch)
  cmp r7, #0
  # Si longitud > 0 devuelve PATRON_ENCONTRADO, si no NO_HAY_PATRON
  movgt r0, #PATRON_ENCONTRADO
  movle r0, #NO_HAY_PATRON



  #----- Fin subrutina

  # Callback, r0 contiene el valor a devolver
  patron_volteo_arm_iter_callback:
  sub sp, fp, #28
  ldmia sp, {r4-r7, fp, sp, lr}
  bx lr


#------------------------------------------------------------------------


patron_volteo_arm_iter_v2:

  #----- Inicio subrutina
  # r0 = &tablero
  # r1 = &longitud
  # r2 = FA
  # r3 = CA
  # *sp = SF
  # *(sp+4) = SC
  # *(sp+8) = color

  mov ip, sp
  push {r4-r8, fp, ip, lr, pc}
  sub fp, ip, #4
  # No hace falta guardar espacio para variables locales



  #----- Cuerpo de la subrutina
  # Cargar SF en r4, SC en r5 y color en r6
  ldmib fp, {r4-r6}

  #--- Primera iteración
  # FA = FA + SF
  add r2, r2, r4
  and r2, r2, #255

  # CA = CA + SC
  add r3, r3, r5
  and r3, r3, #255

  #- if ((f < DIM) && (f >= 0) && (c < DIM) && (c >= 0) && (tablero[f][c] != CASILLA_VACIA))
  # Asumimos que fila y columna son unsigned char, es decir, no pueden ser < 0
  # Se compara con DIM - 1 para poder hacer uso de la comparación unsigned lower or same (ls)
  # f < DIM
  cmp r2, #DIM - 1
  # c < DIM
  cmpls r3, #DIM - 1

  # Caso: Posición no válida (fila o columna se salen del tablero)
  movhi r0, #NO_HAY_PATRON
  # Devolver NO_HAY_PATRON
  bhi patron_volteo_arm_iter_v2_callback

  # Caso: Posición dentro del tablero
  # Cargar en r7 el valor de tablero[f][c]
  add r7, r0, r2, lsl #3
  ldrb r7, [r7, r3]

  # Decidir qué hacer según el valor de tablero[f][c]
  cmp r7, #CASILLA_VACIA

  # Caso: CASILLA_VACIA
  moveq r0, #NO_HAY_PATRON
  beq patron_volteo_arm_iter_v2_callback
  # Comparar tablero[f][c] (r7) con color (r6)
  cmp r7, r6
  # Caso: tablero[f][c] == color
  # Salir de la función, sabemos que longitud=0 y no hace falta comparar
  beq patron_volteo_arm_iter_v2_callback

  # Caso: tablero[f][c] != color
  # *longitud = *longitud + 1
  # Cargar longitud ya que estamos en la primera iteración
  ldr r8, [r1]
  add r8, r8, #1



  #--- Segunda iteración (y posteriores)
  patron_volteo_arm_iter_v2_loop:
  # FA = FA + SF
  add r2, r2, r4
  and r2, r2, #255

  # CA = CA + SC
  add r3, r3, r5
  and r3, r3, #255

  #- if ((f < DIM) && (f >= 0) && (c < DIM) && (c >= 0) && (tablero[f][c] != CASILLA_VACIA))
  # Asumimos que fila y columna son unsigned char, es decir, no pueden ser < 0
  # Se compara con DIM - 1 para poder hacer uso de la comparación unsigned lower or same (ls)
  # f < DIM
  cmp r2, #DIM - 1
  # c < DIM
  cmpls r3, #DIM - 1

  # Caso: Posición no válida (fila o columna se salen del tablero)
  movhi r0, #NO_HAY_PATRON
  # Devolver NO_HAY_PATRON
  bhi patron_volteo_arm_iter_v2_callback

  # Caso: Posición dentro del tablero
  # Cargar en r7 el valor de tablero[f][c]
  add r7, r0, r2, lsl #3
  ldrb r7, [r7, r3]

  # Decidir qué hacer según el valor de tablero[f][c]
  cmp r7, #CASILLA_VACIA

  # Caso: CASILLA_VACIA
  moveq r0, #NO_HAY_PATRON
  beq patron_volteo_arm_iter_v2_callback
  # Comparar tablero[f][c] (r7) con color (r6)
  cmp r7, r6
  beq patron_volteo_arm_iter_v2_color_igual

  # Caso: tablero[f][c] != color
  # *longitud = *longitud + 1
  # No hace falta cargar longitud
  add r8, r8, #1
  # Seguir iterando
  b patron_volteo_arm_iter_v2_loop

  patron_volteo_arm_iter_v2_color_igual:
  # Caso: tablero[f][c] == color
  #- if (*longitud > 0)
  # ldr r8, [r1] (ya está cargado)
  cmp r8, #0
  # Guardar longitud en memoria (al menos ha sido modificado una vez)
  str r8, [r1]
  # Si longitud > 0 devuelve PATRON_ENCONTRADO, si no NO_HAY_PATRON
  movgt r0, #PATRON_ENCONTRADO
  movle r0, #NO_HAY_PATRON



  #----- Fin subrutina

  # Callback, r0 contiene el valor a devolver
  patron_volteo_arm_iter_v2_callback:
  sub sp, fp, #32
  ldmia sp, {r4-r8, fp, sp, lr}
  bx lr

.end
