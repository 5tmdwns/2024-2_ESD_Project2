/*
 * stuffAlertUart.h
 *
 *  Created on: Nov 25, 2024
 *      Author: root
 */
#include "main.h"

#ifndef INC_STUFFMANAGE_H_
#define INC_STUFFMANAGE_H_

typedef enum stuff {
    Seaweed = 0, // SmallBites
    Potato, // SmallBites
    SalmonRoe, // SmallBites
    Deodeok, // SmallBites
    Radish, // SmallBites
    RedCabbage, // SmallBites
    Almond, // AlmondWithCavior
    Cavior, // AlmondWithCavior
    Tofu, // AlmondWithCavior
    Egg, // AlmondWithCavior
    Yuzu, // SnowCrabAndPickledChrysanthemum
    Snowcrab, // SnowCrabAndPickledChrysanthemum
    Beansprouts, // SnowCrabAndPickledChrysanthemum
    Ablalone, // AblaloneTaco
    KoreanBeef, // HearthOvenGrilledHanwoo
    Apple, // HearthOvenGrilledHanwoo
    Onion, // HearthOvenGrilledHanwoo
    Acorn, // EmberToastedAcornNoodle
    Truffle, // EmberToastedAcornNoodle
    Noddle, // EmberToastedAcornNoodle
    Sesame, // BlackSesameSeaurchinNasturtium
    Burdock, // BurdockTarteTatinWithSkinJuice
    Tilefish, // TilefishMustardBrassica
    Mustard, // TilefishMustardBrassica
    Brassica, // TilefishMustardBrassica
    Tuna, // fattyTuna
    Hibiscus, // Smallsweets
    Peanut, // Smallsweets
    HoneyCookie, // Smallsweets
    Kombucha // Smallsweets
} stuff;

const char* stuffToString(stuff index);

void transmitString(const char* message);

void printRemainingStuff(stuff index);

extern uint8_t remainingStuff[30];


#endif /* INC_STUFFMANAGE_H_ */
