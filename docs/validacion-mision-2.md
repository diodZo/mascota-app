# Validación Misión 2 — 2026-11-03

## Resultados

- Antes de cambios: core_tests.exe de la primera misión dio PASS.
- `build.cmd`: MSVC 2022, x64, C++17, /W4 /WX. Compilan núcleo, simulador y pruebas de recursos; sin avisos. Las dos suites C++ dan PASS.
- Núcleo: se conserva la cobertura original del reproductor; tiempos de cuadro, bucle, reproducción única, clips inválidos; se añaden eventos de fin emitidos una sola vez, reinicio/cambio de estado, parser, errores de formato, navegación de seis opciones, happy→idle, walk→happy→idle, orientación, movimiento y tiempo determinista con ticks grandes/pequeños.
- Recursos reales: carga de ambos manifiestos y todos sus PNG, equivalencia píxel a píxel entre happy en atlas e individuales tanto normal como espejado, alfa que conserva el fondo, recorte fuera de hoja rechazado y render 240 × 240 presentado a 720 × 720 fuera de pantalla. También se ejecuta assets_tests.exe desde un directorio distinto a la raíz, con ruta de recursos explícita.
- Herramientas: generación de los recursos provisionales; preparación de un candidato RGBA 64 × 64, informe de revisión no aprobado y rechazo de un recurso opaco. Pillow ya estaba disponible; no se instaló.
- UI: aplicación iniciada; inspección visual de habitación, mascota, contornos nítidos y fuente bitmap. Derecha cambió Comida→Juegos; Enter mostró paseo; izquierda regresó la selección; clic en Salud seleccionó y confirmó, mostrando salto/sonrisa. Se observaron movimiento lateral, patas distintas, idle y retorno tras animación. Se corrigieron líneas del suelo fuera del marco y contraste del icono de juegos seleccionado; se recompiló y revisó la versión corregida.

## Comandos reproducibles

```bat
build.cmd
build\core_tests.exe
build\assets_tests.exe "D:\Repositorio\Personal\python\mascota-app"
python tools\sprite_generator\generate_placeholder.py
python tests\tools_tests.py
build\virtual_pet.exe
```

## Límites y pendientes

No hay referencia original, por lo tanto no se ha validado fidelidad a ese personaje. No se han ejecutado CMake ni firmware ESP32. Las herramientas no garantizan calidad artística; son candidatos para revisión. La UI es una demostración visual: acciones de cuidado y cifras de estadísticas todavía no existen. No se crean otras evoluciones.
