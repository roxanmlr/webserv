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
│   └── utils/
│       ├── ILogger.hpp
│       ├── Logger.cpp / .hpp
│       ├── StringUtils.cpp / .hpp
│       └── FileUtils.cpp / .hpp

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
Reçoit un `IConfig` à l'initialisation. Crée les `ListenConfig` pour chaque paire `host:port` unique, puis entre dans la boucle `epoll_wait()` infinie via `run()`. Dispatche les événements vers les `IClient`, les `ICgiHandler` selon le type de fd.

#### `WebServer::ListenConfig`
Structure interne de `WebServer` qui wrappre un socket en état `LISTEN`, le `host:port` associe ainsi que `IServerConfig` associés pour que `IWebServer` sache quels blocs serveur s'appliquent sur ce port.

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

### Module Utils

#### `ILogger`
Permet de brancher facilement un logger fichier, stderr, ou silencieux pour les tests. Expose une seule méthode `log(level, msg)` avec les niveaux `DEBUG`, `INFO`, `WARN`, `ERROR`. Injecté par dépendance dans tous les modules qui en ont besoin, ce qui permet aux tests d'utiliser un `MockLogger` silencieux.

---

## Flux d'une requête

### Requête statique
1. `poll()` retourne, un `IServerSocket` a `POLLIN` → `acceptClient()` → créer un `IClient`, l'ajouter au poll en `POLLIN`.
2. `POLLIN` sur le fd du client → `client.onReadable()` → en interne : `recv()` + `request.feed(buffer)`.
3. Quand `feed()` renvoie `COMPLETE` → résoudre le `IServerConfig` (via header `Host`) → dispatcher vers le `IRequestHandler` qui matche → la réponse est sérialisée dans le write buffer → `wantsWrite()` devient `true` → le `WebServer` met le fd en `POLLOUT`.
4. `POLLOUT` → `client.onWritable()` → `send()` ce qu'on peut. Quand tout est envoyé : si keep-alive, reset l'état et repasse en `POLLIN` ; sinon `shouldClose()` devient `true` et le `WebServer` ferme.

---

## Points critiques du sujet

### Un seul `poll()`
Pas un poll par thread, pas un poll pour les clients et un autre pour les CGI. Tout dans le même tableau de `pollfd`. D'où l'importance de la map `fd → ConnectionType*` qui distingue : listen socket, client HTTP, pipe CGI.

### Non-bloquant partout
Tous les sockets (listen, accept, pipes CGI) sont marqués `O_NONBLOCK` **dès leur création**.

### Parsing du body
Ne pas lire `Content-Length` bytes d'un coup. Lire ce que `recv` donne, accumuler, et ne considérer la requête complète qu'à la bonne taille. Idem pour le chunked encoding.

### CGI et `POLLOUT`
Si le backend attend du stdin (POST body), le pipe/socket **vers** le backend doit aussi être dans poll en `POLLOUT`. Sinon deadlock possible si le body est gros.

### Timeouts
Aucune requête ne doit hang indéfiniment. `last_activity` par client **et**, fermeture au-delà d'un seuil.


# Instructions

> make
> ./webserv [configuration file]

Si aucun fichier n'est fourni, le serveur charge `config/default.conf`.


# Resources
