# Mascota virtual — Misión 4

Juego C++17 para Windows, pantalla lógica **240 × 240** ampliada 3×. Conserva la habitación, las cinco necesidades y los seis cuidados de Misión 3. Añade cinco especies, nombres con tres botones, cuatro etapas, XP, cuidado histórico, evolución visual y partidas compatibles.

**Todos los personajes siguen usando arte provisional.** Hay veinte formas diferenciadas, no sprites originales aprobados.

## Ejecutar

Abrir `build\virtual_pet.exe`. Si hay partida aparece **Continuar / Nueva mascota**, elegible con izquierda/derecha y Enter. Sin partida comienza la selección de especie.

- **Izquierda / derecha:** recorrer especies, caracteres u opciones.
- **Enter:** aceptar. En el editor, se empieza en LISTO para usar el nombre sugerido.
- Para editar: navegar por letras A–Z, números, espacio, guion, BORRAR, LISTO y VOLVER. BORRAR elimina el último carácter; LISTO termina; VOLVER permite elegir otra especie.
- En la habitación se conservan Comida, Juegos, Dormir, Higiene, Salud y Estadísticas; el ratón también funciona.
- **Salud** muestra resumen, XP, actividades, edad activa, cuidado histórico y tiempo fuera. Estadísticas muestra las cinco necesidades.

Para ir directamente a selección, abrir **Nueva partida.cmd**. Cada nueva mascota crea otro archivo; no reemplaza las partidas anteriores. La última mascota elegida se recuerda al volver a ejecutar normalmente.

## Probar evolución en menos de un minuto

Abrir **Demo evoluciones.cmd**. Elegir especie y nombre. Alimentar, jugar y limpiar una vez con necesidades que puedan mejorar. Con el balance de prueba esas tres actividades permiten alcanzar adulto, pasando por cada efecto de evolución. Mientras dura el efecto, las acciones están bloqueadas; vuelve automáticamente a la habitación.

La demo usa `config/evolution-demo.cfg`; el balance normal usa `config/evolution.cfg` y exige 1, 3 y 7 días **de juego activo acumulado**, además de XP, cuidado, actividades y felicidad. Esos valores son provisionales. No hay progreso de edad ni castigos por ausencia.

## Compilar y probar

Visual Studio con C++ y Windows SDK. No se instala software ni se necesita Python para jugar.

```bat
build.cmd
```

Compila con C++17, /W4 /WX y ejecuta **core_tests, evolution_tests, multispecies_tests y assets_tests**. Pruebas opcionales: `python tests\tools_tests.py` (Pillow sólo para la herramienta de preparación). Generar recursos provisionales: `python tools\sprite_generator\generate_forms.py` (biblioteca estándar de Python).

También está validado con CMake:

```bat
cmake -S . -B build/cmake -G "Visual Studio 17 2022" -A x64
cmake --build build/cmake --config Release
ctest --test-dir build/cmake -C Release --output-on-failure
```

## Partidas y migración

Las partidas nuevas se guardan en `saves/pet-2.json`, `pet-3.json`, etc. La anterior `saves/pet.json` se conserva. `saves/active-profile.txt` apunta a la actual; para abrir una anterior se puede indicar `--save`:

```bat
build\virtual_pet.exe --save "saves\pet.json"
```

Un JSON v1 se carga como Pato Conejo bebé conservando nombre, estadísticas, última acción, timestamp y sueño. En el primer guardado se escribe v2, con copia permanente `.v1` del original y respaldo `.bak`. Los archivos corruptos no se eliminan: si se recupera respaldo y se reemplaza un principal inválido, se conserva también como `.invalid`. Si no hay datos válidos, el juego informa el problema sin descartarlos.

Para comprobar migración **sin tocar tu partida**, copiar `tests/fixtures/mision-3-v1.json` a un nombre libre bajo build/ y abrirlo con `--save`; elegir Continuar y cerrar normalmente. Verificar JSON versión 2 y copia `.v1`. Las pruebas de recursos ya comprueban ese flujo a disco automáticamente.

No abrir dos instancias sobre el mismo archivo. El guardado se realiza al crear mascota, al evolucionar, al cerrar, tras confirmaciones y cada 30 s de juego. Las decisiones de XP y cooldown también se conservan.

## Opciones

```bat
build\virtual_pet.exe --new-game --save "build\prueba-nueva.json" --evolution-config "config\evolution-demo.cfg"
```

`--new-game` fuerza selección y crea otro perfil si el destino ya existe. `--save` elige un archivo sin cambiar la selección global; el balance se elige por lanzamiento. `--assets-root` señala la raíz del proyecto. `--animation` conserva la compatibilidad del manifiesto alternativo del Pato Conejo bebé; las demás formas usan su propio directorio.

## Módulos y documentación

```text
core/evolution/       catálogo estable, requisitos y configuración
core/ui/              editor de nombre compatible con tres botones
core/pet/             necesidades, acciones, progreso y estados
core/animation/       clips y reproductor sin Windows
core/persistence/     Snapshot v2, lector v1/v2, contrato de almacenamiento
platforms/desktop/    GDI/WIC, recursos por forma, perfiles, archivos y entradas
assets/characters/   cinco especies × stage_01..stage_04 × seis clips
config/               gameplay.cfg, evolution.cfg y evolution-demo.cfg
tests/                cuatro suites C++, herramientas y fixture v1
docs/                 arquitectura, evolución, migración y evidencia
```

- `docs/mision-4.md`: implementación, archivos y límites.
- `docs/sistema-evolucion.md`: todos los valores y reglas de progreso.
- `docs/migracion-partidas.md`: esquema, conservación y pruebas.
- `docs/validacion-mision-4.md`: resultados reales y parte manual pendiente.

Pendientes: referencias originales, balance mediante juego real, selector de partidas anteriores, audio y firmware ESP32-S3. El núcleo no incluye Windows, pero el caché gráfico de escritorio no se puede trasladar sin adaptar su presupuesto de memoria.
