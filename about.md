# Safe Noclip

Un petit mod Geode qui ajoute un **bouton noclip** à l'écran en jeu et un **SafeMode** intégré.

## Fonctionnement
- **Bouton à l'écran** (coin haut-droit) : touche-le pour activer/désactiver le noclip.
  - **Vert** = noclip **activé**
  - **Rouge** = noclip **désactivé**
  - *(modifiable via le réglage `Green = noclip ON` si tu veux inverser les couleurs)*
- **Noclip** : évite que le joueur meure en touchant les obstacles.
- **SafeMode** : si le noclip a été utilisé pendant la tentative, le niveau **n'est pas compté comme terminé** au moment de la fin. `isCheated()` renvoie `true` si tu as triché, `false` si tu as fini le niveau à la normale.

## Réglages
| Réglage | Rôle |
|---|---|
| `Noclip enabled by default` | noclip activé à l'entrée dans un niveau |
| `Show toggle button` | afficher le bouton à l'écran |
| `Green = noclip ON` | inverser le code couleur |
| `SafeMode (block counted completions)` | activer/désactiver la protection anti-comptage |

## Plateformes
Windows, macOS, Android et **iOS** (le repo contient le workflow GitHub Actions pour builder l'`.geode` iOS arm64).
