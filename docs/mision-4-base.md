# Misión 4 — Base de cinco especies y evolución

## Alcance del paquete

Se agregó un catálogo **independiente de Windows** con cinco especies y cuatro etapas. El motor de evolución usa edad en minutos de juego, experiencia y calidad del cuidado (0–100). Los umbrales de ejemplo son **provisorios**, no reglas finales de balance. Cada llamada a `evolve()` puede avanzar a lo sumo una etapa y el adulto no evoluciona más.

## Archivos

- `core/evolution/catalog.hpp`: especies, nombres, etapas y motor de elegibilidad.
- `tests/evolution_tests.cpp`: pruebas de catálogo, transiciones, límites y datos inválidos.

## Verificación independiente

Con compilador C++17 (MSVC, GCC o Clang), desde la raíz del repositorio:

```bash
g++ -std=c++17 -Wall -Wextra -Werror tests/evolution_tests.cpp -o evolution_tests
./evolution_tests
```

Para Windows con el Developer Command Prompt:

```bat
cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 tests\evolution_tests.cpp /Fe:build\evolution_tests.exe
build\evolution_tests.exe
```

## Restricciones deliberadas

**Este módulo es una base aislada**, no la Misión 4 completa: no altera `Model`, el esquema JSON versión 1, el renderer ni las partidas existentes. Por ello todavía no permite seleccionar especies en el juego ni evolucionar una mascota guardada.

### Próxima integración (pendiente)

1. Incorporar `Species` y `Progress` en el modelo manteniendo las seis acciones existentes.
2. Definir cómo se obtiene experiencia y cuidado, y ajustar los umbrales con pruebas reales.
3. Migrar persistencia a JSON v2, aceptando v1 como Pato Conejo bebé sin perder progreso y manteniendo el esquema cerrado.
4. Añadir selección de especie y evolución con navegación de tres botones.
5. Integrar los sprites originales cuando estén disponibles; no presentar placeholders como arte definitivo.
6. Revalidar `core_tests`, `assets_tests`, `tools_tests.py` y pruebas nuevas antes de abrir un PR de integración.
