# Validación Misión 4 — 2026-11-03

## Resultados automáticos

- build.cmd: C++17 MSVC x64, /W4 /WX; compilación sin avisos y cuatro suites C++ PASS.
- CMake/MSBuild Release: configurado y compilado con el CMake ya incluido en Visual Studio, sin instalar dependencias. CTest 4/4, 0 fallos.
- core_tests: cinco necesidades, seis acciones, límites, sueño/recuperación, estados previos, parser de animaciones, paneles, tiempo y persistencia conservados.
- evolution_tests: catálogo de cinco especies y cuatro etapas; APIs originales probadas con sus umbrales antiguos como fixture, nuevos valores normales, edad/XP/cuidado/actividades/felicidad insuficientes, adulto y datos inválidos.
- multispecies_tests: selección de cada especie con tres botones; edición/borrado/límite/finalización/volver del nombre; veinte combinaciones de guardado/restauración; migración v1 exacta; premios significativos y cooldowns al reiniciar; cuidado no manipulable por una pulsación; ausencia sin edad; una evolución por actualización, evento único, bloqueo de acciones, forma anterior/nueva, vuelta a habitación, adulto estable y conservación de estadísticas.
- assets_tests: regresiones de PNG/alfa/espejo/atlas, buffer 240x240 y presentación 3x; migración a disco, copia v1 inmutable, recuperación desde respaldo v1, conservación de corrupto, perfiles que no reemplazan el anterior, veinte formas con píxeles distintos, estados de selección/nombre, recursos originales malformados/ausentes y fallback aun sin ningún PNG; efecto de evolución y cambio real de imagen.
- tools_tests.py: PASS; preparación RGBA64 e informe, rechazo de recurso opaco, suite de recursos. Pillow ya estaba disponible y no se necesita para ejecutar el juego.
- generate_forms.py: ejecutado correctamente; genera 20 formas/120 clips de forma automatizada, con biblioteca estándar y IDs leídos del catálogo central.
- Se comprobó ausencia de Windows/WIC en los includes de core/. Build, saves y __pycache__ siguen ignorados por Git.

## Sesión visual real y límites

Se abrió una partida aislada con config/evolution-demo.cfg bajo build/manual-validation/, sin usar la partida anterior. Se observaron selección con vista previa, navegación entre especies, editor de nombre y la habitación con Poni. Hubo interacción manual concurrente: no se atribuyen todas las acciones a automatización.

La partida llegó a juvenil y luego a adulto. Al alimentar desde juvenil se observó el efecto: forma anterior, destellos y felicitación ADULTO ALADO; el archivo registró etapa 4, especie poni, estadísticas y progreso. La captura docs/images/mision-4-evolucion.png registra ese efecto. Se verificó el guardado real con nombre PONI, XP 50 y estado de sueño.

El usuario detuvo Computer Use con Escape. Se respetó la detención y no se realizaron más acciones en ventanas. **Quedó pendiente la comprobación visual de cerrar/abrir y continuar desde el menú en esa sesión**. Se verificó en su lugar la restauración sin ventana del archivo adulto real, en modo sólo lectura:

```text
RESTORE CHECK: poni stage=4 XP=50; name, stats and sleep preserved (read-only source)
```

Las pruebas automatizadas sí cubren restaurar todas las especies/etapas, nombre, estadísticas, sueño, progreso, selección de perfil y compatibilidad v1. La parte visual pendiente no se declara probada.

## Comandos reproducibles

```bat
build.cmd
build\evolution_tests.exe
build\multispecies_tests.exe
build\assets_tests.exe "D:\Repositorio\Personal\python\mascota-app"
python tests\tools_tests.py
```

CMake:

```bat
cmake -S . -B build/cmake -G "Visual Studio 17 2022" -A x64
cmake --build build/cmake --config Release
ctest --test-dir build/cmake -C Release --output-on-failure
```

Para inspeccionar una partida sin modificarla, agregar su ruta después de la raíz en assets_tests.exe. Los fixtures de escritura y corrupción se crean exclusivamente bajo build/persistence-tests/.

## No comprobado

Firmware, hardware ESP32-S3, identidad visual original, autonomía y balance infantil real. No se publicaron cambios ni se hizo merge. No hay ZIP ni proyecto paralelo.
