# Decisiones y riesgos

- C++17 compartido, sin dependencias gráficas en core: evita reescribir comportamiento para ESP32.
- GDI de Windows para el primer prototipo: ya hay MSVC y SDK, no se necesita instalar SDL. Coste: el adaptador actual no funciona en Linux/macOS. SDL3 se podrá incorporar después sin cambiar Model/Player.
- Tiempo inyectado y monotónico: animación determinista y testeable sin ventana. No usar número de frames como reloj.
- Placeholder por transformaciones: comprobar ritmo, ancla y controles sin producir cientos de imágenes provisionales.
- Carpetas vacías reservadas con .gitkeep; no crear implementaciones ficticias de fases posteriores.

Riesgos: referencias artísticas ausentes; cantidad de recursos final elevada; RAM y DMA en placa desconocida; guardado interrumpido; fecha perdida; texto pequeño en pantalla física. Mitigación: atlas/overlays, presupuestos medidos, commit recuperable, hora válida explícita y pruebas de legibilidad con niña/adulto al probar el dispositivo. Prototipo muestra fuentes 12–18 píxeles lógicos; queda pendiente validar accesibilidad en panel físico.

Fuente oficial alternativa SDL3: https://wiki.libsdl.org/SDL3/SDL_RendererLogicalPresentation (presentación por múltiplos enteros). No se ha añadido SDL como dependencia.
