# Misión 4 — implementación integrada

Implementada directamente en D:/Repositorio/Personal/python/mascota-app, en la rama existente feature/mision-4-evoluciones. Se revisaron el catálogo/pruebas aportados por el usuario antes de ampliarlos. No se creó proyecto paralelo, ZIP, commit, publicación ni merge. Build y partidas siguen ignorados por Git.

## Implementado

- Catálogo de cinco especies con ID estable y cuatro nombres de forma por especie.
- Selección con vista previa, posición 1/5 y tres botones; editor de nombre con letras, borrar, terminar, volver y límite configurable.
- Partidas nuevas separadas, menú inicial Continuar/Nueva mascota y recuerdo del perfil actual.
- Edad activa, tiempo jugado y ausencia separados; XP significativa con enfriamientos persistidos, actividades y cuidado histórico.
- Requisitos externos por edad, XP, cuidado, actividades y felicidad; una etapa por actualización, adulto estable, evento y pausa de acciones durante efecto.
- Banco de recursos estable y claves especie.etapa.animación. Resolución por clip: original → placeholder específico → símbolo de emergencia por especie/etapa; los faltantes quedan en build/missing-assets.log.
- Veinte formas provisionales visualmente diferenciadas y seis clips por forma: idle, walk, happy, sad, eat y sleep. Se conserva el reproductor y las hojas de 64x64. Las acciones existentes siguen usando happy; eat queda disponible para contenido futuro.
- JSON v2 con migración v1, copia original permanente, respaldo, preservación de principal corrupto y restauración de progreso.

## Archivos

Ampliados: core/evolution/catalog.hpp y tests/evolution_tests.cpp del usuario; Model, Snapshot, Library, renderer, entradas, SpriteStore y FileStore existentes. Se conservan Stats y Player.

Nuevos:

- core/evolution/settings.hpp
- core/ui/name_editor.hpp
- platforms/desktop/form_resources.hpp
- platforms/desktop/profiles.hpp
- config/evolution.cfg y config/evolution-demo.cfg
- tests/multispecies_tests.cpp y tests/fixtures/mision-3-v1.json
- tools/sprite_generator/generate_forms.py
- Nueva partida.cmd y Demo evoluciones.cmd
- assets/characters/{especie}/stage_01..stage_04/placeholder/{pet.anim,idle.png,walk.png,happy.png,sad.png,eat.png,sleep.png}
- docs/auditoria-mision-4.md, sistema-evolucion.md, migracion-partidas.md, validacion-mision-4.md y captura del efecto bajo docs/images/.

Actualizados: README, arquitectura, personajes, plan de desarrollo, build.cmd, CMakeLists y pruebas de núcleo/recursos. docs/mision-4-base.md se conserva como documento histórico de la base aislada original.

## Decisiones

El núcleo sigue sin Windows: usa tiempo/eventos/datos inyectados. La biblioteca del reproductor permanece estable para no invalidar referencias al cambiar de forma; el cambio se hace por clave de clip, no reemplazando el motor. Sólo el adaptador sabe de GDI/WIC, archivos y perfiles. La selección/nombre/evolución son estados de Model que el renderer representa; el dibujo no concede XP ni decide evolución.

La calidad de cuidado es histórica, no una recompensa instantánea. Los cooldowns se miden en juego activo, por lo que adelantar el reloj o reiniciar no concede XP adicional. No hay edad ni castigo offline; el reloj de fecha sólo informa ausencia. La migración no asigna progreso antiguo inventado.

Los dibujos nuevos son placeholders geométricos explícitos, diferenciados por silueta/paleta y marcadores de etapa; no son diseños definitivos ni se afirma fidelidad a referencias ausentes. La ruta stage_N/pet.anim queda para recursos aprobados; no se confunde con placeholder/pet.anim. Si un original no carga se continúa con su sustituto específico.

## Límites y próximos pasos

Faltan referencias originales, sprites aprobados y balance real (las edades normales son días de tiempo activo). No hay evolución ramificada, minijuego nuevo, audio, editor Unicode ni selector visual de perfiles antiguos. El menú inicial permite iniciar perfiles nuevos, y --save abrir anteriores. No existe bloqueo entre instancias sobre el mismo archivo.

ESP32-S3: la lógica es portable, pero no se ha compilado firmware ni verificado placa. El caché de escritorio precarga 20 formas RGBA normales/espejadas (alrededor de 22 MiB sólo para píxeles de 700 celdas), inviable como presupuesto implícito de un MCU. El adaptador deberá cargar sólo la forma activa y cuadros necesarios, convertir a RGB565/paleta/máscara y medir memoria/FPS. Streams, asignaciones STL, excepciones y precisión/coste de double requieren revisión con la configuración real de ESP-IDF; IStorage deberá tener adaptador NVS con escrituras agrupadas. Sin RTC válido no se inferirá progreso desde la fecha.

Siguiente paso recomendado: completar la revisión visual de reinicio cuando el usuario quiera utilizar control del equipo, probar el balance con sesiones reales y sustituir placeholders con referencias aprobadas. Estado de validación real en validacion-mision-4.md.
