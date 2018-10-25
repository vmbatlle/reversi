
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_arm.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de las funciones patron_volteo y ficha_valida en ARM
# Versión:     1.0.0
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo_arm_arm

# Dimension del tablero (DIM x DIM casillas)
.equ DIM, 8

# Valor almacenado en memoria en una casilla (byte) del tablero para indicar que está vacía
.equ CASILLA_VACIA, 0

# Posibles valores que puede devolver patron_volteo_arm_arm
.equ PATRON_ENCONTRADO, 1
.equ NO_HAY_PATRON, 0



patron_volteo_arm_arm:

  #----- Inicio subrutina
  # r0 = &tablero
  # r1 = &longitud
  # r2 = FA
  # r3 = CA
  # *sp = SF
  # *(sp+4) = SC
  # *(sp+8) = color
  
  #-- Creación del bloque de activación
  # Apilar los registros que se modifican en la subrutina junto con sp, lr y pc
  mov ip, sp
  push {r4-r7, fp, ip, lr, pc}
  # Apuntar el fp al primero de los registros apilado anteriormente (pc)
  sub fp, ip, #4
  # No hace falta guardar espacio para variables locales
  
  # El bloque de activación queda así:
  #
  # (-16/-28) |______r4-r7______| <- sp
  #     (-12) |_______fp________|
  #      (-8) |_______ip________|
  #      (-4) |_______lr________|
  #       (0) |_______pc________| <- fp
  #      (+4) |_______SF________| <- ip
  #      (+8) |_______SC________|
  #     (+12) |______color______|

  #----- Cuerpo de la subrutina
  # Cargar los parámetros pasados por pila
  ldmib fp, {r4-r6}

  # Uso de los registros:
  # r0: &tablero     r4: SF
  # r1: &longitud    r5: SC
  # r2: FA           r6: color
  # r3: CA           r7: tablero[FA][CA] y longitud
  
  patron_volteo_sumas:
  # FA = FA + SF
  add r2, r2, r4
  and r2, r2, #255
  
  # CA = CA + SC
  add r3, r3, r5
  and r3, r3, #255
  
  patron_volteo_llamada_ficha_valida:
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
  # Cargar longitud (factor común ya que se emplea en ambos caminos)
  ldr r7, [r1]
  # Decidir qué hacer según el resultado de la comparación anterior
  beq patron_volteo_color_igual

  # Caso: tablero[f][c] != color
  patron_volteo_color_diferente:
  # *longitud = *longitud + 1
  add r7, r7, #1
  str r7, [r1]
  # patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color)
  # Apilar los tres últimos parámetros en la pila para la llamada recursiva
  push {r4-r6}
  # IMPORTANTE: Deberían guardarse los valores de r0-r3, pero no se hace ya que esta subrutina solamente
  # se llama a sí recursivamente y en ella no se modifican en ningun momento los valores de r0-r3
  # (excepto r0, &tablero, para devolver el resultado de la función, pero en este caso
  #  no importa ya que es recursividad final)
  bl patron_volteo_arm_arm
  # Devolver el resultado que ha devuelto en r0
  b patron_volteo_callback

  # Caso: tablero[f][c] == color
  patron_volteo_color_igual:
  #- if (*longitud > 0)
  cmp r7, #0
  # Si longitud > 0 devuelve PATRON_ENCONTRADO, si no NO_HAY_PATRON
  movgt r0, #PATRON_ENCONTRADO
  movle r0, #NO_HAY_PATRON



  #----- Fin subrutina
  
  # Callback, r0 contiene el valor a devolver
  patron_volteo_callback:
  #-- Destrucción del bloque de activación y retorno
  ldmdb fp, {r4-r7, fp, sp, pc}

.end
