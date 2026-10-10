# Plan de dos entregas

Proyecto: **Spin Network Laboratory — Simulador dinámico de redes topológicas en C++ y SFML**.

## Workshop 1: avance funcional, semanas 1–6

| Semana | Trabajo | Evidencia |
|---|---|---|
| 1 | Diseño de propiedad, arquitectura, Code::Blocks y SFML | Diagrama, configuración, programa compilable |
| 2 | Lista simple propia | Inserción, búsqueda, eliminación, iteradores y limpieza |
| 3 | Hash FNV-1a, encadenamiento y rehash | 5.000 vértices, colisiones forzadas, factor de carga |
| 4 | Modelo de vértices, aristas y grafo | Simetría, duplicados/lazos rechazados, borrado incidente |
| 5 | SFML, renderizado y controles | Ventana, IDs, edición, arrastre, fijación y pausa |
| 6 | Fuerzas, Verlet, pruebas e informe | 100 nodos estables, independencia de FPS, informe de 8 páginas |

La aplicación SFML es obligatoria para aceptar esta entrega. Una demo web o consola no sustituye esa evidencia. El código está implementado y compilado; la prueba de pantalla sigue pendiente porque el entorno restringido no permite conectarse a X11. No registrar “cierre sin fugas” hasta realizar las comprobaciones correspondientes.

## Workshop 2: producto final, semanas 7–12

| Semana | Trabajo | Criterio de salida |
|---|---|---|
| 7 | Auditar copia profunda y regla de cinco | Copias independientes; movimientos y asignaciones comprobados |
| 8 | Lista doble, estados, undo y redo | Navegación bidireccional y eliminación de rama futura |
| 9 | Componentes, ciclos, Euler y grado medio | Árbol, ciclo, dos ciclos desconectados y grafo vacío probados |
| 10 | Panel analítico, selección avanzada y refinamiento | Refinamiento conserva χ; panel y controles integrados en C++ |
| 11 | Optimización y análisis de memoria | Benchmarks reproducibles, pruebas de memoria y límite de historial |
| 12 | Integración e informe final | Demostración completa, ejecutable y presentación |

`DoublyLinkedList<T>` poseerá el siguiente nodo con `unique_ptr`; `previous` será observador. Cada `TopologicalState` poseerá su propia copia de `QuantumGraph`. Un cursor navegará entre estados y una edición posterior a undo eliminará los estados futuros. Máximo: 100 estados. Sólo los cambios topológicos crean snapshots; ni física ni arrastre crean estados.

El refinamiento obligatorio reemplaza una arista por dos mediante un nuevo vértice. No se denomina movimiento Pachner: el modelo obligatorio es un grafo 1D sin caras ni tetraedros.

El proyecto ya conserva copias profundas y funciones de métricas del núcleo original. Su existencia adelanta bases del segundo workshop, pero faltan la lista doble, historial y panel final C++. El historial JavaScript tampoco acredita esos componentes.

## Aceptación de la primera entrega

- [x] Lista simple con API mínima y propiedad `unique_ptr`.
- [x] Hash propio: 5.000 vértices, búsqueda, duplicados, colisiones y rehash probados.
- [x] Grafo: simetría y eliminación de aristas incidentes probadas.
- [x] Física C++: 100 nodos / 261 aristas durante 7.200 pasos, sin posiciones o velocidades no finitas.
- [x] Paso fijo: resultados equivalentes a 30, 60 y 144 FPS.
- [x] Edición, arrastre, fijación y pausa probados en el controlador.
- [x] Aplicación SFML compilada con GCC y CMake.
- [x] Informe técnico de ocho páginas.
- [ ] Ejecutar SFML en el escritorio, verificar todos los controles y guardar captura real.
- [ ] Ejecutar comprobación de fugas en un entorno compatible; LeakSanitizer no funciona bajo ptrace aquí.
- [ ] Validar Code::Blocks/Windows si ése es el equipo de exposición.

El avance está implementado; su aceptación visual y de fugas no está cerrada. Workshop 2 es el siguiente trabajo, no parte de esta entrega.
