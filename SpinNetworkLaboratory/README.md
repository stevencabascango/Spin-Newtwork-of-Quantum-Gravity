# Spin Network Laboratory

**Simulador dinámico de redes topológicas en C++17 y SFML 2.6.**

Sistema computacional inspirado en redes de espín para construir, visualizar, modificar y analizar grafos topológicos dinámicos mediante estructuras de datos personalizadas, integración numérica e invariantes combinatorios. Las fuerzas son artificiales y sirven para organizar el dibujo; no se simula gravedad cuántica.

## Dos entregas

| Entrega | Semanas | Alcance |
|---|---|---|
| Workshop 1 — avance funcional | 1–6 | Lista simple, tabla hash, grafo, SFML, fuerzas y Velocity Verlet; interacción básica y pruebas. |
| Workshop 2 — producto final | 7–12 | Lista doble, historial C++, undo/redo, panel de invariantes, refinamiento local, optimización e informe final. |

La entrega actual es **Workshop 1**. La implementación SFML está compilada; su ejecución visual y el cierre de ventana deben verificarse en un terminal con acceso a pantalla. El entorno de desarrollo bloqueó X11 (`Operation not permitted`). No se presenta la entrega como completamente aceptada hasta realizar esa comprobación. El documento [validación](docs/validation.md) distingue pruebas ejecutadas y comprobaciones pendientes.

## Qué incluye el avance

- `SinglyLinkedList<T>`: `unique_ptr`, puntero de cola observador, inserción al inicio/final, búsqueda, eliminación, iteradores, limpieza y copia profunda.
- `HashTable<uint64_t, QuantumVertex>`: FNV-1a de 64 bits, encadenamiento propio, duplicados rechazados, factor de carga ≤ 0.75, rehash y copia profunda.
- `QuantumGraph`: grafo simple no dirigido, dos adyacencias por arista lógica, eliminación de aristas incidentes y referencias por `targetId`.
- `ForceEngine` y `VerletIntegrator`: atracción elástica, repulsión regularizada, amortiguamiento por separación de operadores y paso fijo de 1/120 s.
- Ventana SFML con círculos, segmentos, IDs, selección, arrastre sin salto, fijación, edición, pausa, parámetros y contadores.
- Red inicial de 20 nodos y configuración de 100 nodos; límite visual de 500. La tabla hash no tiene ese límite.
- Pruebas de estructuras, física e interacción; consola conservada como herramienta auxiliar.

La copia profunda y las métricas básicas ya existentes se conservan como preparación y complemento. No equivalen a haber implementado el Workshop 2.

## Ejecutar en Linux / WSL con escritorio

Necesitas GCC con C++17 y **SFML 2.6.x**. SFML 3 utiliza otra API y no es compatible con esta entrega.

Con SFML 2.6 instalado en el sistema:

```sh
./tools/build.sh
./bin/spin_network
```

Si tu distribución ofrece SFML 3, el script opcional descarga y extrae SFML 2.6.2 en `.deps/`, sin instalar paquetes globales (Linux x86_64, dependencias gráficas y FreeType disponibles):

```sh
./tools/fetch_sfml.sh
./tools/build.sh
./bin/spin_network
```

También puedes indicar una instalación propia con `SNL_SFML_ROOT=/ruta/a/sfml ./tools/build.sh`. El script compila y ejecuta las tres suites de pruebas antes de construir el programa.

