# Misión 3 — mascota jugable

## Auditoría y alcance

Se partió de la Misión 2 en D:/Repositorio/Personal/python/mascota-app. Se leyó el núcleo, la ventana, el renderer, el generador, el manifiesto y las pruebas antes de cambiar archivos. No se encontraron AGENTS.md ni referencias originales. Las pruebas base dieron PASS. Se trabajó sobre una copia y se comprueban hashes antes de trasladar cambios, sin crear commits ni eliminar archivos del usuario.

La habitación, paleta, fuente, iconos, pantalla 240 × 240 y ampliación por 3 se conservan. La leyenda ARTE PROVISIONAL se retira de la habitación; la condición se indica en el título de la ventana y esta documentación. No se añade ninguna especie ni evolución.

## Reglas de juego

Las cinco estadísticas residen en Stats, no en el renderer. Configuración real en config/gameplay.cfg, leída al iniciar; rangos y valores iniciales absolutos, cambios expresados como porcentaje del rango. Se validan límites, valores finitos, duplicados y umbrales de sueño. La variable interna hambre significa reserva de alimento (más alto = mejor alimentado); la interfaz usa SACIEDAD para evitar ambigüedad.

| Estadística | Inicial | Rango | Cambio por minuto despierto |
|---|---:|---:|---:|
| Saciedad / hambre | 65 | 0–100 | -3 |
| Felicidad | 70 | 0–100 | -2 |
| Energía | 70 | 0–100 | -2,5 |
| Higiene | 75 | 0–100 | -1,8 |
| Salud | 90 | 0–100 | +1 con saciedad e higiene al menos al 50%; -1 si alguna está por debajo del 25% |

La salud además admite una decadencia base configurable (0 por defecto). Todos los valores se limitan a sus rangos. No existe muerte, pérdida permanente ni coste de recuperación.

| Acción | Efecto y reacción |
|---|---|
| Comida | +25 saciedad, +3 felicidad; despierta, happy con salto/sonrisa, mensaje de agradecimiento |
| Juegos | +20 felicidad; -8 energía, -4 saciedad, -3 higiene; happy. Si energía <=15%, duerme antes de jugar |
| Dormir | Alterna sueño/despertar; sleep con ojos cerrados, respiración y Z. +25 energía/minuto dormido |
| Higiene | +35 higiene, +5 felicidad; despierta y reacciona con happy |
| Salud | Abre resumen del estado y explicación de cuidados; no es un botón de curación instantánea |
| Estadísticas | Abre valores actuales y barras normalizadas; los valores siguen cambiando |

Los deltas son puntos porcentuales del rango, no valores absolutos invariables. A energía <=15% se duerme automáticamente; al llegar al 95% despierta con happy. Puede despertarse desde Dormir; si sigue agotado se vuelve a dormir. Alimentar/limpiar no recargan energía instantáneamente. Dormido siguen cambiando las demás necesidades. Salud se recupera incluso desde el mínimo al mejorar cuidados.

## Estados y mensajes

Prioridad: sleep > happy temporal de acción > sad por necesidades bajas > walk/idle. Por debajo del 25% de alimento, felicidad, higiene o salud aparece sad; hambre, higiene, felicidad y salud generan mensajes distintos. Energía baja genera mensaje de sueño y reduce el paseo; al umbral de agotamiento activa sleep. Cuando se corrige la necesidad, sad vuelve a idle. Happy regresa automáticamente al estado de reposo que corresponda, que puede ser sad si quedan otras necesidades pendientes.

Paseos autónomos tras ocho segundos de idle, sólo con suficiente energía y sin necesidades bajas, dentro de x=28..148 a 18 px/s por 2,4 s, con espejo y patas alternadas. No se simula caminar con un sprite estático.

Los mensajes de acción duran 3,5 s, luego vuelve el mensaje derivado del estado. Texto breve en la franja inferior de la habitación. Cada opción mantiene nombre y descripción; los paneles indican cómo volver. Enter cierra un panel; izquierda/derecha cierran el panel y cambian la opción. El ratón activa el icono seleccionado directamente mediante Model::activate.

