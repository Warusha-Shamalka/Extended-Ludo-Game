
#ifndef TYPES_H
#define TYPES_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>

extern short diceValue,playerIndex,pieceIndex,maxIndex;
extern short roundNumber;
//this struct will store player names,how many pieces are there on the base and their initial roll value to determine the first player.
struct player 
{
    char playerName[8];
    short basePieceCount;
    short initialRoll;
    bool captureFlag;
};


// this is for store the all important detail of an each element.
struct piece 
{
    char playerName[8];
    char pieceName[3];
    bool capture;
    bool homeStraight;
    short state;
    short location;
    short homeStraightLocation;
    short direction;
};

// this is the array of piece structure.
extern struct piece piecesArray[4][4];
extern struct player playerNames[4];

void unique();
void pieceReset();
void arrengePlayers();
void gameRounds();
short pieceDirection(short,short);
long random();
void firstPlayer();
bool pieceCapture(short);
void pieceCaptureReset(short,short,short,short,short);
bool homeStraight(short,short);
void gameRounds();
bool getPieceFromBase(short);

#endif
