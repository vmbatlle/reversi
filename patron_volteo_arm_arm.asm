
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_arm.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de las funciones patron_volteo y ficha_valida en ARM
# Versión:     0.0.1
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo_arm_arm

# TODO: añadir .equ
.equ DIM 7

# TODO: Se puede reusar r0-r3 al hacer la llamada recursiva pero hay que dejarlo
# muy muy claro el hecho de que lo hacemos porque no llamamos a ninguna funcion de
# fuera y sabemos que nadie nos lo va a modificar

patron_volteo_arm_arm:

  # TODO: Completar el codigo

  #----- Inicio subrutina
  # r0 = &tablero
  # r1 = &longitud
  # r2 = FA
  # r3 = CA
  # *sp = SF
  # *(sp+4) = SC
  # *(sp+8) = color
  
  mov ip, sp
  # TODO: guardar solo los registros r4-r10 necesarios
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
  # TODO: mover los and?
  and r2, r2, #255
  
  # CA = CA + SC
  add r3, r3, r5
  and r2, r2, #255
  
  #----- Fin subrutina
  
  # Callback, r0 contiene el valor a devolver
  patron_volteo_callback:
  # TODO: Solo desapilar los registros que hayamos apilado previamente
  ldmia sp, {r4-r10, fp, sp, pc}

.end