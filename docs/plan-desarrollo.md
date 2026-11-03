# Plan por fases

1. Investigación y arquitectura: análisis, contratos, restricciones, documentación y riesgos. Entrega presente.
2. Mínimo gráfico: Windows 240 × 240, tres controles y ratón, placeholder bebé, idle y pruebas del núcleo. Validar ejecución gráfica antes de declarar completado. Pendiente después: presentación, selección y nombre accesibles con tres botones.
3. Motor: estadísticas centralizadas, acciones, reloj inyectable, menús, audio y almacenamiento versionado. Probar límites de hambre/energía, intervalos, recuperación, reinicio, corrupción y caída durante escritura. Sin muerte permanente.
4. Evolución del Pato Conejo: cuatro etapas; reglas externas por edad, cuidado, felicidad, XP y actividades. Probar umbrales y persistencia. Sin inventar arte final.
5. Incorporar otras cuatro especies sólo después de validar el motor y conseguir referencias.
6. Hardware: identificar placa/pantalla, implementar adaptadores ESP-IDF y medir memoria, FPS y energía. No comprar ni asignar pines sin esquema exacto.

Puerta de salida inicial: build correcto, pruebas sin ventana correctas, ventana con mascota animada y comprobación de izquierda/Enter/derecha. Las fases 3–6 no forman parte de la primera misión.

## Avance Misión 3

Completada una parte funcional de la fase 3: necesidades configurables, seis acciones/paneles, reloj de sesión, guardado mínimo y cinco animaciones provisionales. No están implementados audio, inventario, evolución, otras especies ni política offline. La siguiente iteración propuesta es ajustar el ritmo con pruebas de juego y sustituir arte al recibir referencias.

## Avance Misión 4

Integrados catálogo, selección, nombre, XP, cuidado histórico, edad activa, reglas externas, cuatro etapas, efecto y migración v1 a v2. Veinte formas provisionales y fallback seguro. Cuatro suites C++ y herramientas pasan; se observó evolución hasta adulto con balance de prueba. La restauración del adulto real se comprobó sin ventana; la comprobación visual de reinicio quedó pendiente al detener el usuario Computer Use.

Próximos pasos: esa revisión visual cuando se autorice usar el equipo, balance con sesiones reales, referencias y sprites originales, selector de perfiles anteriores y estudio de memoria/almacenamiento del adaptador ESP32. No se presenta firmware como probado ni se hacen publicaciones o merges.
