# KFS-1 - Kernel From Scratch

## Description
KFS-1 (Kernel From Scratch) is a project that aim introducing kernel programing by making:
- A kernel you can boot via GRUB
- An ASM bootable base
- A basic kernel library, with basics functions and types
- Some basic code to print some stuff on the screen
- A basic "Hello world" kernel

## Features

### Boot Process
The kernel is loaded by GRUB through the Multiboot2 specification.
GRUB loads the kernel ELF image into memory and provides a Multiboot information structure containing framebuffer and memory information.

The assembly boot code initializes the CPU environment, sets up the stack, and transfers execution to the C kernel entry point.

### Framebuffer Driver
Provides direct access to the graphical framebuffer supplied by GRUB.
- Pixel drawing
- Screen clearing
- Color management
- Resolution detection

### Text Rendering
A monochrome bitmap font renderer is used to display text directly in the framebuffer.
- Character rendering
- String printing
- Newline support
- Cursor management

### Keyboard Driver
A PS/2 keyboard driver reads scancodes from I/O port 0x60 and translates them into ASCII characters.
- Key press detection
- Scancode translation
- Basic input handling

---

## Architecture

### Global Architecture
GRUB
↓
boot.s
↓
kernel_main()
↓
Drivers
├── Framebuffer
├── Keyboard
└── Utility Library

### Bootloader
GRUB is used as the bootloader.
The kernel follows the Multiboot2 specification, allowing GRUB to provide hardware information at boot time.

---

## Technical Choices

### Why Framebuffer Instead of VGA
Framebuffer was chosen instead of VGA text mode because:
- Works on modern hardware
- Allows custom graphics rendering
- Supports arbitrary resolutions
- Provides a better foundation for future GUI development

### Monochrome Bitmap Font
A bitmap font was chosen because it is simple to implement and does not require complex font parsing.
Each character is represented by a fixed-size bitmap stored directly in memory.

### PS/2 Keyboard
PS/2 was selected because it is easy to access through I/O ports and does not require USB stack implementation.

---

## Prerequisites

### Required Tools
- `make`
- `qemu-system-i386`
- `docker`
- `docker-compose`

To install them you can do:
```bash
sudo apt update
sudo apt install qemu-system-x86 docker docker-compose
```

### Recommended Environment
A Linux environment is recommended but in theory you can run it in every environment as long as
you have qemu installed and docker + docker-compose

---

## Installation

### Clone Repository
```bash
  git clone https://github.com/TheEternalice/KFS-1-Kernel-From-Scratch.git
```

### Build Docker Image
```bash
  make docker-build
```

### Launch Docker Environment
```bash
  make docker
```

### Build ISO
```bash
  make docker-iso
```

---

## Running

### Launch with QEMU
```bash
   qemu-system-i386 -cdrom ./target/kfs.iso
```

---

## Usage

### Keyboard Shortcuts
F1 F2 F3 F4 F5... to switch between differents screens

---

## Kernel Internals

### Boot Sequence
1. BIOS/UEFI starts GRUB
2. GRUB loads kernel
3. GRUB provides Multiboot information
4. boot.s initializes stack
5. kernel_main() starts
6. Drivers are initialized
7. Display

### Framebuffer Initialization
Framebuffer information is retrieved from the Multiboot framebuffer tag.
The driver stores:
- Width
- Height
- Pitch
- Bits per pixel
- Framebuffer address

### Text Rendering Pipeline
String
 ↓
Character
 ↓
Bitmap glyph
 ↓
Pixel drawing
 ↓
Framebuffer

### Keyboard Input Pipeline
Keyboard
 ↓
Scancode
 ↓
Translation table
 ↓
ASCII character
 ↓
Kernel input buffer

---

## Resources

### OS Development
http://wiki.osdev.org/Main_Page
http://wiki.osdev.org/Introduction
http://wiki.osdev.org/Getting_Started
http://wiki.osdev.org/How_kernel,_compiler,_and_C_library_work_together
http://wiki.osdev.org/UEFI
http://wiki.osdev.org/Bare_Bones
http://wiki.osdev.org/Boot_Sequence

### Multiboot Specification
https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

### Framebuffer Documentation
https://docs.kernel.org/fb/api.html

---

## Acknowledgements

### External Resources
https://wiki.osdev.org/PC_Screen_Font
https://www.youtube.com/watch?v=ELTwwTsR5w8

---

## AI Usage
AI was used to fix some typo in the readme and to make glyphs

---

# Version Française

## Description
KFS-1 (Kernel From Scratch) est un projet ayant pour objectif d'introduire la programmation système et le développement de noyaux en réalisant :
- Un noyau amorçable via GRUB
- Une base de démarrage en assembleur (ASM)
- Une bibliothèque noyau contenant les fonctions et types essentiels
- Du code permettant d'afficher du texte à l'écran
- Un noyau minimal affichant un simple text

## Fonctionnalités

