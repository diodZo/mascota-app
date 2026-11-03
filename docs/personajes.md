# Personajes — catálogo Misión 4

Identificadores/nombres/etapas viven en core/evolution/catalog.hpp; deben permanecer estables para las partidas. Texto visible ASCII, por la fuente actual.

| ID | Especie | Forma 1 | Forma 2 | Forma 3 | Forma 4 |
|---|---|---|---|---|---|
| pato_conejo | Pato Conejo | Bebé | Infantil | Juvenil | Adulto mágico |
| gata_gallina | Gata Gallina | Bebé | Infantil | Juvenil | Adulta majestuosa |
| pez_caminante | Pez Caminante | Bebé | Infantil | Explorador | Guardián acuático |
| poni | Poni Adorable | Bebé | Infantil | Juvenil mágico | Adulto alado |
| chatgpt_junior | ChatGPT Junior | Minirobot | Robot curioso | Robot explorador | Mago tecnológico |

Selección/nombre iniciales se pueden completar con izquierda, Enter y derecha. Hay nombres sugeridos por especie y un límite configurable de hasta 16 caracteres. No se implementa Unicode.

Cada especie tiene stage_01..stage_04 y seis clips de 64x64. Sin referencia original, se entregan sólo placeholders explícitos: orejas/pico en el Pato, orejas/cresta en la gata, cuerpo de pez con extremidades, silueta de poni y robot geométrico. Los marcadores/accesorios distinguen etapas, sin presentarlos como evoluciones finales aprobadas. Las otras especies no reutilizan al Pato como su representación.

Prioridad de recursos: pet.anim original de la etapa, placeholder/pet.anim específico y, si tampoco existe, símbolo de especie/etapa. Se registra el recurso pendiente en build/missing-assets.log. Las referencias futuras prevalecerán; faltan paletas y cuadros definitivos de las veinte formas. El contenido provisional se identifica en el título y en la selección.
