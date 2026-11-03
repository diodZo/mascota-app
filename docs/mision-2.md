# Cambios de la Misión 2

Se conserva el núcleo C++17 y GDI. main.cpp queda a cargo de la ventana, controles, reloj y resolución de rutas. renderer.hpp concentra UI de 240 × 240; sprite_store.hpp carga PNG con WIC, valida recursos y almacena caché normal/espejado. pixel_font.hpp contiene glifos 5 × 7 dibujados a píxel entero, sin fuentes instaladas ni suavizado.

La biblioteca portable define clips y regiones independientes de GDI, con carga textual y gestor de reproducción. Player mantiene su contrato original y añade reinicio/evento de final. Model organiza seis acciones, estados visuales y desplazamiento. Happy termina en idle; walk usa alternancia real de patas y termina en happy/idle. La biblioteca se valida al arrancar antes de abrir la ventana; errores de carga se muestran de forma centralizada. Los errores durante el dibujo cierran limpiamente la ventana con código de fallo.

Habitación nueva: barra superior con nombre/etapa, ventana, cielo, planta, papel mural, suelo, sombra y una fila de seis iconos. El corazón es decorativo; no representa una estadística calculada. El menú muestra el nombre de la opción seleccionada y el estado visual de demostración. No se añadieron estadísticas ni acciones reales de cuidado.

Recursos originales pendientes: imagen de referencia del Pato Conejo bebé, paleta aprobada, silueta y frames originales idle/happy/walk. No se declara integrado el personaje original. Todas las imágenes entregadas están bajo placeholder/ y la ventana indica ARTE PROVISIONAL.

Herramientas: generador provisional determinista sin dependencias; preparador supervisado opcional con Pillow. No se utiliza ImageGen ni se rediseña un original ausente.

Cambios de dependencias: sólo bibliotecas incluidas en Windows SDK (windowscodecs, ole32, msimg32 y shell32). No se instaló software. El caché PNG es específico de escritorio; ESP32 sigue requiriendo su propio adaptador de imágenes y memoria, conservando las definiciones y estados portables.

Estructura completa y ejecución: README.md. Las pruebas de navegación cambian intencionadamente su expectativa de tres a seis opciones, según esta misión; no se elimina la cobertura previa de Player. La vieja prueba de espejo ligado a las opciones Mirar izq./der. se sustituye por espejo durante paseo, porque esas opciones ya no existen.
