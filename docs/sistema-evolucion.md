# Sistema de evolución

## Catálogo y etapas

El catálogo del usuario se amplió conservando los valores de Species y Stage y las funciones speciesFromKey, speciesInfo, stageName, canEvolve y evolve. Los identificadores estables son pato_conejo, gata_gallina, pez_caminante, poni y chatgpt_junior. Los nombres visibles, nombres sugeridos, símbolos de emergencia y cuatro nombres de forma se centralizan en core/evolution/catalog.hpp. Model y el resolver recorren ese catálogo; no contienen reglas de cuidado por especie. El generador lee los identificadores del mismo catálogo; sus dibujos provisionales están aislados en una herramienta, no en reglas del juego.

Stage válido: 1 bebé, 2 infantil, 3 juvenil, 4 adulto. Las denominaciones visibles específicas están en el catálogo. Las reglas son compartidas por las especies, sin condiciones particulares dispersas. Para incorporar una especie nueva se añade al catálogo y se suministran sus recursos; el motor de acciones/evolución no cambia. No hay evoluciones alternativas ramificadas en esta misión.

## Configuración efectiva

Archivo normal: config/evolution.cfg. Override: --evolution-config. Las unidades y límites son explícitos; entradas desconocidas, duplicadas, valores no finitos y requisitos inválidos provocan un error de configuración, no un progreso silenciosamente incorrecto.

`stage destino edad_minutos XP cuidado actividades felicidad`

| Destino | Edad activa | XP | Cuidado histórico | Actividades | Felicidad actual |
|---|---:|---:|---:|---:|---:|
| Infantil (2) | 1440 min | 100 | 30 | 3 | 30 |
| Juvenil (3) | 4320 min | 350 | 45 | 8 | 40 |
| Adulto (4) | 10080 min | 800 | 60 | 16 | 50 |

Edades, XP y actividades de etapas sucesivas deben ser no decrecientes. Edad 0..5256000 minutos, XP/actividades hasta 1000000000; cuidado/felicidad 0..100. Todos los requisitos son inclusivos. La primera etapa es inicial, sin requisito.

| Ajuste | Valor | Unidad / límite |
|---|---:|---|
| cooldown_ms | 60000 | Por acción premiada; 1000..86400000 ms |
| significant_gain | 5 | Mejora mínima en puntos porcentuales; >0..100 |
| care_window_ms | 1800000 | Ventana de respuesta del cuidado; 1000..2592000000 ms |
| initial_care | 60 | Inicial y migrada; 0..100 |
| max_name_length | 16 | 1..16 caracteres de la fuente actual |
| xp_food | 10 | Por mejora significativa; 0..1000 |
| xp_play | 12 | Por mejora significativa; 0..1000 |
| xp_sleep | 8 | Una recompensa por descanso significativo; 0..1000 |
| xp_clean | 10 | Por mejora significativa; 0..1000 |

Las cinco necesidades, sus rangos, tasas y efectos siguen en config/gameplay.cfg, sin cambiar su contrato. Salud/estadísticas no dan XP. Poner XP en 0 desactiva la recompensa y el recuento de actividad de esa acción. Los nombres antiguos no se recortan al bajar el límite de edición; ese límite se aplica a nombres nuevos.

## Edad y tiempo

age_ms registra edad activa; played_ms registra juego activo; offline_ms registra ausencia informativa. Hoy edad y juego crecen juntos: ni selección/nombre/menú inicial ni el efecto de evolución los avanzan. El adaptador usa reloj monotónico y limita una pausa larga a 1 s por tick, como antes. Cada necesidad/progreso se integra en pasos de hasta 10 ms.

Ausencia: sólo se suma si el timestamp anterior es válido y el reloj actual es posterior. Se limita a un año por carga y diez años acumulados. No modifica edad, estadísticas, XP ni cooldowns. Retroceso del reloj o fecha desconocida no produce castigo. Es una política explícita sin progreso offline, no una suposición de RTC disponible en ESP32.

Edad, juego e historial se saturan a diez años; XP/actividades a mil millones. No se admiten números fuera de esos límites en una partida.

## XP, actividades y cuidado

Comida exige mejora real de saciedad; juegos de felicidad; higiene de limpieza. Además debe vencer el enfriamiento independiente de esa acción. Las marcas ready_at se miden en played_ms, no en UTC, y se guardan: cerrar/abrir o adelantar el reloj no las reinicia. Repetir sobre una necesidad llena no da XP. Una acción premiada suma una actividad. Dormir recibe como máximo un premio por episodio al ganar al menos significant_gain energía; se guardan la energía inicial y si ya fue premiado, y también aplica su cooldown.

El cuidado usa una media móvil exponencial: en cada paso, el objetivo es la media de las cinco necesidades normalizadas, y `care += (objetivo-care) * dt/care_window_ms`. Se guarda care y tiempo muestreado. Una pulsación no cambia directamente la nota; mejora necesidades que influyen gradualmente. Las cinco necesidades tienen el mismo peso. No hay muerte ni descenso de etapa; el mínimo de salud sigue siendo recuperable.

## Transición y evento

Se evalúan requisitos una sola vez al terminar cada tick externo, no después de cada subpaso. evolve avanza exactamente una etapa. Una mascota adulta permanece adulta. El evento contiene especie, etapa anterior y nueva; se consume una vez y solicita guardado inmediato. Las estadísticas, nombre, progreso y sueño no se reinician.

Efecto: 3,5 s, sin acciones ni envejecimiento. Durante los primeros 1,5 s se ve la forma anterior; luego la nueva, con destellos, nombre de etapa y felicitación. Regresa automáticamente a la habitación. El estado nuevo se guarda al iniciarse la transición: si el proceso se interrumpe, se restaura la nueva forma sin volver a premiar ni evolucionar dos veces por ese tick. Si ya cumple el siguiente requisito, sólo podrá evolucionar en otra actualización posterior al efecto.

El balance de demostración reduce edades a 0,05/0,10/0,15 minutos, XP a 10/20/30, actividades a 1/2/3 y mínimos de cuidado/felicidad a 0. Conserva cooldown y mejora significativa: alimentar, jugar y limpiar permiten ver tres etapas sucesivas sin modificar el balance normal. Las partidas no guardan el archivo de balance: se elige en cada lanzamiento; nunca se desciende una forma ya alcanzada.
