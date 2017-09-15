# beach-voley

El codigo presente tiene como finalidad cumplir con los requisitos impuestos 
por la materia **75.59 | Tecnicas de programacion concurrente I** dicatada en
la **Facultad de Ingeniria de la Universidad de Buenos Aires**.

## Objetivo
El objetivo de este proyecto consiste en desarrollar una aplicacion que 
simular a un torneo de beach voley. 

## Reglamento
Las personas llegan, de a una, a una playa donde se desarrolla un torneo de 
beach voley. El torneo se realiza en una modalidad algo extraña:
 1. Los participantes compiten en parejas en partidos de 2 contra 2.
 2. Los partidos se juegan en sets, siendo posible jugar de 3 a 5 sets (en 
 voley no existen los partidos en empate). Cada participante obtiene:
 
    - Partido ganado 3 sets a 0 o 3 sets a 1: 3 puntos para cada participante
     del equipo ganador, 0 para los perdedores.
    - Partido ganado 3 sets a 2: 2 puntos para cada participante del equipo 
    ganador, 1 punto para cada participante del equipo perdedor. 
    
 3. Cada participante puede competir en hasta k partidos, con la condicion de 
 que no puede jugar dos partidos con el mismo compañero.

Los partidos se desarrollan en un predio dentro de la playa con capacidad 
maxima de M personas. Todas las personas en el predio compiten en el torneo, 
una vez que completan los k partidos se van, con excepci ́on del caso en el 
cual no puedan encontrar pareja, en cuyo caso abandonan el predio por su 
cuenta.

Las personas pueden entrar y salir del predio en cualquier momento. Las 
personas al formar una pareja se anotan y esperan que se libere una cancha para
poder jugar. Las parejas se comienzan a formar solo luego de que arranque el 
torneo. El torneo arranca cuando haya 10 o m ́as personas en el predio. Cada 
partido dura un tiempo aleatorio.

El predio contiene F · C canchas, ubicadas en F filas y C columnas. Cada 
partido se desarrolla en una cancha al azar, suponiendolas indistinguibles.

En la playa la marea puede subir o bajar. Hay un sistema de alarmas en el 
control que se encarga de monitorear el estado de la marea. Al subir hasta 
cierto punto, suena una alarma y se inhabilitan las F canchas correspondientes 
a una columna. Al bajar a cierto punto, suena otra alarma y se habilitan
nuevamente F canchas. Se estima que la marea no puede cubrir m ́as de C − 1 
columnas, y que sube y baja habilitando y deshabilitando solo una columna de 
canchas a la vez.

Si se inhabilitan las canchas, los partidos que all ́ı se desarrollan se 
cancelan y es como si no se hubiesen jugado.

Hay dos personas encargadas de organizar el torneo, una que lleva un detalle de
todos los partidos jugados en el d ́ıa (parejas y resultado) y otra que se 
encarga de mantener la tabla de resultados y publicarlos periodicamente en el 
sitio web de la playa.

## Requerimientos Funcionales
Los requerimientos funcionales son los siguientes:
 1. Debe poder configurarse sin necesidad de recompilar el codigo:
 
    - Las dimensiones del predio: las F filas y C columnas.
    - La capacidad m ́axima de personas (M).
    - La cantidad m ́axima de partidos por participante k.
    
 2. Al finalizar el torneo, se debe comunicar quien es el campeon. Sera la
  persona que obtenga mayor cantidad de puntos (ser ́an varias personas, en 
  caso de empate en mayor cantidad de puntos).


## Requerimientos No Funcionales
Los siguientes son los requerimientos no funcionales de la aplicacion:
 1. El proyecto debera ser desarrollado en lenguaje C o C++, siendo este 
 ultimo el lenguaje de preferencia.
 2. La simulacion puede no tener interfaz grafica y ejecutarse en una o varias 
 consolas de linea de comandos.
 3. El proyecto debera funcionar en ambiente Unix / Linux.
 4. La aplicacion debera funcionar en una unica computadora.
 5. El programa debera poder ejecutarse en “modo debug”, lo cual dejara 
 registro de la actividad que realiza en un unico archivo de texto para su 
 revision posterior.
 6. Las facilidades de IPC que se podran utilizar para la realizacion de este 
 proyecto son las que abarcan la primera parte de la materia, es decir, hasta 
 el primer parcial. Dichas facilidades son:
 
    - Memoria compartida
    - Señales
    - Pipes y fifos
    - Locks
    - Semaforos
    
    Cualquier otra facilidad queda expresamente excluida para este proyecto.
    
## Tareas a realizar
A continuacioon se listan las tareas a realizar para completar el desarrollo 
del proyecto:
 1. Dividir el proyecto en procesos. El objetivo es lograr que la simulacion 
 este conformada por un conjunto de procesos que sean lo mas sencillos posible.
 2. Una vez obtenida la division en procesos, establecer un esquema de 
 comunicacion entre ellos teniendo en cuenta los requerimientos de la 
 aplicacion. ¿Que procesos se comunican entre sı? ¿Que datos necesitan 
 compartir para poder trabajar?
 3. Tratar de mapear la comunicacion entre los procesos a los problemas 
 conocidos de concurrencia.
 4. Determinar los mecanismos de concurrencia a utilizar para cada una de las 
 comunicaciones entre procesos que fueron detectadas en el item 2. No se 
 requiere la utilizacion de algun mecanismo especifico, la eleccion en cada 
 caso queda a cargo del grupo y debe estar debidamente justificada.
 5. Realizar la codificacion de la aplicacion. El codigo fuente debe estar 
 documentado.
 
## Entrega
La entrega del proyecto comprende lo siguiente:
 1. Informe, se debera presentar impreso en una carpeta o folio y en forma 
 digital (PDF) a traves del campus
 2. El codigo fuente de la aplicacion, que se entregara unicamente mediante 
 el campus
 
 La entrega en el campus estara habilitada hasta las 19 hs de la fecha indicada
 oportunamente.
 
### Informe
El informe a entregar debe contener los siguientes items:
 1. Detalle de resoluci ́on de la lista de tareas anterior.
 2. Diagrama que refleje los procesos, el flujo de comunicacion entre ellos y 
 los datos que intercambian.
 3. Diagramas de clases realizados.
 4. Diagrama de transici ́on de estados de un jugador.