# Partidas v1 → v2

## Garantías

Se leen ambas versiones. Un v1 válido siempre corresponde a Pato Conejo bebé, porque su validador original sólo admitía etapa 1. Se conservan exactamente nombre, etapa, cinco estadísticas, última acción, fecha y sueño. No se infieren edad ni XP a partir de una fecha antigua. Los campos nuevos empiezan seguros: especie pato_conejo, tiempos/XP/actividades/cooldowns 0, cuidado inicial configurable, energía inicial de sueño tomada de la estadística conservada y recompensa de sueño no cobrada.

La migración se serializa como versión 2 en el primer guardado efectivo. Antes de reemplazar se copia el v1 original sin cambios a `.v1`, una sola vez y sin sobrescribirlo. También se conserva el respaldo rotativo `.bak`. Si el v1 vino de un respaldo tras corrupción del principal, se conserva igualmente como `.v1`. Los bytes del principal inválido se copian a `.invalid`, o un sufijo libre si ya existía. No se borran archivos incompatibles o corruptos.

Si tanto principal como respaldo son inválidos, la carga falla con un mensaje y conserva todos los archivos. El usuario puede crear otra partida con --new-game sin eliminar los anteriores. No se implementan migraciones desde versiones futuras.

## Esquema JSON v2 (18 campos)

Se mantiene el esquema cerrado con rechazo de campos desconocidos/duplicados/faltantes, números no finitos, versión desconocida, especie/etapa inválida y estadísticas fuera de sus rangos.

| Campo | Contenido |
|---|---|
| version | 2 |
| name | 1..16 caracteres A–Z, 0–9, espacio o guion; no sólo espacios en la interfaz |
| stage | 1..4 |
| stats | [hambre/saciedad, felicidad, energía, higiene, salud] |
| last_action | -1 o índice 0..5 del menú existente |
| saved_at | Segundos Unix; fecha informativa, no reloj de simulación |
| sleeping | Estado de sueño |
| species | Identificador estable del catálogo |
| age_ms | Edad activa acumulada |
| played_ms | Tiempo de juego acumulado |
| offline_ms | Ausencia informativa, sin penalización |
| experience | XP acumulada |
| care | Nota histórica 0..100 |
| care_sample_ms | Tiempo muestreado; no superior a played_ms |
| activities | Actividades significativas premiadas |
| reward_ready_ms | Seis marcas de enfriamiento, en tiempo de juego |
| sleep_start_energy | Energía normalizada al empezar el descanso |
| sleep_rewarded | Evita volver a premiar el mismo descanso tras reiniciar |

La serialización siempre produce v2; el lector conserva sourceVersion internamente para saber si debe inicializar el cuidado de una migración y preservar el original. Los timestamps/counters se validan antes de convertirlos. El formato no pretende ser un parser JSON de propósito general; los nombres no admiten Unicode ni secuencias que no pueda dibujar la fuente.

## Escritura y perfiles

Archivo temporal → vaciado con FlushFileBuffers → respaldo de datos anteriores válidos → reemplazo con MoveFileExW y WRITE_THROUGH. Al crear una mascota se guarda inmediatamente; al evolucionar se solicita guardado en ese mismo ciclo. Se mantiene el guardado diferido de acciones, cada 30 s activos y al cerrar normalmente. Una salida forzada puede perder los últimos cambios aún no guardados; no se promete protección ante toda falla física del disco.

El perfil nuevo obtiene un nombre libre pet-2.json, pet-3.json, etc.; la partida previa nunca se reutiliza para la nueva mascota. `active-profile.txt` recuerda el último perfil sólo después de guardar correctamente. Su valor debe ser un nombre de archivo local válido; un valor inválido vuelve a pet.json. `--save` permite abrir un archivo concreto sin cambiar ese selector global. Cerrar selección/nombre antes de LISTO no crea ni reemplaza partida.

No hay bloqueo para dos procesos que abran la misma partida: usar una instancia por archivo. Falta un selector visual de perfiles antiguos; se puede reabrir cualquiera con --save.

## Prueba reproducible sin tocar saves/

Desde una consola de Windows en la raíz, elegir un nombre libre:

```bat
copy tests\fixtures\mision-3-v1.json build\migracion-manual.json
build\virtual_pet.exe --save build\migracion-manual.json
```

Elegir Continuar, consultar estadísticas y cerrar normalmente. El archivo pasa a version 2 con especie pato_conejo, nombre NUBE y los mismos datos existentes (salvo el tiempo de sesión transcurrido); `build/migracion-manual.json.v1` coincide byte a byte con el fixture original. No usar `copy` sobre un destino que ya tenga una partida.

Validación automática sin ventana:

```bat
build\multispecies_tests.exe
build\assets_tests.exe "D:\Repositorio\Personal\python\mascota-app"
```

La primera comprueba migración exacta en memoria y las 20 combinaciones especie/etapa. La segunda realiza migración a disco, respaldo, corrupción y conservación de archivos en directorios únicos bajo build/persistence-tests/. Nunca utiliza saves/ como fixture. Se puede agregar una ruta de partida como tercer argumento para comprobar su restauración en modo sólo lectura, además de las pruebas aisladas.
