# Integraci贸n supervisada de la referencia original

Estado: no existe imagen original en la carpeta inspeccionada ni fue adjuntada en esta misi贸n. No se puede afirmar fidelidad de color, proporci贸n o expresi贸n. Se conserva un placeholder expl铆cito y no se generan otras formas.

1. Guardar la referencia intacta en assets/references/ y registrar su procedencia y permisos. No mezclarla con los PNG provisionales.
2. Revisar si contiene fondo, texto u otros personajes. Un recorte rectangular no elimina esos elementos ni convierte una ilustraci贸n en sprite de producci贸n. Aislar manualmente al Pato Conejo y revisar bordes contra fondo claro y oscuro.
3. Mantener orejas, pico, alas peque帽as, paleta, proporciones y expresi贸n. No inventar detalles. Si la referencia no define detalles de animaci贸n, pedir revisi贸n antes de considerarlos finales.
4. Partir de una silueta aislada con alfa real. Herramienta opcional (Pillow):

```bat
python tools\asset_validator\prepare_reference.py --input assets\references\pato_aislado.png --output assets\characters\pato_conejo\baby\review\idle_0.png --crop 0 0 128 128 --confirm-isolated-character
```

Las coordenadas son un ejemplo; hay que elegir las de la imagen real. La confirmaci贸n indica revisi贸n humana, no se deriva de un algoritmo. La herramienta rechaza im谩genes opacas o vac铆as y recortes fuera de l铆mites; centra la silueta en una celda 64 脳 64, usa m谩ximo 56 脳 56 y ancla inferior y=60, conserva relaci贸n de aspecto y usa nearest-neighbor. Emite candidato, preview por factor 6 y .review.json con production_approved=false. No elimina fondo/texto, separa personajes ni detecta todos los defectos. Reducci贸n nearest-neighbor puede perder detalles y requiere retoque supervisado.

5. Revisar 1:1 y ampliado: expresi贸n, pico legible, color sin halos, orejas no cortadas, l铆nea de apoyo de patas, alas, consistencia entre frames. Aprobar paleta y tama帽o con el usuario.
6. Producir cuadros idle/happy/walk coherentes, con cuerpo/ancla constantes; distinguir patas en walk. No presentar s贸lo el desplazamiento como ciclo terminado.
7. Crear un manifiesto petanim con los nombres idle, happy y walk; idle/walk en bucle y happy de una sola reproducci贸n. Rutas relativas a --assets-root. Mantener el texto provisional hasta que los recursos sean realmente aprobados; hoy el renderer lo muestra siempre para evitar una falsa atribuci贸n.
8. Ejecutar build.cmd, inspeccionar transparencia/espejo, probar transiciones y comparar con la referencia. S贸lo tras esa revisi贸n sustituir expl铆citamente la indicaci贸n provisional.

La herramienta fue probada con un recurso provisional, no con una referencia original inexistente. No se ha instalado ninguna dependencia.

## Nota Misi髇 3

El renderer ya no incluye la leyenda dentro de la escena: se indica en el t韙ulo de ventana. Mantener esa indicaci髇 hasta aprobar recursos originales. Se necesitan tambi閚 los clips sad y sleep; el archivo pet.anim debe incluir los cinco nombres utilizados por Model.
