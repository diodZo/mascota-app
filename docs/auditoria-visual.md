# Auditoría visual inicial — Misión 2

Inspección realizada antes de modificar la implementación (2026-11-03).

- Motor gráfico: Win32/GDI, sin motor externo ni Python durante ejecución.
- Resolución real: CreateCompatibleBitmap de 240 × 240; StretchBlt de 720 × 720 con COLORONCOLOR. Ventana fija y proceso consciente de DPI.
- Sprites: no existen imágenes cargadas. main.cpp dibuja rectángulos en coordenadas relativas de 64 × 64; espejo manual, desplazamiento de respiración y ojos superpuestos. Idle de cuatro cuadros a 4 FPS definido en Model.
- Entradas: WM_KEYDOWN traduce izquierda, derecha y Enter a pet::Button; filtra repetición. WM_LBUTTONDOWN convierte coordenadas por factor 3 y confirma opciones. Menú inicial de tres elementos.
- Recursos: assets contiene sólo marcadores .gitkeep. config/animations.json describe recursos futuros pero no se carga. No se encontró referencia original del Pato Conejo en el proyecto.
- Núcleo: Player independiente de Windows con tiempo inyectado; admite FPS, bucle y reproducción única, pero sin reinicio, cambios de clip o eventos. Model contiene selección, orientación y saludo.
- Pruebas: tests/core_tests.cpp verifica tiempos, bucle, reproducción única, clip inválido, tres botones, espejo y actualización. Ejecutable base: PASS antes de modificar. No hay pruebas de carga gráfica.
- Estado Git: archivos de la entrega anterior sin seguimiento; no se crea commit ni se sobrescriben proyectos hermanos.

## Plan de cambio

Mantener C++17 y GDI. Extender Player de forma compatible; agregar gestor de clips y metadatos portable. Separar dibujo del adaptador de eventos. Cargar PNG RGBA mediante WIC, con rutas resueltas desde el ejecutable y opción de raíz configurable. Producir recursos provisionales coherentes mediante herramienta determinista, nunca presentar esos recursos como arte original. Actualizar pruebas de selección al menú requerido de seis opciones, conservando comprobaciones del reproductor existente.

La integración exacta del personaje original queda bloqueada sólo por la referencia faltante; el cargador y su procedimiento de preparación se pueden completar sin ella.
