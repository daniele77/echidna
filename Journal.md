# Introduzione #

Questo è il "diario" del progetto. Descriverà e motiverà
le scelte effettuate in maniera cronologica.
Essendo esclusivamente un promemoria, è in italiano.

# 17 marzo 2012 #

**echidna** è una libreria C++ che permette di sviluppare applicazioni "ad eventi". Le sue caratteristiche principali sono:
  * bus eventi funzionante con il meccanismo publish-subscribe
  * la sottoscrizione avviene per **tipo**, specificando un predicato opzionale
  * possibile estensione ad ambienti multiprocesso e distribuiti
  * fornisce un contenitore di componenti la cui comunicazione può avvenire esclusivamente tramite eventi.

# 6 aprile 2012 #

Aggiungo un esempio tratto da http://www.carlopescio.com/2012/03/life-without-controller-case-1.html, per verificare che l'architettura della libreria consenta lo sviluppo di un'applicazione realistica.