### Processus de Démarrage
Le noyau est chargé par GRUB via la spécification Multiboot2.
GRUB charge l'image ELF du noyau en mémoire et fournit une structure d'informations Multiboot contenant notamment les informations relatives à la mémoire et au framebuffer.
Le code de démarrage en assembleur initialise l'environnement du processeur, configure la pile (stack), puis transfère l'exécution au point d'entrée du noyau écrit en C.

### Pilote Framebuffer
Fournit un accès direct au framebuffer graphique fourni par GRUB.
- Dessin de pixels
- Effacement de l'écran
- Gestion des couleurs
- Détection de la résolution

### Rendu de Texte
Un moteur de rendu basé sur une police bitmap monochrome est utilisé pour afficher du texte directement dans le framebuffer.
- Affichage de caractères
- Affichage de chaînes de caractères
- Gestion des retours à la ligne
- Gestion du curseur

### Pilote Clavier
Un pilote clavier PS/2 lit les scancodes depuis le port d'entrée/sortie 0x60 et les traduit en caractères ASCII.
- Détection des touches
- Traduction des scancodes
- Gestion basique des entrées utilisateur

---

## Architecture

### Architecture Globale
GRUB
↓
boot.s
↓
kernel_main()
↓
Pilotes
├── Framebuffer
├── Clavier
└── Bibliothèque utilitaire

### Chargeur de Démarrage
GRUB est utilisé comme chargeur de démarrage.
Le noyau respecte la spécification Multiboot2, permettant à GRUB de transmettre les informations matérielles nécessaires lors du démarrage.

---

## Choix Techniques

### Pourquoi le Framebuffer plutôt que le mode texte VGA ?
Le framebuffer a été choisi à la place du mode texte VGA car :
- Fonctionne sur du matériel moderne
- Permet un rendu graphique personnalisé
- Supporte des résolutions arbitraires
- Constitue une meilleure base pour le développement futur d'une interface graphique

### Police Bitmap Monochrome
Une police bitmap a été choisie car elle est simple à implémenter et ne nécessite aucun système complexe de chargement ou de rendu de polices.
Chaque caractère est représenté par une image bitmap de taille fixe directement stockée en mémoire.

### Clavier PS/2
Le protocole PS/2 a été choisi car il est simple d'accès via les ports d'entrée/sortie et ne nécessite pas l'implémentation d'une pile USB complète.

---

## Prérequis

### Outils Requis
- make
- qemu-system-i386
- docker
- docker-compose

Installation :
```bash
sudo apt update
sudo apt install qemu-system-x86 docker docker-compose
```

### Environnement Recommandé
Un environnement Linux est recommandé.
Cependant, le projet peut théoriquement être exécuté sur tout système disposant de QEMU ainsi que de Docker et Docker Compose.

---

## Installation

### Cloner le Dépôt
```bash
git clone https://github.com/TheEternalice/KFS-1-Kernel-From-Scratch.git
```

### Construire l'Image Docker
```bash
make docker-build
```

### Lancer l'Environnement Docker
```bash
make docker
```

### Générer l'ISO
```bash
make docker-iso
```

---

## Exécution

### Lancer avec QEMU
```bash
qemu-system-i386 -cdrom ./target/kfs.iso
```

---

## Utilisation

### Raccourcis Clavier
F1, F2, F3, F4, F5, etc. permettent de basculer entre les différents écrans disponibles.

---

## Fonctionnement Interne du Noyau

### Séquence de Démarrage
1. Le BIOS ou l'UEFI démarre GRUB
2. GRUB charge le noyau
3. GRUB fournit les informations Multiboot
4. boot.s initialise la pile
5. kernel_main() démarre
6. Les pilotes sont initialisés
7. L'affichage est activé

### Initialisation du Framebuffer
Les informations du framebuffer sont récupérées à partir du tag framebuffer de Multiboot.
Le pilote stocke :
- La largeur
- La hauteur
- Le pitch (nombre d'octets par ligne)
- Le nombre de bits par pixel
- L'adresse mémoire du framebuffer

### Pipeline de Rendu de Texte
Chaîne de caractères
↓
Caractère
↓
Glyphe bitmap
↓
Dessin des pixels
↓
Framebuffer

### Pipeline de Gestion du Clavier
Clavier
↓
Scancode
↓
Table de traduction
↓
Caractère ASCII
↓
Tampon d'entrée du noyau

---

## Ressources

### Développement de Systèmes d'Exploitation
- http://wiki.osdev.org/Main_Page
- http://wiki.osdev.org/Introduction
- http://wiki.osdev.org/Getting_Started
- http://wiki.osdev.org/How_kernel,_compiler,_and_C_library_work_together
- http://wiki.osdev.org/UEFI
- http://wiki.osdev.org/Bare_Bones
- http://wiki.osdev.org/Boot_Sequence

### Spécification Multiboot
- https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

### Documentation du Framebuffer
- https://docs.kernel.org/fb/api.html

---

## Remerciements

### Ressources Externes
- https://wiki.osdev.org/PC_Screen_Font
- https://www.youtube.com/watch?v=ELTwwTsR5w8

---

## Utilisation de l'IA
L'IA a été utilisée pour corriger certaines fautes dans le README ainsi que pour générer et concevoir certains glyphes.
