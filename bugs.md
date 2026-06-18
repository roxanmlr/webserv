# Bugs

> Revérifié le 2026-06-18 contre l'état actuel du code (y compris les modifications non commitées de `CgiHandler.cpp`).

## Statut

| ID  | Gravité       | Statut       | Composant         | Description courte                                                |
|-----|---------------|--------------|--------------------|---------------------------------------------------------------------|
| B1  | Bloquant      | ⬜ Ouvert    | StaticFileHandler | query string casse le filtre `.php`/`.py` dans canHandle           |
| B2  | Bloquant      | ⬜ Ouvert    | StaticFileHandler | query string dans le path → 404 sur toute URI avec `?`             |
| B3  | Bloquant      | ⬜ Ouvert    | Client            | réponse d'erreur jamais sérialisée → connexion fermée sans rien envoyer |
| B4  | Bloquant      | ✅ Fixé      | CgiHandler         | uri_strip_servername cassait canHandle pour les URIs HTTP/1.1      |
| B5  | Bloquant      | ✅ Fixé      | CgiHandler         | uri_strip_servername cassait la construction de script_path        |
| C1  | Moyen         | ⬜ Ouvert    | Client            | PARSE_ERROR répond 404 au lieu de 400 (getErrorCode() inutilisé)    |
| C2  | Moyen         | ✅ Fixé      | Client            | WRITE_READY posé sur requête INCOMPLETE                            |
| C3  | Moyen         | ✅ Fixé      | Client            | isMethodAllowed non vérifié pour CGI                                |
| C4  | Moyen         | ⚠️ Fixé partiel | Client          | aucune réponse si aucun handler ne matche → voir B3 (jamais envoyée)|
| C5  | Moyen         | ✅ Fixé      | Client             | wantsWrite retournait true sur WRITE_NOT_START                     |
| C6  | Moyen         | ✅ Fixé      | CgiHandler         | loc.getPath() : substring match au lieu de prefix match            |
| C7  | Moyen         | ✅ Fixé      | CgiHandler         | faux positif extension `.py` vs `.pyc`                             |
| C8  | Moyen         | ✅ Fixé      | CgiHandler         | handle() rappelait canHandle() — double appel avec effet de bord   |
| A1  | Architectural | ⬜ Ouvert    | WebServer          | CGI synchrone bloque le event loop entier                          |
| A2  | Sécurité      | ⬜ Ouvert    | WebServer          | pas de timeout d'inactivité client (slow loris)                    |
| A3  | Crash         | ⚠️ Réévalué  | WebServer          | listen_map shallow copy → double-close fd (client_map n'est plus copié, pas de double delete) |
| D1  | Cosmétique    | ⚠️ Partiel   | CgiHandler         | uri_strip_servername commenté mais toujours présent (code mort)    |
| D2  | Cosmétique    | ⬜ Ouvert    | CgiHandler         | setHeader("Content-Length") redondant dans parse_cgi_response      |

---

## Détail des bugs ouverts ou réévalués

### B1 — query string casse le filtre `.php`/`.py` dans StaticFileHandler::canHandle
**Fichier :** `src/handler/StaticFileHandler.cpp:32-43`

```cpp
std::string uri = req.getUri();
if (uri.length() >= 4 && uri.substr(uri.length() - 4) == ".php")
    return (false);
if (uri.length() >= 3 && uri.substr(uri.length() - 3) == ".py")
    return (false);
```

Aucun retrait de la query string avant le test de suffixe. `GET /script.php?x=1` ne se termine pas par `.php`, donc `canHandle` renvoie `true` : le fichier serait servi tel quel par `StaticFileHandler` (fuite de code source) au lieu d'être exclu/redirigé vers le CGI.

**Correction attendue :** stripper la query string (`uri.substr(0, uri.find('?'))`) avant les comparaisons de suffixe.

---

### B2 — query string dans le path construit par StaticFileHandler::handle
**Fichier :** `src/handler/StaticFileHandler.cpp:51`

```cpp
std::string path = rootPath + req.getUri();
```

`req.getUri()` inclut la query string. `GET /index.html?x=1` produit un chemin disque `.../index.html?x=1`, qui n'existe jamais → 404 systématique sur toute requête statique avec query string.

**Correction attendue :** stripper la query string avant de construire `path`.

---

### B3 — réponse d'erreur jamais sérialisée dans write_buffer
**Fichier :** `src/server/Client.cpp:124-176`

Dans `onWritable`, les branches de sortie anticipée (`READ_OVERFLOW` → 413, `READ_ERROR` → 400, `!serv` → 500, `optLoc.empty()` → 404, et le 404 final « aucun handler ne matche ») font toutes :

```cpp
response.setStatus(xxx);
response.setBody("...");
break;
```

mais ne font jamais `write_buffer = response.serialize();` (contrairement aux branches CGI/Static qui le font explicitement après `handle()`). Résultat : `write_buffer` reste vide, la boucle d'envoi (`while (write_pos < write_buffer.size())`) ne fait rien, `write_status` passe directement à `WRITE_DONE`, et `shouldClose()` devient vrai. Le client ferme la connexion **sans avoir envoyé un seul octet** — pas de ligne de statut, pas de corps, rien. C'est un remplacement plus sévère de l'ancien bug documenté (zombie client) : maintenant la connexion se ferme proprement mais silencieusement, sans réponse HTTP du tout.

Ce bug couvre aussi C4 : le 404 « aucun handler ne matche » est bien positionné sur `response`, mais jamais envoyé.

**Correction attendue :** ajouter `write_buffer = response.serialize();` avant chaque `break` de ces branches (ou factoriser un seul `write_buffer = response.serialize();` juste après le bloc, avant la boucle d'envoi).

---

### C1 — PARSE_ERROR répond 404 au lieu de 400
**Fichiers :** `src/server/Client.cpp:109-113` (onReadable), `src/server/Client.cpp:124-148` (onWritable)

`onReadable` détecte `IHttpRequest::PARSE_ERROR` et pose `write_status = WRITE_READY`, mais ne mémorise nulle part que le parsing a échoué. `onWritable` ne consulte que `read_status` (`READ_OVERFLOW`, `READ_ERROR`) — il ignore complètement l'état de parsing HTTP. Une requête malformée finit donc par passer par `serv->matchLocation(_request.getUri())` sur une URI potentiellement vide/partielle, et atterrit sur un 404 au lieu d'un 400.

`IHttpRequest::getErrorCode()` existe déjà (`src/http/HttpRequest.cpp:67`) et retourne le code d'erreur HTTP approprié, mais n'est appelé nulle part dans `Client.cpp`.

**Correction attendue :** stocker un flag (ou l'enum `ParseState`) sur `Client` lors du `feed()`, et dans `onWritable`, si parse a échoué, répondre `response.setStatus(_request.getErrorCode())` (ou 400 par défaut) avant les autres checks.

---

### A1 — CGI synchrone bloque le event loop
**Fichiers :** `src/server/WebServer.cpp:140-163` → `src/handler/CgiHandler.cpp::handle()`

`CgiHandler::handle()` est bloquant jusqu'à `TIMEOUT_SEC` (30 s). Appelé depuis `serveClient()` dans la boucle epoll, aucun autre client ne peut être servi pendant toute l'exécution du script CGI.

**Correction attendue :** exécuter le fork CGI depuis `onWritable`, stocker le pid et les fds dans le `Client`, et lire/écrire la sortie CGI de manière non-bloquante à chaque appel `onReadable`/`onWritable` via epoll.

---

### A2 — Pas de timeout d'inactivité client
**Fichier :** `src/server/WebServer.cpp`

`Client::lastActivity` est mis à jour à chaque `onReadable`/`onWritable` mais jamais consulté pour fermer les connexions inactives (attaque slow loris possible).

**Correction attendue :** dans la boucle `epoll_wait` (`WebServer.cpp:194-222`), à chaque itération, parcourir `client_map` et fermer les clients dont `time(NULL) - lastActivity > IDLE_TIMEOUT`.

---

### A3 — listen_map shallow copy (réévalué)
**Fichier :** `src/server/WebServer.cpp:22-34`

```cpp
WebServer& WebServer::operator=(WebServer const& other) {
    if (this == &other) return *this;
    this->listen_map   = other.listen_map;
    this->_shouldClose = other._shouldClose;
    return *this;
}
```

Contrairement à ce qui était documenté précédemment, `client_map` **n'est pas copié du tout** ici (ni dans le constructeur de copie) — donc pas de double `delete` sur les `IClient*`. En revanche, `listen_map` (qui contient des fds bruts) est copié superficiellement : deux instances de `WebServer` partageraient les mêmes fds d'écoute, et `stop()`/le destructeur de chaque instance ferme indépendamment ces mêmes fds → double `close()`, voire fermeture d'un fd réutilisé par le système entre les deux appels.

**Correction attendue :** rendre `WebServer` non-copiable (constructeur de copie et `operator=` privés, sans implémentation, en C++98) — la classe possède des ressources système (fds, clients) qui n'ont pas de sémantique de copie valable.

---

### D1 — uri_strip_servername : code mort commenté, pas supprimé
**Fichier :** `src/handler/CgiHandler.cpp:80-95`

La fonction est maintenant commentée (`/* ... */`) suite aux fixes B4/B5, donc elle ne compile plus, mais le code mort reste présent dans le fichier.

**Correction attendue :** supprimer le bloc commenté entièrement.

---

### D2 — setHeader("Content-Length") redondant dans parse_cgi_response
**Fichier :** `src/handler/CgiHandler.cpp:176`

```cpp
res.setHeader("Content-Length", ft_itoa(body.size()));  // redondant
res.setBody(body);  // setBody() met déjà à jour Content-Length
```

`HttpResponse::setBody()` recalcule et pose `Content-Length` automatiquement (`HttpResponse.cpp:44-51`). L'appel à `setHeader` juste avant est mort.

**Correction attendue :** supprimer cette ligne.

---

## Bugs déjà fixés (pour référence)

- **B4/B5** : `canHandle`/`handle` n'utilisent plus `uri_strip_servername`, stripent juste la query string.
- **C2** : `write_status` n'est plus mis à `WRITE_READY` que sur `COMPLETE` ou `PARSE_ERROR`, jamais sur `INCOMPLETE`.
- **C3** : `isMethodAllowed` vérifié dans les deux branches (CGI et Static) avant d'appeler `handle()`.
- **C5** : `wantsWrite()` retourne désormais `false` pour `WRITE_NOT_START` (`Client.cpp:226-227`).
- **C6/C7** : `CgiHandler::canHandle` vérifie maintenant un match d'extension exact en fin d'URI (`ext_pos + extension.size() == uri.size()`) et un match de préfixe sur `loc.getPath()` (`uri.find(loc.getPath(), 0) == 0`).
- **C8** : `handle()` ne rappelle plus `canHandle()`, utilise directement `this->cgipass` posé par l'appel précédent dans `Client.cpp`.
