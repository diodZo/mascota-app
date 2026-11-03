# Objetivo ESP32-S3

Pantalla IPS 240 × 240, tres botones, batería, audio retro, guardado y uso sin conexión. No hay modelo de placa ni controlador de pantalla confirmado. No se asignan GPIO, voltajes, cargador o buzzer hasta revisar esquema y fichas del módulo real.

Presupuesto orientativo: framebuffer RGB565 completo 115200 bytes; doble buffer 230400 bytes. Preferir regiones o buffers por filas si es necesario. RAM/PSRAM y flash dependen de la variante y configuración; no se ha compilado ni probado firmware. Revisar DMA, ancho de bus y rendimiento con el driver real. NVS para progreso pequeño y escrituras agrupadas; recursos de sólo lectura en flash. No asumir fecha conservada tras perder alimentación: RTC con respaldo o ajuste manual, además de bandera de hora válida.

Pendientes de investigar con modelo exacto: placa, driver, pines seguros y arranque, resistencias de botones, buzzer/PWM, batería protegida, carga y regulación, suspensión, consumo y autonomía medidos, memoria real y FPS.

Fuentes oficiales consultadas 2026-11-03:
- https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/external-ram.html
- https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- https://www.espressif.com/sites/default/files/documentation/esp32-s3_hardware_design_guidelines_en.pdf
