_This project has been created as part of the 42 curriculum by lmilando and mzouhir._
# Description

Ce projet ecrit en C++98 nous a permis de nous initier au Web ainsi qu'au protocole HTTP et à nous exercer au C++ dans le cadre d'un projet complexe.
Il nous a permis notamment de mieux architecturer en decrivant le produit logiciel à partir d'une specification à savoir le RFC.

## Table des matieres
- [Vue d'ensemble](#vue-densemble)
- [Organisation des fichiers](#organisation-des-fichiers)
- [Modules et interfaces](#modules-et-interfaces)
- [Flux d'une requête](#flux-dune-requête)
- [Points critiques du sujet](#points-critiques-du-sujet)

---

## Vue d'ensemble

L'architecture repose sur des **modules = une interface abstraite + une (ou plusieurs) implementation(s) concrète(s)**. Cela permet :
- de découpler les modules ;
- de tester chaque module independamment (mocks faciles);
- d'ajouter de nouveaux handlers (par méthode, par type de ressource) sans toucher au reste.

L'orchestrateur unique (`WebServer`) possède la seule et unique bouble `poll()`. Tous les I\O réseau (sockets d'écoute, sockets clients, pipes CGI) passent par ce poll.
```
main.cpp
  └── IConfigParser → IConfig
  └── IWebServer (orchestrateur, possède le poll loop)
  		├── IServerSocket[] (un par port en écoute)
        ├── IClient[]         (connexions actives)
        │   ├── IHttpRequest (parser incremental)
        │ 	└── IHttpResponse (builder)
        ├── IRequestHandler (dispatch GET / POST / DELETE)
        ├── ICgiHandler       (CGI classique via fork+execve)
        └── IFastCgiClient    (FastCGI via socket persistant)
```

---
## Organisation des fichiers

Chaque module suit la convention : `IModule.hpp` (interface) + `Module.hpp/.cpp` (implementation)
```
webserv/
├── Makefile
├── README.md
├── config/
│   └── default.conf
├── src/
│   ├── main.cpp
│   ├── config/
│   │   ├── IConfig.hpp
│   │   ├── IConfigParser.hpp
│   │   ├── Config.cpp / .hpp
│   │   └── ConfigParser.cpp / .hpp
│   ├── server/
│   │   ├── IWebServer.hpp
│   │   ├── IServerSocket.hpp
│   │   ├── IClient.hpp
│   │   ├── WebServer.cpp / .hpp
│   │   ├── ServerSocket.cpp / .hpp
│   │   └── Client.cpp / .hpp
│   ├── http/
│   │   ├── IHttpRequest.hpp
│   │   ├── IHttpResponse.hpp
│   │   ├── HttpRequest.cpp / .hpp
│   │   ├── HttpResponse.cpp / .hpp
│   │   └── StatusCodes.cpp / .hpp
│   ├── handler/
│   │   ├── IRequestHandler.hpp
│   │   ├── RequestHandler.cpp / .hpp
│   │   ├── StaticFileHandler.cpp / .hpp
│   │   └── UploadHandler.cpp / .hpp
│   ├── cgi/
│   │   ├── ICgiHandler.hpp
│   │   └── CgiHandler.cpp / .hpp
│   ├── fastcgi/
│   │   ├── IFastCgiClient.hpp
│   │   ├── IFastCgiProtocol.hpp
│   │   ├── FastCgiClient.cpp / .hpp
│   │   ├── FastCgiProtocol.cpp / .hpp
│   │   ├── FastCgiRecord.cpp / .hpp
│   │   └── FastCgiHandler.cpp / .hpp
│   └── utils/
│       ├── ILogger.hpp
│       ├── Logger.cpp / .hpp
│       ├── StringUtils.cpp / .hpp
│       └── FileUtils.cpp / .hpp
└── tests/
    ├── ITestCase.hpp
    ├── TestRunner.cpp / .hpp
    ├── main_tests.cpp
    ├── unit/
    │   ├── ConfigParserTest.cpp
    │   ├── HttpRequestTest.cpp
    │   ├── HttpResponseTest.cpp
    │   ├── RequestHandlerTest.cpp
    │   ├── CgiHandlerTest.cpp
    │   └── FastCgiProtocolTest.cpp
    ├── integration/
    │   ├── StaticServingTest.cpp
    │   ├── UploadTest.cpp
    │   ├── DeleteTest.cpp
    │   ├── CgiIntegrationTest.cpp
    │   └── FastCgiIntegrationTest.cpp
    ├── stress/
    │   ├── siege_basic.sh
    │   └── stress_keepalive.sh
    ├── mocks/
    │   ├── MockClient.hpp
    │   ├── MockHttpRequest.hpp
    │   ├── MockHttpResponse.hpp
    │   ├── MockLogger.hpp
    │   └── MockFastCgiClient.hpp
    └── fixtures/
        ├── configs/
        │   ├── valid_simple.conf
        │   ├── valid_multi_server.conf
        │   ├── valid_fastcgi.conf
        │   └── invalid_syntax.conf
        ├── requests/
        │   ├── get_simple.raw
        │   ├── post_chunked.raw
        │   └── malformed.raw
        └── www/
            ├── index.html
            └── upload/
```
## Modules et interfaces
### Module Config
#### `IConfigParser`
Charges un fichier de configuration et produis un `IConfig`. Expose une seule méthode `parse(path)` qui retourne un pointeur vers `IConfig`. Utilisé uniquement par `main.cpp` au démarrage.

#### `IConfig`
Representes la configuration chargée. Donnes accès aux `IServerConfig` (un par bloc `server`), avec leurs `LocationConfig` (un par bloc `location`). Permet à `IWebServer` de résoudre le bon bloc serveur depuis le header `Host` via `findServer(host, port)`.

`IServerConfig` contient : ports, `server_name`, pages d'erreur, `client_max_body_size`, liste de `LocationConfig`.
`LocationConfig` contient : `root`, `index`, méthodes autorisées, `autoindex`, redirections, parametres CGI, chemin d'upload.

---

### Module Server
#### `IWebServer`
Reçoit un `IConfig` à l'initialisation. Crée les `IServerSocket` pour chaque paire `host:port` unique, puis entre dans la boucle `poll()` infinie via `run()`. Dispatche les événements vers les `IClient`, les `ICgiHandler` et les `IFastCgiClient` selon le type de fd.

#### `IServerSocket`
Wrappre un socket en état `LISTEN`. Expose `getFd()` pour l'enregistrement dans `poll()` et `acceptClient()` qui retourne le fd du client accepté en mode non-bloquant. Fournit aussi les `IServerConfig` associés pour que `IWebServer` sache quels blocs serveur s'appliquent sur ce port.

Setup : `socket()` → `setsockopt(SO_REUSEADDR)` → `fcntl(O_NONBLOCK)` → `bind()` → `listen()`.

#### `IClient`
Représente une connexion HTTP active. `IWebServer` l'appelle sur `onReadable()` (POLLIN) et `onWritable()` (POLLOUT). L'état interne (`READING_HEADERS`, `READING_BODY`, `PROCESSING`, `WRITING`, `CLOSING`) détermine ce que retournent `wantsRead()`, `wantsWrite()` et `shouldClose()`, permettant à `IWebServer` de mettre à jour le masque `pollfd` sans connaître le détail du protocole HTTP.

### Module HTTP
#### `IHttpRequest`
Parser **incrémental** : les données arrivent en morceaux via poll. `feed(data, len)` accumule les octets et retourne `INCOMPLETE`, `COMPLETE` ou `PARSE_ERROR`. Une fois `COMPLETE`, `IClient` peut interroger `getMethod()`, `getUri()`, `getHeader()` et `getBody()` pour construire la réponse via `IRequestHandler`.

États internes : request line → headers → body (gestion `Content-Length` **et** `Transfer-Encoding: chunked`).

#### `IHttpResponse`
Builder de réponse HTTP. `IRequestHandler` appelle `setStatus()`, `setHeader()` et `setBody()`, puis `IClient` sérialise le tout via `serialize()` pour l'envoyer au socket. Gères les headers obligatoires : `Date`, `Server`, `Content-Length` ou `Transfer-Encoding`, `Connection`.

### Module Handler
#### `IRequestHandler`
Interface commune à tous les handlers. `IWebServer` (via le dispatcher `RequestHandler`) appelle d'abord `canHandle(req, loc)` sur chaque handler enregistré, puis délègue à `handle(req, loc, res)` au premier qui accepte. La réponse est construite directement dans le `IHttpResponse` fourni.

Implémentations concrètes :
- **`StaticFileHandler`** : GET sur fichier, autoindex, MIME types.
- **`UploadHandler`** : POST avec `multipart/form-data` ou body brut.
- **`DeleteHandler`** : DELETE sur ressource.
- **`RedirectHandler`** : `return 301/302` configuré.
- **`CgiHandler`** : exécution CGI classique (fork + execve).
- **`FastCgiHandler`** : délègue à un backend FastCGI (PHP-FPM, etc.) via socket.

Le `RequestHandler` principal (le dispatcher) tient la liste des handlers et délègue au premier dont `canHandle()` renvoie `true`. Vérifications préalables : méthode autorisée (sinon `405`), taille body (sinon `413`).

### Module CGI

#### `ICgiHandler`
Étend `IRequestHandler`. En plus de `handle()`, expose `getInputFd()` et `getOutputFd()` (les deux pipes) afin que `IWebServer` les ajoute au tableau `pollfd`. `onInputWritable()` est appelé sur `POLLOUT` du pipe stdin (envoi du body POST), `onOutputReadable()` sur `POLLIN` du pipe stdout (lecture de la réponse CGI). Quand `isFinished()` passe à `true`, `IWebServer` retire les pipes du poll et réveille le `IClient` HTTP.

Workflow :
1. Crée deux `pipe()`, les marque `O_NONBLOCK`.
2. `fork()` puis dans l'enfant : `dup2()` + `execve()` du binaire (`php-cgi`, `python`, …).
3. Les pipes sont ajoutés au `poll()` principal — mappés vers le `IClient` qui attend.
4. Le processus CGI lui-même n'est pas dans poll (`waitpid` non-bloquant ou `SIGCHLD`).

---

### Module FastCGI

Le module FastCGI est une alternative **bien plus performante** au CGI classique. Au lieu de forker un processus par requête, le webserver maintient une connexion socket TCP (ou Unix) persistante vers un backend (typiquement **PHP-FPM**, `python-fastcgi`, etc.) déjà lancé. Chaque requête est encodée selon le **protocole FastCGI** (records binaires) et envoyée sur cette socket.

**Avantages** :
- Pas de `fork()` ni `execve()` à chaque requête (zéro overhead processus).
- Le backend gère lui-même son pool de workers.
- Connexions multiplexables (plusieurs requêtes en parallèle sur la même socket via `requestId`).

**Configuration type (`location`)** : `fastcgi_pass 127.0.0.1:9000` (ou socket Unix), avec les params `SCRIPT_FILENAME` et `QUERY_STRING`.

#### `IFastCgiProtocol`
Encode et décode les **records FastCGI**. C'est un protocole binaire avec une en-tête fixe de 8 octets : `version, type, requestId, contentLength, paddingLength, reserved`. Utilisé exclusivement par `IFastCgiClient` pour construire les buffers à envoyer et interpréter les réponses du backend.

Types de records utilisés :
- `FCGI_BEGIN_REQUEST` (1) — début d'une requête, indique le rôle (`RESPONDER`).
- `FCGI_PARAMS` (4) — paires clé/valeur (l'équivalent des variables d'environnement CGI : `REQUEST_METHOD`, `SCRIPT_FILENAME`, etc.).
- `FCGI_STDIN` (5) — body de la requête HTTP envoyé au backend.
- `FCGI_STDOUT` (6) — réponse du backend (headers + body HTTP).
- `FCGI_STDERR` (7) — erreurs du backend (à logger).
- `FCGI_END_REQUEST` (3) — fin de la requête côté backend.

#### `IFastCgiClient`
Représente **une connexion** vers un backend FastCGI. Comme un `IClient` HTTP, il est piloté par le poll loop : son fd vit dans le même `pollfd[]` que les sockets HTTP. `IWebServer` appelle `onReadable()` et `onWritable()` selon les événements poll. `start(req, loc)` déclenche l'envoi des records FastCGI ; une fois l'état `FINISHED`, `getResponseHeaders()` et `getResponseBody()` alimentent le `IHttpResponse` du `IClient` HTTP appelant.

États : `CONNECTING` → `SENDING` → `RECEIVING` → `FINISHED` / `ERROR`.

#### `FastCgiHandler`
Implémente `IRequestHandler`. Quand il intercepte une requête (extension `.php` par exemple) :

1. Ouvre une socket TCP/Unix non-bloquante vers `fastcgi_pass`.
2. Crée un `FastCgiClient` lié au `IClient` HTTP appelant.
3. Enregistre la socket FastCGI dans le poll principal du `WebServer`.
4. Construit la map de paramètres CGI (`REQUEST_METHOD`, `SCRIPT_FILENAME`, `CONTENT_LENGTH`, `HTTP_*`, etc.).
5. Envoie `BEGIN_REQUEST` → `PARAMS` → `STDIN` (au fur et à mesure que `POLLOUT` est signalé).
6. À chaque `POLLIN`, accumule `STDOUT` du backend → quand `END_REQUEST` arrive, parse les headers HTTP du backend, construit `IHttpResponse` et marque le `IClient` HTTP comme prêt à écrire.

#### Multiplexing (optionnel)

Le protocole FastCGI permet de réutiliser **une seule socket** pour plusieurs requêtes simultanées via le `requestId` (entier 16 bits). Pour simplifier, l'implémentation initiale peut ouvrir **une socket par requête** (PHP-FPM ne supporte de toute façon pas le multiplexing). Une version avancée maintient un pool de connexions persistantes.

---

### Module Utils

#### `ILogger`
Permet de brancher facilement un logger fichier, stderr, ou silencieux pour les tests. Expose une seule méthode `log(level, msg)` avec les niveaux `DEBUG`, `INFO`, `WARN`, `ERROR`. Injecté par dépendance dans tous les modules qui en ont besoin, ce qui permet aux tests d'utiliser un `MockLogger` silencieux.

---

## Module de test

Le dossier `tests/` est un module à part entière, compilé via une cible Makefile dédiée (`make test`). Il ne pollue pas le binaire `webserv` final.

### Philosophie

Grâce aux **interfaces abstraites** définies plus haut, chaque module est testable en isolation : il suffit de fournir des **mocks** (implémentations factices) des interfaces dont la classe testée dépend. Trois niveaux de tests :

1. **Tests unitaires** (`tests/unit/`) : une classe = un fichier de tests. Utilisent les mocks.
2. **Tests d'intégration** (`tests/integration/`) : lancent un vrai `WebServer` sur un port aléatoire et envoient des requêtes via socket TCP.
3. **Tests de stress** (`tests/stress/`) : scripts shell utilisant `siege`, `wrk` ou `ab` pour vérifier la disponibilité sous charge (exigé par le sujet).

### `ITestCase`

Interface minimale d'un cas de test (pas besoin de framework externe, conforme à la norme C++98). Expose `name()` et `run()` — `run()` lève une `std::runtime_error` en cas d'échec. Enregistré dans `TestRunner` via `add()`.

### `TestRunner`

Enregistre tous les `ITestCase` via `add()`, les exécute en séquence, et imprime un rapport (vert/rouge, total, durée). Retourne le nombre d'échecs pour le code de retour du processus.

### Mocks

Les mocks implémentent les interfaces avec un comportement contrôlable depuis le test. Par exemple `MockHttpResponse` stocke le statut, les headers et le body dans des champs publics directement inspectables après l'appel à `handler.handle(...)`. Même principe pour `MockClient`, `MockLogger`, `MockFastCgiClient`, etc.

### Fixtures

Le dossier `tests/fixtures/` contient des fichiers réutilisables :
- **`configs/`** : configurations valides et invalides pour tester le parser.
- **`requests/`** : requêtes HTTP brutes (octet par octet) pour tester le parser HTTP, y compris malformées.
- **`www/`** : arborescence de fichiers statiques servie par les tests d'intégration.

### Tests d'intégration

Pattern type : le test fork le serveur sur un port libre, attend qu'il écoute, envoie une requête via `socket()` + `connect()`, lit la réponse, et compare. À la fin : `kill()` + `waitpid()`.

### Tests de stress

Le sujet impose explicitement de stress-tester. Les scripts `siege_basic.sh` et `stress_keepalive.sh` utilisent `siege` et `ab` avec 50–100 connexions simultanées.
Vérifier : aucun crash, aucune fuite mémoire (`valgrind`), aucune connexion qui hang, disponibilité ≥ 99.5 %.

### Test FastCGI spécifique

Le protocole FastCGI étant binaire, on teste l'encodage/décodage des records indépendamment du réseau via `FastCgiProtocolTest`. Pour les tests d'intégration FastCGI, lancer **php-fpm** en background sur un port de test, puis vérifier que le webserver exécute correctement un `phpinfo()`.

## Flux d'une requête

### Requête statique
1. `poll()` retourne, un `IServerSocket` a `POLLIN` → `acceptClient()` → créer un `IClient`, l'ajouter au poll en `POLLIN`.
2. `POLLIN` sur le fd du client → `client.onReadable()` → en interne : `recv()` + `request.feed(buffer)`.
3. Quand `feed()` renvoie `COMPLETE` → résoudre le `IServerConfig` (via header `Host`) → dispatcher vers le `IRequestHandler` qui matche → la réponse est sérialisée dans le write buffer → `wantsWrite()` devient `true` → le `WebServer` met le fd en `POLLOUT`.
4. `POLLOUT` → `client.onWritable()` → `send()` ce qu'on peut. Quand tout est envoyé : si keep-alive, reset l'état et repasse en `POLLIN` ; sinon `shouldClose()` devient `true` et le `WebServer` ferme.

### Requête FastCGI
1. Étapes 1–2 identiques.
2. Le dispatcher choisit `FastCgiHandler` (extension `.php` par ex.) → ouvre une socket vers `fastcgi_pass` → crée un `FastCgiClient` → l'ajoute au poll en `POLLOUT`.
3. `POLLOUT` sur la socket FastCGI → envoie `BEGIN_REQUEST`, puis `PARAMS`, puis `STDIN` (body).
4. `POLLIN` sur la socket FastCGI → accumule les records `STDOUT` → à la réception de `END_REQUEST`, parse les headers du backend et construit la `IHttpResponse` du client HTTP.
5. Le `IClient` HTTP passe alors en `POLLOUT` et envoie la réponse au navigateur (comme une requête statique).

À tout moment, `POLLHUP` / `POLLERR` ou `recv()` qui retourne `0` → fermer proprement (côté HTTP ET côté FastCGI).

---

## Points critiques du sujet

### Pas de test d'`errno` après `read`/`write`
Si `recv()` retourne `-1`, on ferme la connexion, point. Pas de `if (errno == EAGAIN)`. Le poll doit donc **garantir** que le fd est prêt — c'est pour ça que tout passe par poll, **y compris** les pipes CGI et les sockets FastCGI.

### Un seul `poll()`
Pas un poll par thread, pas un poll pour les clients et un autre pour les CGI/FastCGI. Tout dans le même tableau de `pollfd`. D'où l'importance de la map `fd → ConnectionType*` qui distingue : listen socket, client HTTP, pipe CGI, socket FastCGI.

### Non-bloquant partout
Tous les sockets (listen, accept, pipes CGI, sockets FastCGI) sont marqués `O_NONBLOCK` **dès leur création**. Pour FastCGI : `connect()` non-bloquant retourne `-1` avec `EINPROGRESS` — on attend `POLLOUT` pour savoir que la connexion est établie (sans tester `errno` après le `connect`, mais en testant `getsockopt(SO_ERROR)` une fois `POLLOUT` signalé).

### Parsing du body
Ne pas lire `Content-Length` bytes d'un coup. Lire ce que `recv` donne, accumuler, et ne considérer la requête complète qu'à la bonne taille. Idem pour le chunked encoding. Idem pour les records FastCGI.

### CGI/FastCGI et `POLLOUT`
Si le backend attend du stdin (POST body), le pipe/socket **vers** le backend doit aussi être dans poll en `POLLOUT`. Sinon deadlock possible si le body est gros.

### Timeouts
Aucune requête ne doit hang indéfiniment. `last_activity` par client **et** par connexion FastCGI, fermeture au-delà d'un seuil (30-60 s).


# Instructions

```makefile
test:        $(NAME)_test
	./$(NAME)_test

$(NAME)_test: $(TEST_OBJS) $(SRC_OBJS_NO_MAIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

stress: all
	./tests/stress/siege_basic.sh
```

Si aucun fichier n'est fourni et la variable d'environnement  $WEBSERVER_CONF est vide, le serveur charge `config/default.conf`.

Pour tester FastCGI en local, lancer un backend séparément :
```bash
php-fpm --nodaemonize --fpm-config /etc/php-fpm.conf  # écoute sur :9000
```

# Resources
