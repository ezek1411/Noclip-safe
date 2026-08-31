# Safe Noclip

Mod Geode qui ajoute un bouton noclip (vert = on, rouge = off) + un **SafeMode**
qui empêche le niveau d'être compté comme terminé si le noclip a été utilisé.

- **Icône** : `logo.png` (chargée automatiquement par Geode).
- **Sources dans un zip** : `noclip-safe-source.zip`
- **2 workflows GitHub** : `extract.yml` (décompresse le zip) puis `build.yml` (compile pour iOS).

## Flux de build (sans Mac)

Le dépôt GitHub ne contient que le **zip** + les **2 workflows**. GitHub fait tout le reste :

1. **`extract.yml`** (`Extract Source Zip`) — à chaque push, il **décompresse**
   `noclip-safe-source.zip` et **commit les sources** dans le dépôt.
2. **`build.yml`** (`Build Mod (iOS)`) — lancé automatiquement **juste après**
   l'extraction (`workflow_run`), il **compile** le mod pour **iOS** sur un runner macOS
   et sort le `.geode` en artifact **SafeNoclip-iOS**.

### Étapes
1. Crée un repo GitHub et pousse :
   - `noclip-safe-source.zip`
   - `.github/workflows/extract.yml`
   - `.github/workflows/build.yml`
2. GitHub Actions : le workflow **Extract Source Zip** décompresse et commit les sources.
3. Le workflow **Build Mod (iOS)** s'enchaîne, compile et publie l'artifact **SafeNoclip-iOS**.
4. Télécharge `me.noclip-safe.geode` (iOS arm64).

> 🔧 Si le build iOS échoue (SDK trop ancien), décommente **`sdk: 'nightly'`** dans
> `.github/workflows/build.yml`.
> Le `build.yml` contient aussi un filet de sécurité : si les sources ne sont pas
> encore extraites, il décompresse lui-même le zip avant de builder.

## Installer sur ton iPhone
- Installe le **launcher Geode iOS** par sideloading (TrollStore recommandé, iOS 14+).
- Launcher → Settings → *About* → maintiens appuyé **iOS Launcher** 3 s →
  active **Developer Mode** → active le **Web Server** (ou `scp`).
- Envoie le `.geode` :
  ```bash
  DEVICE_URL="http://IP-DE-TON-IPHONE:8080"
  curl -X POST -F "file=@me.noclip-safe.geode" "${DEVICE_URL}/upload"
  curl -X POST "${DEVICE_URL}/launch"   # optionnel
  ```
- Active le mod dans le menu Geode. Le bouton apparaît en haut à droite en jeu.

## Structure du code
- `src/main.cpp` — bouton (CCDrawNode, aucune dépendance sprite), hook `destroyPlayer`.
- `src/SafeMode.hpp` / `.cpp` — état noclip + `isCheated()` + hook `levelComplete`
  qui force `m_isTestMode` pour ne pas compter une complétion triché.
- `mod.json` — métadonnées + réglages (noclip by default, show button, couleurs, safe-mode).
- `logo.png` — icône du mod (chargée par Geode).
- `CMakeLists.txt` — build (arm64 sur iOS).

## Note d'implémentation
Le hook `destroyPlayer(PlayerObject*, GameObject*)` suit la signature 2.2 commune.
Si ta binding diffère, `geode build` te signalera la signature attendue par le header
`Geode/modify/PlayLayer.hpp` — tu n'as qu'à l'adapter.
