Proyecto Hardware (2018-19). Práctica 1
=======================================
Autores:  MARTÍNEZ BATLLE, Victor <736478@unizar.es>
           ROYO MENESES, Diego <740338@unizar.es>

Última modificación: 2018/10/21
Web del proyecto: https://github.com/vmbatlle/reversi

Aclaración de fuentes
---------------------
1. **main.c**: fichero fuente principal.

1. **patron_volteo_arm_c.asm**: Implementación en ensamblador manteniendo la llamada a la función en C (paso 3).

1. **patron_volteo_arm_arm.asm**: Implementación en ensamblador aplicando la técnica de _inlining_ de la función `ficha_valida(...)` dentro de `patron_volteo_arm_arm(...)` (paso 4).

1. **test_bench.c**: Biblioteca para verificación automática (paso 5).

1. **timer2.c**: Biblioteca para las mediciones de tiempo (paso 6 y 7).

1. **reversi8_2018.c**: Modificado para incluir la lógica de pruebas (`patron_volteo_test(...)` paso 5) y nuevas implementaciones de `patron_volteo(...)` en C (`patron_volteo_c_iter(...)`  y `patron_volteo_c_iter_inline` de los apartados opcionales).

1. **patron_volteo_arm_iter.asm**: Implementaciones en ensamblador cambiando la estrategia algorítmica por una iterativa (opcional).

Aclaración de funciones implementadas
-------------------------------------
### Optimizaciones de patron_volteo
1. `patron_volteo_arm_c(...)` en fichero patron_volteo_arm_c.asm.

1. `patron_volteo_arm_arm(...)` en fichero patron_volteo_arm_arm.asm.

1. `patron_volteo_c_iter(...)` del fichero fichero reversi8_2018.c es la versión iterativa en C del apartado opcional.

1. `patron_volteo_c_iter_inline(...)` del fichero reversi8_2018.c es una nueva versión iterativa en C, empleando la estrategia de _inlining_, al igual que aplica el compilador en -O2.

1. `patron_volteo_arm_iter(...)` del fichero patron_volteo_arm_iter.asm es una primera versión iterativa equivalente a `patron_volteo_c_iter_inline(...)`.

1. `patron_volteo_arm_iter_v2(...)` del fichero patron_volteo_arm_iter.asm es la versión más eficiente. La estrategia algorítmica equivale a `patron_volteo_c_iter_inline(...)`.

### Bibliteca de mediciones
1. `timer2_inicializar()`, `timer2_empezar()`, `timer2_leer()` y `timer2_parar()` en fichero timer2.c.

### Biblioteca de pruebas
Para las pruebas se unas un puntero a función `praton_volteo_implementacion`. Este puntero apunta a la función de la implementación que se desee probar en cada caso.

#### Prueba 1: TEST_BENCH_1
Sigue la estrategia propuesta en el guión de prácticas. Se invoca a `patron_volteo_test(...)` y ésta a su vez invoca a todas las implementaciones. Se ha aprovechado un generador de movimientos aleatorios para jugar una partida entera a reversi usando este método de pruebas.

_Se podrán dar más explicaciones sobre el generador aleatorio en la defensa del proyecto._

#### Prueba 2: TEST_BENCH_2
Estudia casos considerados críticos y aplica sobre ellos todas las implementaciones de `patron_volteo(...)`. Compara los resultados de todas las implementaciones.

Los casos se encuentran descritos en la biblioteca test_bench.c.

#### Prueba 3: TEST_BENCH_3
Mediante el generador de movimientos aleatorios se juega la misma partida completa con cada implementación de `patron_volteo(...)`. Al finalizar un juego se comparan los tablero para comprobar que, al aplicar los mismo movimientos de fichas negras, los tableros resultantes son iguales.

#### Prueba 4: TEST_BENCH_TIMER2
Pruebas de la biblioteca de medición de tiempo pedidas en el paso 7. Implementadas en main.c.

Anotaciones para el día de la defensa
-------------------------------------
1. _"El día de la corrección se debe realizar la verificación automática."_ utilizar TEST_BENCH_1. Descomentado `#define TEST_BENCH_1` y estableciendo _breakpoints_ dentro de la función `patron_volteo_test(...)` y al final de `reversi8()`.

1. _"Durante la corrección de la práctica
deberéis tener todo preparado para hacer estas mediciones."_ Utilizar TEST_BENCH_TIMER2 descomentando en `#define` en main.c.

1. Los datos de rendimiento se presentarán en formato impreso.