## Animaciones provisionales

Idle 16 cuadros/4 FPS, happy 6/8 FPS, walk 4/8 FPS conservados; se añade una expresión breve de idle. Sad 3/2 FPS cambia ojos/cejas/pico y baja la cabeza; sleep 2/1 FPS cambia postura relativa del torso/cabeza y ojos cerrados. Todos usan la misma geometría y paleta del placeholder de la misión anterior. La prueba de recursos compara píxeles sin dx/dy para verificar que cada clip contiene variantes reales, además de transformaciones.

Faltan referencias y cuadros originales aprobados del Pato Conejo bebé. No se afirma fidelidad visual ni arte de producción. El generador sigue siendo determinista y sin dependencias; el preparador opcional supervisado de la Misión 2 se conserva.

## Arquitectura y decisiones

- core/pet/stats.hpp: configuración, rangos, estadísticas y normalización.
- core/pet/model.hpp: acciones, pantallas solicitadas, prioridades visuales, mensajes y paso del tiempo; no importa Windows.
- core/animation: reproductor y clips independientes de plataforma; se agregan dos clips a los datos y fixtures.
- core/persistence/snapshot.hpp: contrato versionado, serialización JSON de esquema cerrado e IStorage portable.
- platforms/desktop/file_store.hpp: escritura temporal, vaciado, reemplazo y respaldo mediante Windows; no introduce reglas de gameplay.
- renderer.hpp: conserva la habitación; dibuja mensajes, barras y paneles a partir del modelo.
- main.cpp: convierte entradas, inyecta tiempo monotónico y timestamp Unix, coordina almacenamiento. Los errores de arranque/cierre se comunican al usuario.

Se evita una librería JSON adicional para este esquema pequeño. El lector sólo admite los siete campos definidos, números finitos, nombre ASCII compatible con la fuente y la etapa bebé (1); rechaza versiones desconocidas, campos repetidos/faltantes y datos fuera de rango. No es un parser JSON general ni un sistema de migraciones.

## Guardado

Ruta por defecto relativa a la raíz del proyecto: saves/pet.json. Puede elegirse otra con --save. JSON versión 1: name, stage, stats en orden [hambre, felicidad, energia, higiene, salud], last_action (-1 sin acción o 0..5 en orden del menú), saved_at (segundos Unix) y sleeping. El campo version completa los siete campos del esquema.

Se guarda un segundo después de la última confirmación, cada 30 s de tiempo simulado de sesión y al cerrar normalmente. Se escribe primero .tmp, se vacía el archivo y se reemplaza con MoveFileExW. El guardado anterior validado se conserva como .bak. Si el principal está corrupto se intenta el respaldo; si ambos fallan se conserva todo y se rechaza el arranque para no borrar progreso inadvertidamente. El aviso recuperado aparece en pantalla. Fallo de guardado automático: mensaje y reintento al siguiente intervalo. Fallo al cerrar: se mantiene la ventana para poder reintentar.

No se aplican cambios offline: restaurar conserva los valores y el estado de sueño. El timestamp está disponible para una futura política de ausencia amable. El reloj de animación es monotónico y se inyecta en el núcleo; no depende de la fecha del sistema. El adaptador limita pausas grandes a un segundo por tick, igual que antes.

## Limitaciones y siguiente paso

Una sola mascota bebé; nombre predeterminado sin editor en UI (el formato admite 1–16 caracteres A-Z, dígitos, espacios y guiones), sin inventario, minijuego dedicado, sonido ni evolución. Sólo Windows en escritorio; firmware/NVS pendientes. Las partidas no deben abrirse simultáneamente en varias instancias con el mismo archivo, ya que no existe bloqueo de sesión. Cambiar los rangos de config puede invalidar una partida anterior; usar otra ruta --save para ensayos.

Siguiente paso: ajustar el ritmo de necesidades y recuperación mediante una sesión real de juego y conseguir la referencia original para sustituir los placeholders sin cambiar reglas ni renderer.
