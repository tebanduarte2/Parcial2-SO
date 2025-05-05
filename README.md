# preguntas de analisis y comparación de rendimiento entre la asignacion por medio de Buddy System y New-Delete.
## Ejecución de Imagen de 200x200 con Buddy System y con New/Delete.

![image](https://github.com/user-attachments/assets/6886ada5-b02c-4e66-99f8-cd03de79020b)
![image](https://github.com/user-attachments/assets/f5acc8b7-25ff-4639-bf5a-61d97701df2e)

## ¿Qué diferencia observaste en el tiempo de procesamiento entre los dos modos de asignación de memoria?.

Como se puede observar en las imágenes de ejemplo, el tiempo de procesamiento de la ejecución con Buddy System es menor al que se logra usando `new-delete`, por lo que se evidencia que es más eficiente.

## ¿Cuál fue el impacto del tamaño de la imagen en el consumo de memoria y el rendimiento?.

Tiene mucho impacto, ya que las operaciones son proporcionales al tamaño de la imagen. Como estamos usando 2 `for` anidados para procesarla, la complejidad sería O(alto * ancho) en píxeles, lo cual es mucho. Así que si cambia mucho el tamaño de una imagen, se notará en el tiempo de procesamiento y en la cantidad de memoria utilizada, como en este ejemplo,
que ejecutamos el programa con una imagen de más de 1000x1000 píxeles de tamaño y se evidencia que los tiempos son mucho mayores a la ejecución de la imagen de 200x200.
 
![image](https://github.com/user-attachments/assets/1d5ea141-9a14-4b98-8c6e-276c905880ab)
![image](https://github.com/user-attachments/assets/9d988211-8552-4324-94aa-35bfa55e9086)


## ¿Por qué el Buddy System es más eficiente o menos eficiente que el uso de new/delete en este caso?.

En este caso, el Buddy System es más eficiente ya que tenemos una asignación de memoria controlada, y eso nos evita muchas llamadas al sistema que hace `new-delete` porque tiene que buscar un espacio libre y reservarlo. En cambio, con Buddy System ya sabemos el espacio reservado que tenemos y se hace referencia a la dirección del siguiente espacio que se usará. Esto agiliza el tiempo de procesamiento.

## ¿Cómo afectaría el aumento de canales (por ejemplo, de RGB a RGBA) en el rendimiento y consumo de memoria?.

Podría aumentar un poco la carga, pero no tanto como lo hace aumentar el tamaño de la imagen, ya que los canales solo se gestionan en un `for` anidado a los `for` que recorren la matriz, que en este caso solo va hasta 3 por RGB, pero con RGBA subiría hasta 4.

## ¿Qué ventajas y desventajas tiene el Buddy System frente a otras técnicas de gestión de memoria en proyectos de procesamiento de imágenes?
El uso de Buddy System nos permite un buen control de la segmentacion de la memoria en la asignacion, lo cual es una muy buena ventaja en procesamiento de imagenes ya que aqui se hacen muchas asignaciones de memoria debido a la naturaleza de las matrices que son el objeto que generalmete usamos para representar las imagenes.
