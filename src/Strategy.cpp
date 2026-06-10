#include "Strategy.h"
#include "MotorControl.h"

int stratStep = 0;
bool stratRunning = false;

const float STRAT_SPEED = 1000.0; // Vitesse de 0.5 m/s
const float PIVOT_DIST = (3.1415926535 * 307) / 4.0; // Quart de cercle pour 90°

void nextStep() {
    switch(stratStep) {
        case 0: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 1
        case 1: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Pivot Gauche 1
        case 2: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 2
        case 3: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Pivot Gauche 2
        case 4: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 3
        case 5: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Pivot Gauche 3
        case 6: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 4
        case 7: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Retour face avant
        case 8: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 1
        case 9: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Pivot Gauche 1
        case 10: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 2
        case 11: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Pivot Gauche 2
        case 12: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 3
        case 13: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Pivot Gauche 3
        case 14: moveDistance(0, 1000.0, STRAT_SPEED); break; // Côté 4
        case 15: moveDistance(1, -PIVOT_DIST, STRAT_SPEED); moveDistance(2, PIVOT_DIST, STRAT_SPEED); break; // Retour face avant
        default: 
            stratRunning = false; 
            break; // Séquence terminée
    }
}

void startStrategy() {
    stratStep = 0;
    stratRunning = true;
    nextStep();
}

void stopStrategy() {
    stratRunning = false;
    setVelocity(0, 0.0); // Arrêt d'urgence
}

void updateStrategy() {
    if (!stratRunning) return;
    
    // Si les deux moteurs ont fini leur mouvement, on passe à l'étape suivante
    if (isMovementFinished()) {
        stratStep++;
        nextStep();
    }
}