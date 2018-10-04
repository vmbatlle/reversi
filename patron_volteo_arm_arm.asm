
#----------------------------------------------------------------------------------------
# Fichero:     patron_volteo_arm_arm.asm
# Autores:     Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
# Descripción: Implementación de las funciones patron_volteo y ficha_valida en ARM
# Versión:     0.0.1
#----------------------------------------------------------------------------------------

.section .text
.globl patron_volteo_arm_arm

patron_volteo_arm_arm:

  # TODO: Completar el codigo

  ##----- Inicio subrutina
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
  
  #----- Fin subrutina
  
  # Callback, r0 contiene el valor a devolver
  patron_volteo_callback:
  # TODO: restar la cantidad necesaria y solo desapilar los registros que hayamos apilado previamente
  sub sp, fp, #40
  pop {r4-r10, fp, ip, lr}
  # No cargar el valor de PC y devolver el sp al que tenía al iniciar la función
  add sp, sp, #4
  bx lr

.end