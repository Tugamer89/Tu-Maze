# Relazione

Questo progetto mira a espandere e migliorare il progetto originario [Maze](https://github.com/Tugamer89/maze), implementando un motore di rendering 3D nativo basato su **OpenGL**. Inoltre, è stata integrata la libreria [ImGui](https://github.com/ocornut/imgui) per fornire un'interfaccia grafica intuitiva (GUI) utile alla gestione in tempo reale dei parametri di scena.

Il gameplay si sviluppa in prima persona: l'obiettivo del giocatore è fuggire nel minor tempo possibile da un labirinto generato proceduralmente a ogni partita, orientandosi esclusivamente tramite l'ausilio di una minimappa.

## Tappe

### Stage 1 (v0.2.x)

L'obiettivo di questa prima fase è stato predisporre un ambiente di sviluppo solido. È stato configurato un template di base che include correttamente [ImGui SFML](https://github.com/SFML/imgui-sfml) affiancato al codice necessario per gestire un primo spazio tridimensionale tramite **OpenGL**.

![Vista di default, stage 1](resources/screenshots/stage1.png)

In questo stage viene visualizzato un semplice pavimento, ruotabile liberamente trascinando il cursore con il tasto sinistro del mouse. È presente inoltre un pannello GUI rudimentale per testare la modifica in tempo reale di alcuni parametri visivi.

### Stage 2 (v0.3.x)

Il focus si è spostato sulla generazione della griglia di base del labirinto. Per ora la struttura è riempita interamente di muri e posizionata al centro dell'origine degli assi. Questa modifica ha richiesto una transizione architetturale da una scena a singolo oggetto a una scena complessa gestita tramite un **grafo di scena** (*Scene Graph*).

![Labirinto completo, stage 2](resources/screenshots/stage2.png)

Il risultato visivo è una griglia completa esplorabile spostando la telecamera tramite input del mouse.

### Stage 3 (v0.4.x)

La generazione della mappa è stata implementata con la logica di un vero e proprio labirinto. È stato adottato un [algoritmo](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Iterative_implementation_(with_stack)) basato su una visita DFS (*Depth-First Search*) randomizzata, che garantisce percorsi sempre diversi e completabili.

![Vero labirinto, stage 3](resources/screenshots/stage3.png)

### Stage 4 (v0.5.x)

L'obiettivo di questa tappa è innalzare la fedeltà visiva applicando texture ad alta risoluzione ai modelli del labirinto. È stato sviluppato un sistema di materiali che utilizza, per ogni superficie, tre mappe:

* **Albedo (`diff`)**: per il colore base.
* **Roughness (`rough`)**: per determinare la ruvidezza del materiale e la dispersione dei riflessi.
* **Normal (`nor`)**: per manipolare le normali dei pixel, simulando micro-rilievi e restituendo un'illuminazione molto più realistica.

A fronte di queste novità, gli shader storici "Normal" e "Gouraud" sono stati rimossi in quanto obsoleti.

![Labirinto con shader flat, stage 4](resources/screenshots/stage4_flat.png)

![Dettagli fotorealismo texture, stage 4](resources/screenshots/stage4_photo.png)

### Stage 5 (v0.6.x)

Sono state apportate migliorie all'interfaccia utente, aggiungendo un contatore degli FPS per monitorare le performance grafiche. È stata inoltre introdotta una schermata di caricamento progressiva per fornire feedback all'utente durante l'inizializzazione sincrona degli asset pesanti.

![Schermata di caricamento, stage 5](resources/screenshots/stage5.gif)

Parallelamente, il sistema di illuminazione Phong è stato fixato e raffinato per correggere fastidiosi artefatti visivi agli angoli dei muri.

| Prima (Spigoli rotti) | Dopo (Spigoli corretti) |
| :---: | :---: |
| ![Spigoli rotti, stage 5](resources/screenshots/stage5_broken.png) | ![Spigoli aggiustati, stage 5](resources/screenshots/stage5_new.png) |

### Stage 6 (v0.7.x)

L'obiettivo di questa tappa è stata l'ottimizzazione radicale delle prestazioni (FPS), andando a risolvere un pesante collo di bottiglia generato dalla GPU (*Fill-rate bottleneck*).

Il **mapping triplanare** utilizzato nello stage precedente per applicare le texture costringeva il *Fragment Shader* a eseguire ben 9 campionamenti in memoria video per ogni singolo pixel a schermo (calcolando i 3 assi per le mappe di colore, normali e ruvidezza e poi fondendoli insieme).

Sfruttando la natura architettonica del labirinto, composto esclusivamente da pareti ortogonali perfettamente allineate agli assi cartesiani, la tecnica è stata sostituita con un più efficiente **Box Mapping** (*Single-Sample Dominant-Axis Mapping*). Lo shader ora identifica matematicamente l'asse dominante della faccia osservata ed effettua il campionamento delle texture una sola volta per quel piano specifico. Questa modifica ha ridotto il carico sulla memoria video del 66%, mantenendo intatto il fotorealismo ma garantendo un drastico aumento dei fotogrammi al secondo.

## Crediti

Lo sviluppo è stato supportato da **[Gemini](https://gemini.google.com/)**, utilizzato per la correzione e il miglioramento di testi e commenti (sia nel codice che nella documentazione), oltre che come assistente per velocizzare la risoluzione di *Code Smells* e altre *Issue* architetturali segnalate da **[SonarQube](https://sonarcloud.io/project/overview?id=Tugamer89_Tu-Maze)**.

### Soluzioni tecniche

* **Algoritmo di generazione del labirinto**: L'algoritmo si basa su una versione randomizzata della visita DFS di un grafo, implementata in maniera iterativa tramite stack come descritto nell'apposita [pagina Wikipedia](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Iterative_implementation_(with_stack)).
* **Dal Mapping Triplanare al Box Mapping**: Inizialmente le texture venivano applicate tramite un approccio *Triplanar Mapping* (ispirato a [questo articolo](https://catlikecoding.com/unity/tutorials/advanced-rendering/triplanar-mapping/)). Per risolvere severi colli di bottiglia prestazionali, l'approccio è stato convertito in un *Dominant-Axis Box Mapping*, un'ottimizzazione standard nei motori grafici basati su griglie ortogonali che proietta la texture su un singolo piano per evitare il costoso calcolo di interpolazione multi-asse. Maggiori dettagli su Wikipedia alla voce [Box Mapping](https://en.wikipedia.org/wiki/Texture_mapping#Box_mapping).

### Codice esterno

* **Template di base**: Il template di partenza è basato sul Lab7 del corso universitario, a cui è stato applicato un pesante refactoring architetturale unito a diversi miglioramenti qualitativi.

### Risorse

* **Texture `cobblestone_pavement`**: Creata da [Charlotte Baglioni](https://www.artstation.com/wyrine), prelevata in CC0 da [PolyHaven](https://polyhaven.com/a/cobblestone_pavement).
* **Texture `mossy_brick`**: Creata da [Amal Kumar](https://www.artstation.com/amalbubble), prelevata in CC0 da [PolyHaven](https://polyhaven.com/a/mossy_brick).
