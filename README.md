# 🤖 Robot base différentiel ESP32 S3

Ce projet pilote un robot mobile à deux roues motrices équipé d'encodeurs quadrature sur les moteurs et aura par la suite des encodeurs déportés sur suspensions. 

## ✨ Fonctionnalités
* **Architecture Dual-Core :** Le Cœur 1 gère la physique des moteurs (Régulateurs PID, Profils Spatio-temporels), le Cœur 0 gère le Wi-Fi et l'Interface Web.
* **Profil Cinématique Hybride :** Génération automatique de profils d'accélération en Trapèze ou Triangle selon la distance demandée.
* **Gamepad API :** Contrôle au joystick (ex: Manette Switch Pro) directement depuis le navigateur Web (sans fil).
* **OTA (Over-The-Air) :** Mises à jour du code par le Wi-Fi.

## 🔌 Câblage Matériel
| Composant ESP32 | Broche | Description |
| :--- | :--- | :--- |
| **Moteur 1 (Gauche)** | GPIO 4 | Direction (DIR) |
| | GPIO 5 | Puissance (PWM) |
| | GPIO 15 | Encodeur Phase A |
| | GPIO 16 | Encodeur Phase B |
| **Moteur 2 (Droit)** | GPIO 6 | Direction (DIR) |
| | GPIO 7 | Puissance (PWM) |
| | GPIO 17 | Encodeur Phase A |
| | GPIO 18 | Encodeur Phase B |

Des encodeurs déportés seront rajoutés par la suite

## 🚀 Installation & Sécurité
1. Clonez ce dépôt dans VSCode avec PlatformIO.
2. Allez dans le dossier `include/`.
3. Dupliquez le fichier `secrets_template.h` et renommez-le en `secrets.h`.
4. Insérez vos identifiants Wi-Fi dans `secrets.h` (ce fichier est ignoré par Git pour votre sécurité).
5. Compilez et téléversez !

## ⚙️ Liste du matériel utilisé

- Les moteurs : 128,40€  : https://fr.aliexpress.com/item/4000414958534.html?spm=a2g0o.order_list.order_list_main.5.3e245e5bDmvX9Y&gatewayAdapt=glo2fra ( version DC24  i5 1640 RPM)
- Un gros BAU résistant à 25A : 13.69€ https://fr.aliexpress.com/item/1005004163545345.html?spm=a2g0o.order_list.order_list_main.10.3e245e5bDmvX9Y&gatewayAdapt=glo2fra ( version SA2-M3 - REDLED )
- 2 nouveaux blocs pour batteries makita avec interrupteur : 20.77€ https://fr.aliexpress.com/item/1005009675112454.html?spm=a2g0o.order_list.order_list_main.20.3e245e5bDmvX9Y&gatewayAdapt=glo2fra
- 3 KG de petg CF noir : 47.40€ https://fr.aliexpress.com/item/1005006759639025.html?spm=a2g0o.order_list.order_list_main.15.324b5e5bqGRehB&gatewayAdapt=glo2fra
- Contrôleur moteur : 47,80€ https://www.gotronic.fr/art-commande-moteur-cc-2x20-a-mdd20a-37247.htm
- ESP32 S3 : 8.99€ : https://fr.aliexpress.com/item/1005008867880664.html?spm=a2g0o.order_list.order_list_main.25.53de5e5bDqreen&gatewayAdapt=glo2fra ( version N16R8 Onboard KIT A )

Total pour le moment : 268.05€ 
