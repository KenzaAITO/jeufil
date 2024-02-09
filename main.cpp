/*
Author : Dimitri Britan et Romain Bayol
Comment : Pierrick Ledys
*/
#include "mbed.h"
#include <cstdlib>

// Déclaration des identifiants de message
const uint16_t START = 0x200;
const uint16_t JEUXFINI = 0x201;
const uint16_t ERREUR = 0x202;

// Déclaration de la liaison
CAN can1(p30, p29);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
// Déclaration des broches GPIO
AnalogIn fil1(p17);  //fil vert
AnalogIn fil2(p18);  //fil gris
AnalogIn fil3(p19);  //fil rouge
AnalogIn fil4(p20);  //fil bleu

// Déclaration du message CAN
CANMessage msg;

// Fonction principale
int main() {
    std::srand(std::time(nullptr));
    can1.frequency(250000);
    int game_status=0;
    std::srand(static_cast<unsigned>(time(nullptr)));

    while (1) {
        // Envoi du message CAN
        if (can1.read(msg)) {
            // Traitement du message
            switch(msg.id){
                case START:
                    led1 = *reinterpret_cast<int*>(msg.data);
                    game_status=1;
                    break;
            }
        if (game_status==1) {
            break;  // Sortir de la boucle
        } //quitter la lecture pour lancer le jeux
        }
    }
    int randomValue = std::rand()%4; //Génère un nombre aléatoire entre 0 et 3
    while (1) {
        // Lecture des tensions
        int voltage1 = fil1.read() * 3.3;
        int voltage2 = fil2.read() * 3.3;
        int voltage3 = fil3.read() * 3.3;
        int voltage4 = fil4.read() * 3.3;

        if(randomValue==0){ //enlever fil bleu et gris
            led1 = (voltage1 >=3) ? 0 : 1;
            led2 = (voltage4 >=3) ? 1 : 0;
            led3 = (voltage3 >=3) ? 0 : 1;
            led4 = (voltage2 >=3) ? 1 : 0;
        }
        if(randomValue==1){ //enlever fil rouge
            led1 = (voltage2 >=3) ? 0 : 1;
            led2 = (voltage3 >=3) ? 1 : 0;
            led3 = (voltage4 >=3) ? 0 : 1;
            led4 = (voltage1 >=3) ? 0 : 1;
        }
        if(randomValue==2){ //enlever fil bleu
            led1 = (voltage1 >=3) ? 0 : 1;
            led2 = (voltage3 >=3) ? 0 : 1;
            led3 = (voltage2 >=3) ? 0 : 1;
            led4 = (voltage4 >=3) ? 1 : 0;
        }
        if(randomValue==3){ //enlever fil gris rouge bleu
            led1 = (voltage4 >=3) ? 0 : 1;
            led2 = (voltage1 >=3) ? 1 : 0;
            led3 = (voltage3 >=3) ? 1 : 0;
            led4 = (voltage2 >=3) ? 1 : 0;
        }

        // Envoi du message CAN en cas d'erreur
        if (randomValue==0 && (led1 == 1 || led3 == 1)) {
            game_status = 1;
            can1.write(CANMessage(ERREUR, reinterpret_cast<char*>(game_status),sizeof(game_status)));
            break;
        }

        // Envoi du message CAN en cas de victoire
        if (randomValue==0 && led2 == 0 && led4 == 0) {
            game_status = 2;
            can1.write(CANMessage(JEUXFINI, reinterpret_cast<char*>(game_status),sizeof(game_status)));
            break;
        }
    }
    // Éteindre les LEDs
    led1=0;
    led2=0;
    led3=0;
    led4=0;

}
