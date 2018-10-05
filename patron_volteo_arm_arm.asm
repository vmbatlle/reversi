
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_arm.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de las funciones patron_volteo y ficha_valida en ARM
# Versión:     1.0.0
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo_arm_arm

.equ DIM, 7

.equ CASILLA_VACIA, 0

.equ PATRON_ENCONTRADO, 1
.equ NO_HAY_PATRON, 0

# TODO: Se puede reusar r0-r3 al hacer la llamada recursiva pero hay que dejarlo
# muy muy claro el hecho de que lo hacemos porque no llamamos a ninguna funcion de
# fuera y sabemos que nadie nos lo va a modificar

patron_volteo_arm_arm:

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
  
  # FA = FA + SF
  add r2, r2, r4
  # TODO: mover los and?
  and r2, r2, #255
  
  # CA = CA + SC
  add r3, r3, r5
  and r2, r2, #255
  
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
  ldr r7, [r7, r3]
  # Decidir qué hacer según el valor de tablero[f][c]
  cmp r7, #CASILLA_VACIA
  # Caso: CASILLA_VACIA
  moveq r0, #NO_HAY_PATRON
  beq patron_volteo_callback
  # Comparar tablero[f][c] (r7) con color (r6)
  cmp r7, r6
  # Cargar longitud (factor común ya que se emplea en ambos caminos)
  ldr r7, [r2]
  beq patron_volteo_color_igual


  # Caso tablero[f][c] != color
  # *longitud = *longitud + 1
  # ldr r7, [r2] (ya está cargado antes del branch)
  add r7, r7, #1
  str r7, [r2]
  # patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color)
  # No hace falta guardar &tablero ya que es recursividad final
  # Guardar en la pila SF (r4 -> sp), SC (r5 -> sp+4) y color (r6 -> sp+8) para la llamada recursiva
  push {r4-r6}
  bl patron_volteo_arm_arm
  # Devolver el resultado que ha devuelto en r0
  b patron_volteo_callback

  # Caso casilla == color
  patron_volteo_color_igual:
  #- if (*longitud > 0)
  # ldr r7, [r2] (ya está cargado antes del branch)
  cmp r7, #0
  # Si longitud > 0 devuelve PATRON_ENCONTRADO, si no NO_HAY_PATRON
  movgt r0, #PATRON_ENCONTRADO
  movle r0, #NO_HAY_PATRON




  #----- Fin subrutina
  
  # Callback, r0 contiene el valor a devolver
  patron_volteo_callback:
  sub sp, fp, #24
  ldmia sp, {r4-r7, fp, sp, lr}
  bx lr

.end
