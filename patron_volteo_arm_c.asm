
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_c.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de la función patron_volteo en ARM
# Versión:     1.0.0
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo_arm_c
.extern ficha_valida

# Posibles valores que puede devolver patron_volteo_arm_c
.equ PATRON_ENCONTRADO, 1
.equ NO_HAY_PATRON, 0



patron_volteo_arm_c:

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
  push {r4-r10, fp, ip, lr, pc}
  # Apuntar el fp al primero de los registros apilado anteriormente (pc)
  sub fp, ip, #4
  # Solo hace falta guardar espacio para posicion_valida
  # casilla y patron no necesitan almacenarse en memoria, solo registros
  sub sp, sp, #4
  
  # El bloque de activación queda así:
  #
  #     (-44) |_posicion_valida_| <- sp
  # (-16/-40) |_____r4-r10______|
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

  # Uso de los registros (en esta parte):
  # r0: &tablero     r4: SF
  # r1: &longitud    r5: SC
  # r2: FA           r6: color
  # r3: CA
  
  patron_volteo_sumas:
  # FA = FA + SF
  add r2, r2, r4
  # Para restar 1 suma 255, por lo tanto solo los 8 primeros bits de r2 son válidos
  and r2, r2, #255
  
  # CA = CA + SC
  add r3, r3, r5
  and r3, r3, #255
  
  patron_volteo_llamada_ficha_valida:
  # casilla = ficha_valida(tablero, FA, CA, &posicion_valida)
  # Salvaguardar el valor de los registros r0-r3 en r7-r10
  mov r7, r0        /* &tablero */
  mov r8, r1        /* &longitud */
  mov r9, r2        /* FA */
  mov r10, r3       /* CA */
  # Pasar los parámetros de ficha_valida en r0-r3
  # r0 ya tiene &tablero
  mov r1, r2        /* FA */
  mov r2, r3        /* CA */
  sub r3, fp, #44   /* &posicion_valida */
  bl ficha_valida
  
  # Uso de los registros (en esta parte):
  # r0: casilla                                   r6: color
  # r1: <undef> (longitud más adelante)           r7: &tablero
  # r2: <undef>                                   r8: &longitud
  # r3: <undef> (posicion_valida más adelante)    r9: FA
  # r4: SF                                       r10: CA
  # r5: SC

  #--- Bloque condicional principal
  #- if ((posicion_valida == 1) && (casilla != color)) {
  #- } else if ((posicion_valida == 1 && (casilla == color)) {
  #- } else { }
  patron_volteo_bloque_condicional:
  # Cargar posicion_valida en r3 y compararlo con 1 para ver qué caso escoger
  ldr r3, [fp, #-44]
  cmp r3, #1

  # Caso: posicion_valida != 1 (correspondiente al else final)
  patron_volteo_posicion_valida_else:
  movne r0, #NO_HAY_PATRON
  bne patron_volteo_callback  /* return NO_HAY_PATRON; */
  
  # Caso: posicion_valida == 1 (correspondiente a los dos primeros casos del if)
  patron_volteo_posicion_valida_1:
  # Cargar longitud en r1 (se empleará más adelante)
  ldr r1, [r8]
  # Comparación de casilla (r0) con color (r6) para ver qué caso escoger
  cmp r0, r6
  beq patron_volteo_color_igual


  # Caso: casilla != color (correspondiente al primer caso del bloque if)
  patron_volteo_color_diferente:
  # *longitud = *longitud + 1
  # Sumar uno y almacenarlo en &longitud
  add r1, r1, #1
  str r1, [r8]
  # patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color)
  # Paso de parámetros y creación del marco de pila para la llamada recursiva
  # Pasar los cuatro primeros parámetros en r0-r3 (ver uso de los registros)
  mov r0, r7    /* &tablero */
  mov r1, r8    /* &longitud */
  mov r2, r9    /* FA */
  mov r3, r10   /* CA */
  # Apilar los tres últimos parámetros en la pila
  push {r4-r6}  /* SF, SC, color */
  bl patron_volteo_arm_c
  # r0 contiene el resultado a devolver
  b patron_volteo_callback /* return patron; */
  
  # Caso casilla == color (correspondiente al segundo caso del bloque if)
  patron_volteo_color_igual:
  #- if (*longitud > 0)
  cmp r1, #0
  # Si longitud > 0 devuelve PATRON_ENCONTRADO, si no NO_HAY_PATRON
  movgt r0, #PATRON_ENCONTRADO /* return PATRON_ENCONTRADO; */
  movle r0, #NO_HAY_PATRON     /* return NO_HAY_PATRON; */


  
  #----- Fin subrutina
  
  # Callback, r0 contiene el valor a devolver
  patron_volteo_callback:
  #-- Destrucción del bloque de activación
  # Devolver el valor original a los registros,
  # deshacer el bloque de activación de la subrutina
  # y retornar al código correspondiente
  ldmdb fp, {r4-r10, fp, sp, pc}

.end
