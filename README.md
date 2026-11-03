# Mascota virtual — Misión 3

Pato Conejo bebé jugable en Windows: cinco estadísticas, cuidados reales, sueño, expresiones, paneles y guardado. Conserva la habitación de Misión 2, pantalla lógica **240 × 240**, ventana **720 × 720**, escala entera y pixel art nítido. Núcleo C++17 portable; GDI/WIC sólo en el adaptador de escritorio.

La referencia original sigue pendiente. Los sprites se identifican como provisionales en el título de la ventana, fuera de la escena. No hay nuevas especies/evoluciones.

## Ejecutar y compilar

Abrir `build\virtual_pet.exe`. Para compilar y ejecutar pruebas (Visual Studio con herramientas C++ y SDK Windows):

```bat
build.cmd
build\virtual_pet.exe
```

No requiere Python ni descargas para jugar. `build.cmd` ejecuta las dos suites C++ y devuelve error si una falla. Pruebas opcionales de herramientas: `python tests\tools_tests.py` (Pillow ya disponible en el entorno de validación).

Alternativa CMake, no ejecutada aquí:

```bat
cmake -S . -B build-cmake
cmake --build build-cmake --config Release
ctest --test-dir build-cmake -C Release --output-on-failure
```

## Jugar

Izquierda/derecha seleccionan una de seis opciones; Enter confirma. El ratón también activa cada icono. En un panel, Enter vuelve; izquierda/derecha vuelven y cambian opción.

- **Comida:** aumenta saciedad, reacción feliz y mensaje.
- **Juegos:** aumenta felicidad y consume algo de energía, alimento e higiene.
- **Dormir:** dormir/despertar; recupera energía progresivamente y despierta al estar descansado.
- **Higiene:** limpia y aumenta felicidad.
- **Salud:** resumen del estado y cuidados que ayudan a recuperarse.
- **Estadísticas:** valores actuales y barras de las cinco necesidades.

La mascota respira, pestañea, pasea con patas alternadas, se alegra, se entristece si necesita atención y duerme al agotarse. Las estadísticas cambian con el tiempo; siempre se puede recuperar, sin muerte permanente. En UI, **Saciedad alta significa estar bien alimentado**; corresponde a la variable interna hambre.

## Configuración y partida

Editar `config/gameplay.cfg` con el juego cerrado para cambiar rangos, valores iniciales, ritmo y efectos. Las tasas son porcentaje del rango por minuto; los efectos de acciones, puntos porcentuales. Ejemplo predeterminado: comida +25; juegos +20 felicidad/-8 energía; limpieza +35 higiene; sueño +25 energía por minuto. Detalles en docs/mision-3.md.

Partida automática: `saves/pet.json`, ignorada por Git. Guarda nombre, etapa bebé, cinco estadísticas, última acción, timestamp y estado de sueño. Guardado diferido un segundo tras acciones, cada 30 s y al cerrar. Respaldo .bak y recuperación de datos válidos. No abrir dos instancias con el mismo archivo. No hay penalizaciones offline en esta misión.

```bat
build\virtual_pet.exe --assets-root "D:\Repositorio\Personal\python\mascota-app" --animation "assets/characters/pato_conejo/baby/placeholder/individual.anim" --save "D:\Repositorio\Personal\python\mascota-app\saves\prueba.json"
```

La raíz de recursos se encuentra desde el ejecutable, sin depender del directorio actual. `--save` permite ensayos separados. Un cambio incompatible de rangos requiere un archivo de prueba nuevo; no se descarta una partida inválida en silencio. El nombre se conserva en JSON; todavía no hay editor de nombre en pantalla.

## Organización

```text
core/pet/             stats.hpp (configuración), model.hpp (reglas/acciones)
core/animation/       reproductor, clips, estados y eventos
core/persistence/     Snapshot JSON versionado e interfaz IStorage
platforms/desktop/    ventana/entradas, renderer, PNG, fuente y FileStore
assets/characters/    placeholder bebé: idle, happy, walk, sad y sleep
config/               gameplay.cfg y resumen de animaciones
tools/                generador de placeholder y preparación supervisada
tests/                lógica, recursos, persistencia y herramientas
docs/                 arquitectura, misiones y validaciones
```

Cambios, decisiones, límites y parámetros: **docs/mision-3.md**. Evidencia de pruebas: **docs/validacion-mision-3.md**. El contrato de sprites y su preparación sigue en docs/animaciones.md y docs/preparacion-recursos.md; la Misión 3 amplía los clips y mueve la leyenda fuera de escena.

Pendientes: referencia/sprites originales aprobados, ajustar el ritmo mediante juego real, sonido, editor de nombre y adaptación de hardware. No se implementan todavía otras formas.
