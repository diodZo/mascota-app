# Animaciones y sprites — contrato Misión 2

## Runtime

Player conserva la interfaz original `Clip {frames, fps, loop}` y añade play/reinicio y `takeCompletion()`. El evento se produce una vez al cruzar el final de un clip no repetitivo, no al llamar play ni en cada tick posterior. FPS válidos 1–120. Las pruebas originales de tiempo se conservan.

Library almacena definiciones sin Windows. AnimationManager cambia de clip por nombre, proporciona el cuadro actual y entrega el evento de finalización. La biblioteca debe vivir más que el gestor y permanecer inmutable durante su uso; Model posee ambos y no permite copias. Renderizador y decodificador no controlan transiciones. Posición y espejo viven en Model; desplazamientos particulares en SpriteFrame. El espejo invierte también dx, conservando dy.

## Formato petanim 1

Texto UTF-8, sin BOM, comentarios de línea con #. Cada celda es 64 × 64; dx/dy entre -64 y 64. El parser rechaza clips vacíos/duplicados, FPS inválidos, instrucciones desconocidas, basura al final y bloques incompletos.

```text
petanim 1
clip idle 4 1
frame "assets/characters/pato_conejo/baby/placeholder/idle.png" 0 0 64 64 0 0
frame "assets/characters/pato_conejo/baby/placeholder/idle.png" 64 0 64 64 0 -1
end
```

`clip nombre fps loop` con loop 0/1; `frame "ruta" x y ancho alto dx dy`. Atlas: variar x/y. Individual: archivo distinto con x=y=0. El decodificador comprueba dimensiones de hoja múltiplos de 64, máximo 4096 por eje, recortes dentro de la imagen, píxeles visibles y transparencia en cada imagen. No garantiza que cada celda tenga contenido ni valida automáticamente identidad/alineación artística; revisar estos aspectos con el procedimiento de preparación.

PNG RGBA; WIC convierte a BGRA premultiplicado para AlphaBlend sin bordes negros. El tamaño de render es 64 × 64 sin escalado del sprite. La presentación final completa usa factor entero 3 con COLORONCOLOR, equivalente a vecino más próximo en este caso. Las imágenes y su espejo se decodifican una vez en caché, no en cada fotograma. El presupuesto del caché de escritorio no es el futuro presupuesto ESP32.

## Clips provisionales entregados

- idle: 16 cuadros a 4 FPS, cuatro segundos por ciclo; respiración vertical de hasta 2 px, cabeza de 1 px, pestañeo sólo en un cuadro al final del ciclo.
- happy: 6 cuadros a 8 FPS, 750 ms, salto de hasta 8 px, sonrisa, pequeñas alas levantadas; evento al terminar vuelve a idle.
- walk: 4 cuadros a 8 FPS, patas con fases alternadas y oscilación vertical de 1 px. Model desplaza 18 px/s, invierte orientación al cambiar de dirección y limita x entre 28 y 148. Dura 2,4 s y pasa por happy antes de idle.

No se presenta el paseo como arte final: sí hay cambios reales de patas, no sólo traslación de un sprite estático. Todas las variantes son del mismo placeholder. El generador reutiliza geometría/paleta y produce atlas; también genera los seis happy individuales para comprobar ambas formas de carga. Son 26 celdas provisionales, no las 420 finales. El original y sus cuadros aprobados siguen pendientes.

Model avanza en pasos de hasta 10 ms para transiciones estables; el adaptador limita a 1 s un intervalo causado por pausas de ventana. No representa todavía tiempo offline ni simulación de necesidades.

## Extensi�n Misi�n 3

Se a�aden sad (3 cuadros/2 FPS) y sleep (2/1 FPS), y expresi�n sutil en idle. El mismo generador/paleta produce variantes reales. Los cinco clips est�n en ambos manifiestos. Las transiciones ahora dependen de necesidades/acciones; walk vuelve a reposo sin happy autom�tico, y Juegos activa happy con efectos de estad�sticas. La leyenda provisional pasa al t�tulo de ventana. V�ase mision-3.md para las reglas vigentes.
