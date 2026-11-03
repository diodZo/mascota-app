# Arquitectura

El n煤cleo C++17 recibe eventos Left/Accept/Right y tiempo transcurrido en milisegundos. No importa Windows, Python ni un motor gr谩fico. Model controla selecci贸n y respuesta; Player calcula cuadro de animaci贸n sin depender del reloj del sistema. El adaptador Windows obtiene tiempo monot贸nico y traduce teclado/rat贸n; GDI dibuja a un buffer 240 脳 240 y copia por factor 3. No hay suavizado ni escalado fraccional.

Se ha elegido GDI por ser suficiente para validar la primera misi贸n sin instalar dependencias. SDL3 es alternativa para escritorio multiplataforma (admite presentaci贸n por escala entera), pero no es un requisito del n煤cleo ni se ejecutar铆a en ESP32. Python/Tk fue descartado como motor principal para evitar duplicar l贸gica al portar. El renderer Windows ser谩 reemplazado, no convertido, para ESP32.

Contratos previstos para fase 3: IClock (monot贸nico y fecha opcional con indicador de validez), IStorage (load/commit de datos versionados), IAudio (eventos de sonido), IRenderer (sprite/regiones), IInput (tres eventos). Introducir interfaces s贸lo al necesitar una segunda implementaci贸n. Configuraci贸n y evoluci贸n ser谩n datos validados; evitar dependencia JSON dentro del n煤cleo. Un paso de generaci贸n transformar谩 configuraci贸n a tablas C++ para flash.

M贸dulos reservados: pet, evolution, animation, time, persistence, config, rendering/input/audio en adaptadores. Estado de mascota: awake, eating, playing, sleeping, dirty, unwell; prioridad visual sleep > unwell > sad > acci贸n > idle. No se implementan estos estados en el prototipo.

Guardado futuro: JSON versionado en PC con validaci贸n, temporal, vaciado y reemplazo at贸mico m谩s copia recuperable; NVS en dispositivo mediante IStorage, escritura s贸lo al cambiar progreso y en intervalos. Persistir especie, nombre, etapa, estad铆sticas, edad, inventario, progreso y 煤ltima hora v谩lida. Tiempo monot贸nico para sesi贸n; ausencia limitada configurable (propuesta 8 h), penalizaciones acotadas sin muerte. Fecha inv谩lida o retroceso: no aplicar castigo y solicitar ajuste por interfaz local. Este dise帽o est谩 pendiente de implementaci贸n y pruebas.

Errores de arranque y carga deben comunicarse en un punto del adaptador; no bloquear recuperaci贸n de mascota por datos corruptos. El prototipo valida clips; faltan todav铆a el sistema completo de errores y las pruebas de persistencia.

## Actualizaci髇 Misi髇 2

El dibujo procedural inicial ha sido reemplazado por PNG provisionales cargados por WIC. Model usa Library/AnimationManager portables; main.cpp no dibuja sprites ni UI. renderer.hpp y sprite_store.hpp son adaptadores Windows. Consulta animaciones.md y mision-2.md para el contrato actual. La planificaci髇 de estad韘ticas, tiempo offline, persistencia y firmware anterior sigue siendo futura.

## Actualizaci髇 Misi髇 3

Stats y Config controlan necesidades y tasas; Model controla acciones, mensajes y pantalla solicitada. Snapshot/IStorage son portables; FileStore implementa el JSON en Windows, coordinado desde main.cpp. Hay guardado m韓imo, sin pol韙ica offline. Los paneles dibujan valores del modelo y no alteran reglas. La habitaci髇 se mantiene. Contratos y validaci髇 actuales en mision-3.md y validacion-mision-3.md; los apartados anteriores reflejan el dise駉 de cada misi髇.
