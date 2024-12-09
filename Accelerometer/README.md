Créateurs: 
  -Lucas Lalumière Longpré
  -Marc-Antoine Sauvé
Code pour le prototype du système d'affichage utilisé dans le projet de trottinette électrique. 
Calcule le roulis et le tangage d'un accéléromètre et l'affiche à un écran tactile. On peux aussi
tester la fonctionnalité tactile de l'écran. La DEL du MCU s'allume lorsque le roulis ou le tangage
est à plus de 30 degré (Trottinette sur le côté).

Librairies à installer:
  -TFT_eSPI (https://github.com/Bodmer/TFT_eSPI)

Plateforme: Platformio
Framework: Arduino
Langue: C++
Matériel:
  -MCU: ESP32-C3 DevkitC-02
  -Accel: MPU-6050
  -Écran: ILI9341