Con CMake instalado:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/spin_network
```

Para las dependencias locales agrega `-DCMAKE_PREFIX_PATH="$PWD/.deps/sfml/usr"` al configurar y ejecuta con `LD_LIBRARY_PATH="$PWD/.deps/sfml/usr/lib/x86_64-linux-gnu" ./build/spin_network`.

Para probar estructuras y física sin SFML: `./tools/build.sh --core-only`, o CMake con `-DSNL_BUILD_DESKTOP=OFF`. La consola auxiliar es `./bin/spin_console`.

## Code::Blocks / Windows

Abre `SpinNetworkLaboratory.cbp`. Debug y Release construyen la aplicación SFML; Tests, PhysicsTests e InteractionTests construyen las suites; Console conserva la consola original.

1. Configura un compilador compatible con C++17.
2. Descarga SFML **2.6.x para MinGW**, con la misma arquitectura y versión de compilador de tu Code::Blocks. Si no existe un binario compatible, compila SFML con ese compilador.
3. En Project → Build options, agrega `SFML/include` a Compiler search directories y `SFML/lib` a Linker search directories. El proyecto ya enlaza `sfml-graphics`, `sfml-window` y `sfml-system`.
4. Copia las DLL correspondientes del SDK junto al ejecutable. Debug y Release están configurados con las bibliotecas normales; no cambies a las variantes `-d` sin cambiar también sus DLL.
5. Conserva `assets/fonts/` dentro del proyecto. La aplicación busca los recursos en la carpeta de trabajo y junto al ejecutable.
6. Build and Run (F9). Compila y ejecuta también los tres targets de pruebas.

La compilación y ejecución en Windows/Code::Blocks todavía requieren validación en ese equipo. Las instrucciones oficiales están en [SFML con Code::Blocks](https://www.sfml-dev.org/tutorials/2.6/getting-started/code-blocks/).

## Controles del Workshop 1

| Acción | Control |
|---|---|
| Seleccionar / arrastrar | Botón Seleccionar o `S`; mouse izquierdo |
| Agregar vértice | Botón Agregar y clic; `A` agrega en el puntero si está dentro del lienzo |
| Crear arista | Botón Conectar o `C`; clic en origen y destino |
| Eliminar vértice seleccionado | `Delete` |
| Eliminar nodo o arista bajo el puntero | Botón Eliminar o `D`, luego clic |
| Pausar / reanudar | `Space` o botón Pausar |
| Fijar / liberar seleccionado | `P` o botón Fijar |
| Reiniciar posiciones, conservando topología y fijaciones | `R` o botón Reiniciar |
| Aumentar / disminuir repulsión | `Up` / `Down` |
| Aumentar / disminuir rigidez | `]` / `[` |
| Aumentar / disminuir amortiguamiento | `.` / `,` |
| Red inicial / red de 100 nodos | `1` / `2` |
| Encuadrar sin alterar la geometría | `F` |
| Cerrar | `Esc` o botón de cierre |

Para la prueba visual automática (requiere escritorio):

```sh
./bin/spin_network --smoke-test
```

Abre 100 nodos, renderiza 120 frames, guarda `docs/screenshots/workshop1-sfml.png`, comprueba eventos de pausa y repulsión, y cierra la ventana. Esta prueba no pudo ejecutarse dentro del entorno restringido; aún debe correrse desde tu terminal normal. Complementa con el guion manual del documento de validación.

## Documentación y demostración web

- [Arquitectura y contratos](docs/architecture.md).
- [Plan de las dos entregas](docs/two-workshops.md).
- [Resultados y aceptación pendiente](docs/validation.md).
- Informe técnico de ocho páginas: `docs/report/workshop1.html` y `docs/report/workshop1.pdf`.

`site/dist/index.html` sigue siendo una demo independiente en JavaScript. Se puede abrir sin servidor o con `python3 tools/preview.py` en http://localhost:8000. Sus métricas e historial son una vista preliminar de funcionalidades de la segunda entrega; no ejecutan el núcleo C++ ni reemplazan la ventana SFML. Los cambios del grafo web viven en memoria y se pierden al recargar.

La descarga `site/dist/SpinNetworkLaboratory.zip` contiene los fuentes actualizados y la documentación. Los ejecutables locales viven en `bin/`; los binarios Linux no son ejecutables de Windows.

## Alcance del producto final

Workshop 2 implementará lista doble, snapshots topológicos (máximo 100), copia profunda auditada, undo/redo con eliminación de la rama futura, panel de β₀/β₁/χ/grado medio y refinamiento local de una arista `u-v` a `u-w-v` (ΔV=ΔE=1). El paralelismo es opcional. No son requisitos los movimientos Pachner 3D, CUDA, homología simplicial general ni gravedad cuántica completa.
