#ifndef __BOTONES_ANTIREBOTES_H__
#define __BOTONES_ANTIREBOTES_H__

/*
 * TODO
 * Estado de la máquina de estados:
 */
enum estado_rebotes { unpressed, wait_trp, pressed, wait_release, wait_trd, MAX_STATES };

/* TODO cambiar los nombres de los enum */

enum pulsacion {pulsacion_none, pulsacion_iz, pulsacion_dr};

/* TODO cabecera */
void antirebotes_iniciar(void);
enum pulsacion antirebotes_gestionar(void);

#endif /* __BOTONES_ANTIREBOTES_H__ */
