# Relazione

Questo progetto ha l'obiettivo di migliorare il progetto [Maze](https://github.com/Tugamer89/maze) implementando un rendering 3D nativo che sfrutta **OpenGL** e usando [ImGui](https://github.com/ocornut/imgui) per una GUI più semplice per gestire diverse impostazioni.

Si tratta di un gioco in prima persona con l'obiettivo di uscire da un labrinto generato casualmente ad ogni run nel minor tempo possibile con il solo aiuto di una minimappa.

## Tappe

### Stage 1 (v0.1.x)

L'obiettivo di questa prima tappa è di scrivere un template di base per l'ambiente di sviluppo, quindi includendo correttamente [ImGui SFML](https://github.com/SFML/imgui-sfml) e il resto del codice per gestire uno spazio 3-dimensionale mediante **OpenGL**.

Viene visualizzato un semplice pavimento che può essere ruotato con il drag del mouse premendo il tasto sinistro e c'è una semplice GUI per modificare diversi parametri di scena.

## Crediti

È stato utilizzato [Gemini](https://gemini.google.com/) per la correzzione e il miglioramento dei testi sia nel codice che nella documentazione, oltre che nello sviluppo per velocizzare la risoluzione di *Code Smells* e altri *Issues* segnalati da [SonarQube](https://sonarcloud.io/project/overview?id=Tugamer89_Tu-Maze).

### Soluzioni tecniche

* **Algoritmo di generazione del labirinto**: L'algoritmo si basa su una versione randomizzata della visita DFS di un grafo, implementata tramite stack come descritto sulla seguente [pagina Wikipedia](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Iterative_implementation_(with_stack))

### Codice esterno

* **Template di base**: Il template di partenza è quello del Lab7 visto a lezione, a cui è stato fatto un po' refactoring e diversi piccoli miglioramenti.

### Risorse

* **Lorem Ipsum**: "[Font Name]" sourced from TODO.
