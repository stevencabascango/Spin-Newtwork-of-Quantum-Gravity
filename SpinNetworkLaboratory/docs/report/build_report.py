#!/usr/bin/env python3
"""Generate a self-contained, eight-page technical report (print to PDF)."""
from pathlib import Path
import base64
import html

ROOT = Path(__file__).resolve().parents[2]
shot = ROOT / 'docs/screenshots/workshop1-sfml.png'
native = shot.exists()
if not native:
    shot = ROOT / 'docs/screenshots/workshop1-web.png'
image = 'data:image/png;base64,' + base64.b64encode(shot.read_bytes()).decode() if shot.exists() else ''
caption = ('Ventana SFML: captura producida por --smoke-test.' if native else
           'Captura real de la demo web en Brave. Es JavaScript independiente y no acredita ejecución de SFML.')

pages = [
('Avance funcional', '''
<p class="eyebrow">WORKSHOP 1 · SEMANAS 1–6</p>
<h1>Spin Network<br>Laboratory</h1>
<p class="subtitle">Simulador dinámico de redes topológicas<br>en C++17 y SFML 2.6</p>
<div class="callout">Sistema computacional inspirado en redes de espín para construir, visualizar, modificar y analizar grafos topológicos dinámicos mediante estructuras de datos personalizadas, integración numérica e invariantes combinatorios.</div>
<h2>Problema y propósito</h2>
<p>Una red contiene vértices y relaciones. Sus operaciones deben mantener una representación consistente al insertar, conectar, eliminar o redimensionar el almacenamiento. El proyecto conecta esas operaciones con un dibujo dinámico: cada edición produce un cambio visible y sus efectos pueden inspeccionarse.</p>
<p>El objetivo académico es demostrar estructuras propias con gestión clara de memoria, no sólo utilizar un contenedor ya implementado. La tabla hash almacena los vértices; las listas simplemente enlazadas almacenan sus conexiones. Una aplicación SFML procesa entrada, presenta el grafo y permite experimentar con fuerzas artificiales.</p>
<h2>Alcance científico</h2>
<p>“QuantumVertex” es un nombre conceptual para un vértice etiquetado. Los espines son etiquetas semienteras positivas. No se imponen reglas de acoplamiento SU(2), ni se modelan amplitudes cuánticas. Las fuerzas organizan la visualización y no representan la dinámica real de gravedad cuántica.</p>
<table><tr><th>Entrega</th><th>Resultado</th></tr><tr><td>Workshop 1 · semanas 1–6</td><td>Grafo editable, física y renderizado SFML.</td></tr><tr><td>Workshop 2 · semanas 7–12</td><td>Historial C++, invariantes, refinamiento y UI final.</td></tr></table>
<p class="small">Equipo: completar con los integrantes reales. Repositorio GitHub: URL no proporcionada. Documento preparado el 6 de octubre de 2026.</p>
'''),
('Arquitectura y modelo de memoria', '''
<h1>Una propiedad<br>explícita</h1>
<p>La aplicación de escritorio separa almacenamiento, dinámica y presentación. El núcleo y el controlador no incluyen SFML: sus pruebas se ejecutan sin crear una ventana.</p>
<pre>DesktopApplication ── InputController / GraphRenderer / HUD
        │
        └── SimulationController
                ├── QuantumGraph
                │     └── HashTable&lt;uint64_t, QuantumVertex&gt;
                │            └── QuantumVertex
                │                  └── SinglyLinkedList&lt;SpinEdge&gt;
                └── ForceEngine / VerletIntegrator</pre>
<h2>Propietarios y observadores</h2>
<p>Los nodos de la tabla y de la lista poseen su enlace siguiente mediante <code>std::unique_ptr</code>. La tabla posee cada valor y cada bucket; el vértice posee su lista de adyacencia. La cola de la lista es un puntero observador: no libera memoria. No se utilizan <code>new</code> ni <code>delete</code> manuales para gestionar nodos.</p>
<p>Una arista contiene <code>targetId</code> y parámetros, no un puntero al vértice destino. Rehash puede reconstruir los vértices en otras direcciones; las conexiones siguen siendo válidas porque se resuelven mediante IDs. El controlador también conserva IDs para selección, conexión y arrastre.</p>
<h2>Tiempo de vida</h2>
<p><code>findVertex</code> devuelve un observador. Su validez termina al eliminar ese vértice, reconstruir la tabla o asignar otro grafo. El motor de fuerzas utiliza observadores únicamente durante una evaluación sin cambios topológicos. Ninguno se almacena entre frames.</p>
<p>La limpieza recorre y desmonta las cadenas iterativamente; evita una destrucción recursiva proporcional a la longitud. Las copias de la lista y la tabla son profundas: cada copia obtiene nodos internos propios. Los objetos movidos se pueden reutilizar.</p>
<h2>Organización del código</h2>
<table><tr><th>Carpeta</th><th>Responsabilidad</th></tr><tr><td>include/DataStructures</td><td>Lista simple y hash propia.</td></tr><tr><td>include/Core y Physics</td><td>Modelo, vectores, fuerzas e integrador.</td></tr><tr><td>include/Application y Graphics</td><td>Controlador, ventana, entrada y HUD.</td></tr><tr><td>src / tests / assets / docs</td><td>Implementaciones SFML, pruebas, fuente tipográfica e informe.</td></tr></table>
<p>El uso de STL como infraestructura —buckets, buffers y ordenamiento— no reemplaza las estructuras enlazadas y hash que se evalúan.</p>
'''),
('Lista simplemente enlazada', '''
<h1>Adyacencias<br>sin ownership ambiguo</h1>
<p><code>SinglyLinkedList&lt;T&gt;</code> implementa la colección de conexiones de cada vértice. Cada nodo contiene un valor y un <code>unique_ptr</code> al siguiente. La lista mantiene cabeza propietaria, cola observadora y contador de elementos.</p>
<pre>head (unique_ptr)
  └─ [valor | next] ── [valor | next] ── [valor | null]
                                              ↑
                                          tail (observador)</pre>
<table><tr><th>Operación</th><th>Contrato / costo</th></tr><tr><td>pushFront(value)</td><td>Inserta en cabeza; actualiza cola si estaba vacía. O(1).</td></tr><tr><td>pushBack(value)</td><td>Enlaza desde cola y actualiza contador. O(1).</td></tr><tr><td>find / contains</td><td>Recorren por igualdad; find devuelve observador o null. O(n).</td></tr><tr><td>remove / removeIf</td><td>Eliminan la primera coincidencia y reparan cola. O(n).</td></tr><tr><td>clear / size</td><td>Limpieza iterativa O(n), consulta O(1).</td></tr><tr><td>begin / end</td><td>Iteración mutable o constante sin exponer los nodos.</td></tr></table>
<h2>Eliminación y cola</h2>
<p>La eliminación modifica el enlace propietario que apunta al nodo: puede ser la cabeza o el <code>next</code> del anterior. El siguiente nodo se mueve a ese enlace. Si el nodo eliminado era la cola, la cola pasa al anterior; si era el único elemento, queda nula.</p>
<p>La variante <code>removeIf</code> permite eliminar una arista por identificador de destino sin exigir igualdad completa entre todos sus parámetros. La búsqueda por valor requiere un tipo T comparable sólo cuando se llama a esas operaciones.</p>
<h2>Copia y movimiento</h2>
<p>El constructor de copia crea una lista temporal y copia valores con <code>pushBack</code>. Al completar, intercambia su contenido. La asignación por valor e intercambio evita aliasing entre cadenas. El movimiento transfiere ownership y deja la fuente vacía y reutilizable.</p>
<h2>Pruebas del avance</h2>
<p>Se verifican inserción al frente y al final, búsqueda constante y mutable, ausencia, eliminación de cabeza, interior y cola, reinserción después de vaciar, iteradores y copia independiente. Se construye además una cadena de 10.000 elementos, se mueve, se reutiliza el origen y se limpia.</p>
<div class="callout">La cola nunca es propietaria. La memoria pertenece a la cadena de <code>unique_ptr</code> iniciada en la cabeza.</div>
'''),
('Tabla hash, colisiones y rehash', '''
<h1>Búsqueda por ID<br>con hash propia</h1>
<p><code>HashTable&lt;uint64_t, QuantumVertex&gt;</code> usa encadenamiento separado. El arreglo de buckets pertenece a la tabla; cada bucket inicia una cadena propia de nodos clave/valor. Identificadores duplicados se rechazan sin sobrescribir el valor original.</p>
<h2>FNV-1a de 64 bits</h2>
<div class="equation">h₀ = 14695981039346656037<br>hᵢ₊₁ = (hᵢ XOR bᵢ) × 1099511628211 (mod 2⁶⁴)<br>index(key) = hash(key) mod capacity</div>
<p>Las claves enteras se serializan en bytes little-endian. Así el resultado no depende del endianness del procesador. Una prueba verifica que FNV-1a de “hello” produce <code>0xa430d84680aabd0b</code>. También se admiten cadenas e inyección de un hash alternativo para probar colisiones.</p>
<h2>Encadenamiento</h2>
<pre>bucket[0] → [clave, valor] → [clave, valor] → null
bucket[1] → null
bucket[2] → [clave, valor] → null</pre>
<p>Insertar, buscar y eliminar recorren únicamente la cadena correspondiente. El costo esperado es O(1) con distribución adecuada; una cadena patológica puede requerir O(n). La prueba de colisiones usa una función constante: 1.000 claves distintas caen deliberadamente en el mismo bucket.</p>
<h2>Factor de carga y reconstrucción</h2>
<div class="equation">α = elementos / buckets &nbsp;&nbsp; objetivo: α ≤ 0.75</div>
<p>Antes de insertar una clave que supere el umbral, se amplía la tabla. Rehash construye una tabla temporal y copia los valores. Sólo después de terminar intercambia tablas. Si una asignación de memoria falla, la tabla original no cambia.</p>
<p>Esta garantía tiene costo: copiar un vértice incluye copiar sus adyacencias. Rehash es O(V+E) esperado para este grafo; con hashes patológicos, las búsquedas de duplicados en las cadenas pueden llevarlo a O(V²). No se conserva un observador durante esta operación.</p>
<h2>Evidencia</h2>
<p>Se insertan 5.000 vértices, se buscan todos, se rechazan duplicados, se fuerza rehash a 16.384 buckets y se eliminan todas las entradas. La prueba de colisiones incluye borrado de cabeza, interior y cola y reutilización de tablas copiadas/movidas.</p>
'''),
('Modelo del grafo y sus contratos', '''
<h1>Una arista lógica,<br>dos adyacencias</h1>
<p><code>QuantumGraph</code> representa un grafo simple no dirigido. Al conectar a y b crea un registro a→b y otro b→a, con el mismo ID lógico y los mismos parámetros. El contador global aumenta una sola vez.</p>
<table><tr><th>QuantumVertex</th><th>SpinEdge</th></tr><tr><td>ID, masa, volumen</td><td>ID de arista y targetId</td></tr><tr><td>Posición y posición anterior</td><td>Espín positivo semientero</td></tr><tr><td>Velocidad, aceleración, fuerza</td><td>Longitud natural y rigidez</td></tr><tr><td>pinned y lista de conexiones</td><td>Destino resuelto por ID</td></tr></table>
<h2>Validaciones</h2>
<p>Se rechazan vértices con posición no finita o masa no positiva. La sobrecarga que recibe un vértice completo exige además estado físico válido y adyacencias vacías: las conexiones sólo se crean por la API del grafo. Las aristas rechazan parámetros inválidos, lazos, duplicados y extremos ausentes.</p>
<p>Si falla la creación de la segunda adyacencia por falta de memoria, se retira la primera antes de propagar la excepción. El contador no se incrementa hasta completar ambas. Esto mantiene el contrato no dirigido.</p>
<h2>Eliminar un vértice</h2>
<p>Primero se obtiene una lista de IDs vecinos. Luego se elimina cada arista recíproca y se reduce el contador lógico. Finalmente se elimina el vértice de la tabla. Los demás vértices no conservan referencias a un ID que desapareció.</p>
<h2>API pública</h2>
<pre>addVertex / removeVertex / findVertex / containsVertex
addEdge / removeEdge / hasEdge
vertexCount / numberOfEdges / degree / clear</pre>
<p>Las listas de adyacencia se modifican únicamente mediante la API del grafo, nunca directamente desde un observador mutable.</p>
<h2>Pruebas</h2>
<p>Un grafo de 1.000 vértices y 5.000 aristas verifica simetría y conteo. Cada nodo tiene grado 10. Al eliminar el vértice 0 se retiran sus 10 aristas; ningún vecino conserva una conexión a él. Copiar y modificar el original no afecta la copia. Reinsertar el ID produce un vértice aislado.</p>
<div class="callout">Los invariantes básicos ya existentes se conservan como complemento. El panel analítico y la integración con historial corresponden al Workshop 2.</div>
'''),
('Fuerzas e integración numérica', '''
<h1>Geometría dinámica,<br>tiempo fijo</h1>
<p>Para vértices i y j, Δ=xⱼ−xᵢ y r=|Δ|. Las fuerzas de visualización se evalúan por pares con signos opuestos. No se confunden con interacción cuántica.</p>
<div class="equation">Fˢᵢⱼ = k_global · k_arista · (r−L) · Δ/(r+εₛ)<br>Fʳᵢⱼ = −kᵣ · dirección / (r²+ε²)<br>Fᵈᵢ = −γvᵢ &nbsp;&nbsp;&nbsp; aᵢ = Fᵢ/mᵢ</div>
<p>La longitud natural L es 90 en los presets. La repulsión usa ε=5; el pequeño εₛ del resorte evita división por cero. Si dos nodos coinciden, una dirección determinista basada en IDs proporciona una fuerza de separación. Se evita introducir infinitos para fijar un vértice.</p>
<h2>Velocity Verlet</h2>
<ol><li>Evaluar fuerzas conservativas y calcular aₙ.</li><li>Aplicar medio amortiguamiento: v ← exp(−γΔt/(2m))v.</li><li>Medio impulso: v ← v + (Δt/2)aₙ.</li><li>Avanzar posición: xₙ₊₁ ← xₙ + Δt·v.</li><li>Recalcular fuerzas en xₙ₊₁ y obtener aₙ₊₁.</li><li>Completar impulso y amortiguamiento: v ← exp(−γΔt/(2m))[v + (Δt/2)aₙ₊₁].</li></ol>
<p>El amortiguamiento se separa simétricamente porque depende de velocidad. El caso conservativo conserva aproximadamente energía; el amortiguado la disipa. Un nodo fijo o arrastrado omite integración de posición y mantiene velocidad cero.</p>
<h2>Independencia de renderizado</h2>
<div class="equation">Δt = 1/120 s &nbsp;&nbsp; física: 120 pasos/s</div>
<p>El renderizado aporta tiempo al acumulador. Se ejecutan pasos completos hasta consumirlo. Una pausa borra el acumulador. Una suspensión larga se limita a 0.1 s de recuperación, como máximo 12 pasos, para evitar una espiral de actualización.</p>
<h2>Pruebas numéricas</h2>
<p>Un oscilador de dos nodos mantiene error relativo de energía inferior al 0.1% tras 1.200 pasos sin amortiguamiento. Con amortiguamiento la energía baja; una masa libre cumple v(t)=v(0)exp(−γt/m). El estado tras un segundo coincide a 30, 60 y 144 FPS. Cien nodos permanecen finitos durante 7.200 pasos.</p>
<p>El cálculo de fuerzas cuesta O(V²+E) esperado por paso. El avance usa cálculo directo; no se promete rendimiento para millones de nodos.</p>
'''),
('Interacción y evidencia visual', f'''
<h1>Manipular<br>y observar</h1>
<p>La aplicación SFML dibuja vértices como círculos, aristas como segmentos e IDs junto a los nodos. El HUD incluye pausa, cantidad de vértices/aristas y parámetros de resortes, repulsión y amortiguamiento.</p>
<table><tr><th>Acción</th><th>Control</th></tr><tr><td>Agregar / seleccionar / conectar</td><td>A / S / C, o botones y clics.</td></tr><tr><td>Eliminar seleccionado / por puntero</td><td>Delete / D y clic.</td></tr><tr><td>Pausar / fijar / reiniciar posiciones</td><td>Space / P / R.</td></tr><tr><td>Repulsión / rigidez / amortiguamiento</td><td>Up–Down / [–] / ,–.</td></tr><tr><td>20 nodos / 100 nodos / encuadrar</td><td>1 / 2 / F.</td></tr></table>
<p>El arrastre conserva el punto de agarre: seleccionar un borde del nodo no lo hace saltar al centro del puntero. Reiniciar posiciones conserva topología y fijaciones. Encuadrar modifica sólo la cámara. La ventana se puede cerrar con Esc o el control del sistema.</p>
{'<img class="screenshot" src="'+image+'" alt="Captura de la demostración"/>' if image else '<div class="callout">Captura pendiente de la comprobación visual.</div>'}
<p class="small">{html.escape(caption)}</p>
<h2>Estado de comprobación</h2>
<p>La aplicación SFML compila con GCC y CMake. Este entorno restringido bloquea la conexión X11: aún no se afirma haber observado su ventana. El comando <code>./bin/spin_network --smoke-test</code> está preparado para abrir 100 nodos, renderizar, guardar captura, probar pausa/repulsión y cerrar en un escritorio habilitado.</p>
<p>El controlador ya tiene pruebas automáticas de edición, arrastre, fijación, pausa y eliminación. El guion de <code>docs/validation.md</code> completa la comprobación manual. La demo web se conserva como complemento independiente y sus controles sí se probaron en Brave.</p>
'''),
('Resultados, entrega y siguiente etapa', '''
<h1>Evidencia<br>y cierre de alcance</h1>
<table><tr><th>Comprobación</th><th>Resultado</th></tr><tr><td>Lista simple y memoria propietaria</td><td>Pruebas pasan; nodos con unique_ptr.</td></tr><tr><td>Hash: 5.000 vértices, colisiones, rehash</td><td>Pasa; 1.000 claves forzadas a un bucket.</td></tr><tr><td>Grafo y eliminación incidente</td><td>Pasa; 1.000 nodos / 5.000 aristas.</td></tr><tr><td>Física y 100 nodos estables</td><td>Pasa; 7.200 pasos, 60 s simulados.</td></tr><tr><td>30 / 60 / 144 FPS</td><td>Mismo estado tras un segundo.</td></tr><tr><td>CMake / CTest</td><td>Aplicación compilada; 3/3 suites pasan.</td></tr><tr><td>ASan + UBSan</td><td>Tres suites pasan, detect_leaks=0.</td></tr><tr><td>Ventana SFML / fugas / Windows</td><td>Pendientes de validación externa.</td></tr></table>
<p>El benchmark de física optimizada dio aproximadamente 0.19–0.23 ms por paso para 100 nodos en este equipo. No mide tiempo de renderizado, ni establece una garantía para otros equipos.</p>
<h2>Limitaciones concretas</h2>
<p>LeakSanitizer aborta bajo ptrace en este entorno. ASan y UBSan no reportaron errores con detección de fugas desactivada; no se afirma ausencia de fugas verificada. La conexión X11 devuelve Operation not permitted, por lo que la aceptación visual sigue abierta. Los binarios generados son Linux x86_64; Windows/Code::Blocks requiere compilación y prueba con SFML compatible.</p>
<h2>Cómo reproducir</h2>
<pre>./tools/build.sh
./bin/spin_network
./bin/spin_network --smoke-test
cmake -S . -B build &amp;&amp; cmake --build build
ctest --test-dir build --output-on-failure</pre>
<p>Conservar assets/fonts y usar SFML 2.6.x. El README explica dependencias locales y Code::Blocks. Los fuentes incluyen tests, scripts de compilación, el informe y la demo web.</p>
<h2>Workshop 2</h2>
<p>La entrega final añadirá lista doble y estados con copia profunda, undo/redo, eliminación de la rama futura, límite de 100 snapshots topológicos, invariantes y panel analítico. El refinamiento local u–v → u–w–v preservará χ porque ΔV=ΔE=1. Multithreading es opcional; un movimiento Pachner 3D queda fuera del alcance obligatorio.</p>
<p class="small">Referencias: <a href="https://www.sfml-dev.org/tutorials/2.6/getting-started/linux/">SFML 2.6 — Linux</a>; <a href="https://www.sfml-dev.org/documentation/2.6.2/">API SFML 2.6.2</a>; implementación y evidencias en include/, tests/ y docs/validation.md.</p>
''')
]
style = '''
@page{size:A4;margin:0}*{box-sizing:border-box}body{margin:0;background:#e5e7eb;color:#182532;font:10pt/1.35 Arial,sans-serif}.page{position:relative;width:210mm;height:297mm;padding:14mm 19mm 17mm;background:white;margin:8mm auto;break-after:page;overflow:hidden}.page:last-child{break-after:auto}.header{font-size:8pt;letter-spacing:1.3px;color:#526575;border-bottom:1px solid #dbe3e8;padding-bottom:3mm;margin-bottom:5mm}h1{font-size:28pt;line-height:1.08;letter-spacing:-1px;margin:3mm 0 5mm}h2{font-size:13pt;margin:4mm 0 2mm;color:#25523f}p{margin:2.5mm 0}ol{padding-left:6mm;margin:3mm 0}li{margin:1mm 0}.eyebrow{font-size:8pt;letter-spacing:1px;color:#327255}.subtitle{font-size:15pt;color:#526575}.callout{background:#edf5ea;border-left:3px solid #709c55;padding:4mm;margin:5mm 0}.equation{background:#f1f5f7;padding:4mm;font-size:12pt;line-height:1.7;margin:4mm 0}pre{font:8.7pt/1.4 monospace;white-space:pre-wrap;background:#f1f5f7;padding:4mm;margin:4mm 0}code{font-size:9pt}table{width:100%;border-collapse:collapse;margin:4mm 0;font-size:9.5pt}th,td{text-align:left;padding:2mm;border-bottom:1px solid #dbe3e8}th{background:#edf3f5}.small{font-size:8.5pt;color:#526575}.screenshot{display:block;width:100%;max-height:70mm;object-fit:contain;background:#0b1015;margin-top:5mm}.footer{position:absolute;bottom:10mm;left:19mm;right:19mm;border-top:1px solid #dbe3e8;padding-top:2mm;font-size:8pt;color:#526575;display:flex;justify-content:space-between}a{color:#25523f}@media print{body{background:white}.page{margin:0;box-shadow:none}}'''
sections = []
for index,(title,body) in enumerate(pages,1):
    sections.append(f'<section class="page"><div class="header">SPIN NETWORK LABORATORY · WORKSHOP 1</div>{body}<footer class="footer"><span>{html.escape(title)}</span><span>{index} / 8</span></footer></section>')
report = '<!doctype html><html lang="es"><meta charset="utf-8"><title>Spin Network Laboratory — Informe Workshop 1</title><style>'+style+'</style><body>'+''.join(sections)+'</body></html>'
(ROOT/'docs/report/workshop1.html').write_text(report)
(ROOT/'site/dist/workshop1-report.html').write_text(report)
print('Generated eight-page report:', ROOT/'docs/report/workshop1.html')
