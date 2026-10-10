# Validación del Workshop 1

Fecha: 6 de octubre de 2026. Entorno: Linux x86_64 / WSL, GCC 15.2, C++17, SFML 2.6.2 extraído localmente; CMake 3.28.3.

## Evidencia ejecutada

| Comprobación | Resultado |
|---|---|
| Compilación GCC con `-Wall -Wextra -Wpedantic` | Núcleo, consola, física, interacción y aplicación SFML compilados |
| Lista simple | pushFront/pushBack, búsqueda, contains, eliminación de cabeza/cola, reutilización, iteradores y copia/movimiento |
| Tabla hash | 5.000 QuantumVertex, búsquedas, rechazo de duplicados, rehash a 16.384 buckets y eliminación completa |
| Colisiones | Hash de prueba constante: 1.000 entradas en el mismo bucket; búsqueda y eliminación de cabeza/interior/cola |
| Grafo | 1.000 vértices, 5.000 aristas, rechazo de lazos/duplicados/extremos inexistentes, borrado incidente, copia y movimiento |
| Fuerzas | Resorte conocido y repulsión; pares iguales/opuestos; separación de nodos coincidentes |
| Verlet sin amortiguamiento | Error relativo de energía < 0.1% tras 1.200 pasos de un oscilador |
| Amortiguamiento | Energía decreciente y velocidad libre conforme a exp(-γt/m) |
| Fijación | Posición constante y velocidad cero |
| Paso fijo | Mismo estado tras un segundo a 30, 60 y 144 FPS; pausa sin acumulación; recuperación limitada a 12 pasos |
| Estabilidad | 100 nodos, 261 aristas, 7.200 pasos (60 s simulados), coordenadas finitas y acotadas |
| Rendimiento de referencia | Aproximadamente 0.19–0.23 ms/paso en la ejecución optimizada local; no incluye renderizado |
| Controlador | Creación/conexión/eliminación, conexiones inválidas, arrastre con offset, pausa, fijación, reinicio de posiciones y límite de 500 nodos |
| Sanitizadores | Tres suites pasan AddressSanitizer + UndefinedBehaviorSanitizer con `detect_leaks=0` |
| Demo web | Pruebas Node y controles comprobados en Brave; es un programa independiente |

Los tiempos dependen del equipo. Los tests de controlador prueban la lógica con coordenadas del grafo; no acreditan entrega de eventos por el sistema operativo.

## Limitaciones observadas

Al ejecutar `./bin/spin_network --smoke-test`, SFML no pudo abrir X11. La conexión al socket de pantalla devuelve `Operation not permitted` dentro del sandbox. La aplicación está enlazada, pero no se afirma haber observado su ventana ni haber obtenido una captura SFML.

LeakSanitizer abortó con “does not work under ptrace”. Las suites se verificaron de nuevo con detección de fugas desactivada. AddressSanitizer y UndefinedBehaviorSanitizer no reportaron errores; esto no demuestra por sí solo ausencia de fugas. La propiedad se revisó: nodos propietarios usan `unique_ptr`, la cola y enlaces de observación no poseen memoria.

Code::Blocks y SFML en Windows necesitan pruebas en el equipo de presentación. El ejecutable local es Linux x86_64.

## Repetir las pruebas

```sh
./tools/build.sh
node tests/test_web.cjs
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Para los sanitizadores (terminal sin ptrace, Linux):

```sh
mkdir -p bin/sanitized
for suite in core physics interaction; do
  g++ -std=c++17 -O1 -g -fsanitize=address,undefined \
    -fno-omit-frame-pointer -I include tests/test_${suite}.cpp \
    -o bin/sanitized/test_${suite}
  ASAN_OPTIONS=detect_leaks=1 ./bin/sanitized/test_${suite}
done
```

## Comprobación visual pendiente

Desde un terminal normal con escritorio:

1. Ejecuta `./bin/spin_network --smoke-test`. Debe abrir 100 nodos, guardar `docs/screenshots/workshop1-sfml.png`, imprimir PASS y cerrarse.
2. Ejecuta `./bin/spin_network` y pulsa `Space`: la red debe quedar inmóvil; reanuda.
3. Pulsa `2`: deben aparecer 100 nodos / 261 aristas. Observa la red durante un minuto.
4. Selecciona un nodo, pulsa `P` y comprueba que permanece fijo. Pulsa `P` otra vez.
5. Arrastra desde el borde de un nodo: debe conservar el punto de agarre. Suelta y verifica que vuelve a responder a la física.
6. Agrega un nodo con `A` dentro del lienzo. Usa `C` y elige origen y destino. Repite la conexión y prueba un lazo: el conteo no debe aumentar.
7. Selecciona un nodo conectado y pulsa `Delete`: su contador y las aristas incidentes deben disminuir. Con `D`, elimina una arista por su punto medio.
8. Cambia `Up/Down`, `[/]`, `,/.` y comprueba los parámetros del HUD. Con `R`, deben reiniciarse posiciones sin cambiar V/E ni fijaciones.
9. Pulsa `F`: cambia el encuadre, no las coordenadas del grafo. Redimensiona la ventana y vuelve a seleccionar/arrastrar.
10. Cierra, abre de nuevo y cierra varias veces. Ejecuta las pruebas de fugas en un entorno compatible. Sólo entonces marca cerrada la aceptación visual y de memoria.

La captura incluida en el informe durante esta preparación muestra la demo web y está identificada como tal. Sustituirla o complementarla con la captura SFML después de ejecutar la prueba visual.
