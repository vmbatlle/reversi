
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_iter.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de las funciones patron_volteo y ficha_valida en ARM, versión iterativa
# Versión:     1.0.0
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo

# Dimension del tablero (DIM x DIM casillas)
.equ DIM, 8

# Valor almacenado en memoria en una casilla (byte) del tablero para indicar que está vacía
.equ CASILLA_VACIA, 0

# Posibles valores que puede devolver patron_volteo_arm_arm
.equ PATRON_ENCONTRADO, 1
.equ NO_HAY_PATRON, 0

patron_volteo:

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
  bhi patron_volteo_callback

  # Caso: Posición dentro del tablero
  # Cargar en r7 el valor de tablero[f][c]
  add r7, r0, r2, lsl #3
  ldrb r7, [r7, r3]

  # Decidir qué hacer según el valor de tablero[f][c]
  cmp r7, #CASILLA_VACIA
  # Si tablero[f][c] no es CASILLA_VACIA, compararlo con color (r6)
  cmpne r7, r6

  # Caso: tablero[f][c] == CASILLA_VACIA || tablero[f][c] == color
  # En ambos casos hay que devolver NO_HAY_PATRON
  # Si tablero[f][c] == color, sabemos que longitud=0 y no hace falta compararlo (primera iteración)
  moveq r0, #NO_HAY_PATRON
  beq patron_volteo_callback

  # Caso: tablero[f][c] != CASILLA_VACIA && tablero[f][c] != color
  # *longitud = *longitud + 1
  # Cargar longitud ya que estamos en la primera iteración
  ldr r8, [r1]
  add r8, r8, #1



  #--- Segunda iteración (y posteriores)
  patron_volteo_loop:
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
  bhi patron_volteo_callback

  # Caso: Posición dentro del tablero
  # Cargar en r7 el valor de tablero[f][c]
  add r7, r0, r2, lsl #3
  ldrb r7, [r7, r3]

  # Decidir qué hacer según el valor de tablero[f][c]
  cmp r7, #CASILLA_VACIA

  # Caso: CASILLA_VACIA
  moveq r0, #NO_HAY_PATRON
  beq patron_volteo_callback
  # Comparar tablero[f][c] (r7) con color (r6)
  cmp r7, r6
  beq patron_volteo_color_igual

  # Caso: tablero[f][c] != color
  # *longitud = *longitud + 1
  # No hace falta cargar longitud
  add r8, r8, #1
  # Seguir iterando
  b patron_volteo_loop

  patron_volteo_color_igual:
  # Caso: tablero[f][c] == color
  # longitud ha sido modificada al menos una vez, es decir, es >0
  # Guardar el valor modificado
  str r8, [r1]
  mov r0, #PATRON_ENCONTRADO



  #----- Fin subrutina

  # Callback, r0 contiene el valor a devolver
  patron_volteo_callback:
  ldmdb fp, {r4-r8, fp, sp, pc}

.end
