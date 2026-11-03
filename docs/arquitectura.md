# Arquitectura

El núcleo C++17 recibe eventos Left/Accept/Right y tiempo transcurrido en milisegundos. No importa Windows, Python ni un motor gráfico. Model controla selección y respuesta; Player calcula cuadro de animación sin depender del reloj del sistema. El adaptador Windows obtiene tiempo monotónico y traduce teclado/ratón; GDI dibuja a un buffer 240 × 240 y copia por factor 3. No hay suavizado ni escalado fraccional.

Se ha elegido GDI por ser suficiente para validar la primera misión sin instalar dependencias. SDL3 es alternativa para escritorio multiplataforma (admite presentación por escala entera), pero no es un requisito del núcleo ni se ejecutaría en ESP32. Python/Tk fue descartado como motor principal para evitar duplicar lógica al portar. El renderer Windows será reemplazado, no convertido, para ESP32.

Contratos previstos para fase 3: IClock (monotónico y fecha opcional con indicador de validez), IStorage (load/commit de datos versionados), IAudio (eventos de sonido), IRenderer (sprite/regiones), IInput (tres eventos). Introducir interfaces sólo al necesitar una segunda implementación. Configuración y evolución serán datos validados; evitar dependencia JSON dentro del núcleo. Un paso de generación transformará configuración a tablas C++ para flash.

Módulos reservados: pet, evolution, animation, time, persistence, config, rendering/input/audio en adaptadores. Estado de mascota: awake, eating, playing, sleeping, dirty, unwell; prioridad visual sleep > unwell > sad > acción > idle. No se implementan estos estados en el prototipo.

Guardado futuro: JSON versionado en PC con validación, temporal, vaciado y reemplazo atómico más copia recuperable; NVS en dispositivo mediante IStorage, escritura sólo al cambiar progreso y en intervalos. Persistir especie, nombre, etapa, estadísticas, edad, inventario, progreso y última hora válida. Tiempo monotónico para sesión; ausencia limitada configurable (propuesta 8 h), penalizaciones acotadas sin muerte. Fecha inválida o retroceso: no aplicar castigo y solicitar ajuste por interfaz local. Este diseño está pendiente de implementación y pruebas.

Errores de arranque y carga deben comunicarse en un punto del adaptador; no bloquear recuperación de mascota por datos corruptos. El prototipo valida clips; faltan todavía el sistema completo de errores y las pruebas de persistencia.

## Actualización Misión 2

El dibujo procedural inicial ha sido reemplazado por PNG provisionales cargados por WIC. Model usa Library/AnimationManager portables; main.cpp no dibuja sprites ni UI. renderer.hpp y sprite_store.hpp son adaptadores Windows. Consulta animaciones.md y mision-2.md para el contrato actual. La planificación de estadísticas, tiempo offline, persistencia y firmware anterior sigue siendo futura.

## Actualización Misión 3

Stats y Config controlan necesidades y tasas; Model controla acciones, mensajes y pantalla solicitada. Snapshot/IStorage son portables; FileStore implementa el JSON en Windows, coordinado desde main.cpp. Hay guardado mínimo, sin política offline. Los paneles dibujan valores del modelo y no alteran reglas. La habitación se mantiene. Contratos y validación actuales en mision-3.md y validacion-mision-3.md; los apartados anteriores reflejan el diseño de cada misión.

## Misión 4 integrada

Catálogo/Settings en core/evolution; NameEditor en core/ui. Model conserva estadísticas/acciones y agrega edad activa, XP, cuidado histórico, setup y efecto. Un evento de evolución solicita persistir inmediatamente; el renderer no modifica reglas. El banco de clips usa claves especie.etapa.animación y vive más que AnimationManager, evitando referencias invalidadas al evolucionar.

Snapshot v2 conserva progreso y lee v1; FileStore protege original v1/respaldos/principales corruptos. Profiles separa partidas nuevas del perfil anterior y recuerda la actual. Main coordina entradas, reloj, recursos y almacenamiento; core no incluye Windows. La ausencia sólo se registra, no avanza edad ni penaliza.

FormResources resuelve por clip y valida antes de usar; puede recurrir a formas provisionales o símbolos sin cerrar el juego. El caché RGBA de escritorio precarga todo: en ESP32 se requiere otro adaptador con carga de forma activa y presupuesto medido. Contratos y límites actuales: sistema-evolucion.md, migracion-partidas.md y mision-4.md.
