# Validación Misión 3 — 2026-11-03

## Construcción y pruebas

Windows x64, Visual Studio 2022, C++17, /W4 /WX. `build.cmd` finaliza con código 0 y sin avisos; no se instala software.

1. Prueba base de Misión 2 antes de cambios: PASS.
2. core_tests.exe: PASS. Conserva comprobaciones de Player, bucle, one-shot, eventos, parser de clips y tiempo determinista. Añade las seis acciones, cambios reales en estadísticas, límites, sueño manual/automático, recuperación, baja energía, tristeza por hambre/higiene/felicidad, recuperación de salud desde 0 sin muerte, happy→reposo, paseo/espejo, paneles, configuración y JSON de ida/vuelta con nombre, timestamp, última acción y sueño. Comprueba rechazo de configuración/datos/versión inválidos y que consultar paneles no modifica estadísticas.
3. assets_tests.exe: PASS. PNG transparentes, equivalencia atlas/fotogramas individuales, espejo, recorte inválido, renderer 240 × 240 y presentación 3×. Se dibujan también ambos paneles. Cada animación tiene cuadros con píxeles diferentes incluso anulando desplazamientos. Guardado real a disco, restauración exacta, reemplazo, respaldo, recuperación del respaldo al corromper el principal y rechazo si ambos archivos están corruptos.
4. tests/tools_tests.py: PASS. Herramienta de preparación, candidato RGBA64 e informe, rechazo de recurso opaco. La suite de recursos incluida también se ejecuta. Pillow ya estaba instalado; no es requisito del juego.

Los archivos deliberadamente corruptos de las pruebas viven en build/persistence-tests/, sin tocar saves/ ni entrar en la entrega empaquetada.

## Verificación en ventana

Aplicación iniciada desde su ejecutable, habitación y tres controles conservados. Se observaron paneles con estadísticas variables, sueño y mensajes. Hubo interacción manual concurrente, por lo que no se atribuyen todas las acciones de la sesión a automatización.

Después de cerrarse la primera sesión se reabrió la aplicación: cargó una partida con energía aproximadamente 11 y sleeping=true; se vio la mascota dormida, ojos cerrados, Z y mensaje DULCES SUENOS. Más tarde el panel mostró energía aproximadamente 40 mientras seguía descansando; no se aplicaron penalizaciones por tiempo fuera del juego.

Prueba controlada: izquierda seleccionó Estadísticas; derecha desde el panel volvió a Comida; Enter alimentó, con salto, sonrisa y mensaje QUE RICO! GRACIAS. El efecto numérico de alimentación está comprobado sin ventana en la suite y se registra en el JSON de la sesión. Las capturas incluyen el panel de valores, sueño restaurado y reacción al comer. Las interrupciones de foco/interacción manual se resolvieron reobservando o deteniendo las entradas, sin actuar en otras aplicaciones.

## Reproducir

```bat
build.cmd
build\core_tests.exe
build\assets_tests.exe "D:\Repositorio\Personal\python\mascota-app"
python tests\tools_tests.py
build\virtual_pet.exe
```

En el juego: Estadísticas para observar valores; Enter vuelve. Comida aumenta Saciedad; Juegos sube Felicidad y baja Energía; Higiene sube su valor; Dormir muestra sleep y aumenta Energía al esperar. Cerrar y abrir recupera esos valores y última acción en saves/pet.json. Salud abre un resumen de cuidados.

No se ha ejecutado CMake ni firmware ESP32. No se valida fidelidad a una referencia original ausente. La persistencia es mínima con respaldo, sin migraciones, reloj offline ni bloqueo entre múltiples instancias.
