# Relazione

Questo progetto ha l'obiettivo di migliorare il progetto [Maze](https://github.com/Tugamer89/maze) implementando un rendering 3D nativo che sfrutta **OpenGL** e usando [ImGui](https://github.com/ocornut/imgui) per una GUI più semplice per gestire diverse impostazioni.

Si tratta di un gioco in prima persona con l'obiettivo di uscire da un labrinto generato casualmente ad ogni run nel minor tempo possibile con il solo aiuto di una minimappa.

## Tappe

### Stage 1 (v0.2.x)

L'obiettivo di questa prima tappa è di scrivere un template di base per l'ambiente di sviluppo, quindi includendo correttamente [ImGui SFML](https://github.com/SFML/imgui-sfml) e il resto del codice per gestire uno spazio 3-dimensionale mediante **OpenGL**.

![Vista di default, stage 1](resources/screenshots/stage1.png)

Viene visualizzato un semplice pavimento che può essere ruotato con il drag del mouse premendo il tasto sinistro e c'è una semplice GUI per modificare diversi parametri di scena.

### Stage 2 (v0.3.x)

L'obiettivo principale è generare un labirinto, per ora con muri ovunque, centrato nell'origine e di conseguenza modificare l'attuale gestione della scena in una scena multi-oggetto usando un grafo di scena.

![Labirinto completo, stage 2](resources/screenshots/stage2.png)

Adesso viene visualizzato tutto il labirinto (pratiamente una griglia) e si può spostare con il drag del mouse.

### Stage 3 (v0.4.x)

In questa tappa si modifica la generazione del labirinto andando a crearne uno vero e proprio seguendo un [algoritmo](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Iterative_implementation_(with_stack)) basato su una visita DFS randomizzata.

![Vero labirinto, stage 3](resources/screenshots/stage3.png)

### Stage 4 (v0.5.x)

L'obiettivo di questa tappa è attaccare delle texture in alta qualità al labrinto. In particolare ogni texture oltre al il colore (`diff`), ha anche una mappa di "ruvidezza" (`rough`) che indica quanto riflettere la luce e una mappa di normali (`nor`) che indica di quanto inclinare la normale di ogni voxel per far sembrare le ombre più realisiche.

Sono stati rimossi gli shader "Normal" e di Gouraud perché non più utili.

![Labirinto con shader flat, stage 4](resources/screenshots/stage4_flat.png)

![Dettagli fotorealismo texture, stage 4](resources/screenshots/stage4_photo.png)

### Stage 5 (v0.6.x)

Sono stati fatti miglioramenti alla GUI aggiungendo un contantore degli FPS per misurare le performance ed è stata aggiunta una schermata iniziale di caricamento per dare un feedback all'utente di cosa sta succedendo.

![Schermata di caricamento, stage 5](resources/screenshots/stage5.gif)

È stata migliorata anche la renderizzazione tramite phong degli spigoli dei muri.

| Prima (Spigoli rotti) | Dopo (Spigoli aggiustati) |
| :---: | :---: |
| ![Spigoli rotti, stage 5](resources/screenshots/stage5_broken.png) | ![Spigoli aggiustati, stage 5](resources/screenshots/stage5_new.png) |

## Crediti

È stato utilizzato [Gemini](https://gemini.google.com/) per la correzzione e il miglioramento dei testi/commenti sia nel codice che nella documentazione, oltre che nello sviluppo per velocizzare la risoluzione di *Code Smells* e altri *Issues* segnalati da [SonarQube](https://sonarcloud.io/project/overview?id=Tugamer89_Tu-Maze).

### Soluzioni tecniche

* **Algoritmo di generazione del labirinto**: L'algoritmo si basa su una versione randomizzata della visita DFS di un grafo, implementata tramite stack come descritto sulla seguente [pagina Wikipedia](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Iterative_implementation_(with_stack)).
* **Mapping triplanare**: Per il mapping triplanare delle texture è stata presa fortissima ispirazione da [questo articolo](https://catlikecoding.com/unity/tutorials/advanced-rendering/triplanar-mapping/) e la realizzazione è stata fortemente LLM-aided.

### Codice esterno

* **Template di base**: Il template di partenza è quello del Lab7 visto a lezione, a cui è stato fatto un po' refactoring e diversi piccoli miglioramenti.

### Risorse

* **Texture `cobblestone_pavement`**: Presa da [PolyHaven](https://polyhaven.com/a/cobblestone_pavement), fatta da [Charlotte Baglioni](https://www.artstation.com/wyrine).
* **Texture `mossy_brick`**: Presa da [PolyHaven](https://polyhaven.com/a/mossy_brick), fatta da [Amal Kumar](https://www.artstation.com/amalbubble).
