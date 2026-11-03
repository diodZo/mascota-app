# Validación de primera misión

Fecha: 2026-11-03. Entorno: Windows x64, MSVC 2022, C++17.

## Comandos

- Inspección con Get-ChildItem y Get-Content; comprobación de herramientas con Get-Command.
- `build.cmd`: compilación del test con /W4 /WX, ejecución y compilación del simulador con /W4 /WX. Resultado: código 0, sin avisos.
- `build/core_tests.exe`: PASS para tiempo, límites de cuadro, bucle, reproducción única, clip inválido, izquierda/derecha/aceptar, orientación y actualización del modelo.
- Lanzamiento de build/virtual_pet.exe y capturas con la habilidad computer-use. Ventana visible con textos completos, mascota provisional, escala entera, menú y saludo. Dos observaciones mostraron ojos abiertos/cerrados y cambio vertical, confirmando idle. También se observaron cambios de selección y confirmación; hubo interacción manual durante la revisión, por lo que no se atribuyen todos esos cambios a automatización.

## Límites

El modelo de los tres botones está probado automáticamente; la sesión gráfica fue inspeccionada con interacción manual concurrente. No se han probado firmware, CMake, otras plataformas, persistencia, audio, estadísticas o sprites originales: quedan fuera del alcance. El ratón comparte el mismo método press que el teclado; su comportamiento visual no se ha aislado en una prueba automatizada.

Próximo paso recomendado: revisar con el usuario el aspecto y los controles del prototipo antes de comenzar estadísticas y guardado (fase 3).
