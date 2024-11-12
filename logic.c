#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"

#define ROUND printf("\n>> Round number %d \n", ++roundNumber)
#define UPDATE_LOCATION_CLOCKWISE piecesArray[playerIndex][pieceIndex].location = (piecesArray[playerIndex][pieceIndex].location + diceValue) % 52
#define UPDATE_LOCATION_COUNTER_CLOCKWISE piecesArray[playerIndex][pieceIndex].location = ((piecesArray[playerIndex][pieceIndex].location - diceValue) + 52) % 52
#define PIECEINFO_CLOCKWISE printf(">> %s moves %s piece from %d to %d by %d units in CLOCKWISE direction \n", piecesArray[playerIndex][pieceIndex].playerName, piecesArray[playerIndex][pieceIndex].pieceName, piecesArray[playerIndex][pieceIndex].location, (piecesArray[playerIndex][pieceIndex].location + diceValue) % 52, diceValue)
#define PIECEINFO_COUNTER_CLOCKWISE printf(">> %s moves %s piece from %d to %d by %d units in COUNTER CLOCKWISE direction \n", piecesArray[playerIndex][pieceIndex].playerName, piecesArray[playerIndex][pieceIndex].pieceName, piecesArray[playerIndex][pieceIndex].location, ((piecesArray[playerIndex][pieceIndex].location - diceValue) + 52) % 52, diceValue)
#define YELLOW_START 2
#define YELLOW_APPROACH 52
#define BLUE_START 15
#define BLUE_APPROACH 13
#define RED_START 28
#define RED_APPROACH 26
#define GREEN_START 41
#define GREEN_APPROACH 39
#define SIZE 4

short diceValue,playerIndex,pieceIndex,maxIndex = -1;
short roundNumber = 0;

enum capture      { NOT_CAPTURED, CAPTURED };
enum state        { BASE,BOARD,HOME,REMOVED};
enum direction    { NOT_SET = -1, CLOCKWISE, COUNTER_CLOCKWISE };
enum homeStraight { NO, YES };


// this is the array of player structure.
struct player playerNames[4]=
{
    {"YELLOW", 4, 0},
    {"BLUE", 4, 0},
    {"RED", 4, 0},
    {"GREEN", 4, 0}
};

// this is for store the all important detail of an each element.

// this is the array of piece structure.
struct piece piecesArray[4][4];


// this gives a random number.
long random() 
{
    return rand() % 6 + 1;
}

// to decide the first player to play.
void firstPlayer()
 {
    for (short playerIndex = 0; playerIndex < 4; playerIndex++) 
    {
        playerNames[playerIndex].initialRoll = random();
    }
    unique();
}

// this checks whether there are two or more players who get the same maximum number or not.
void unique() 
{
    int max = 0;
    for (short playerIndex = 0; playerIndex < 4; playerIndex++) 
    {
        if (max < playerNames[playerIndex].initialRoll) 
        {
            max = playerNames[playerIndex].initialRoll;
            maxIndex = playerIndex; // change the playerIndex to maxPlayerIndex
        } 
        else if (max == playerNames[playerIndex].initialRoll) 
        {
            firstPlayer();
            return;
        }
    }
}

// this function will rearrange players according to the first player.
void arrengePlayers() 
{
    // this prints thier first roll values
    for (short playerIndex = 0; playerIndex < 4; playerIndex++) 
    {
        printf(">> %s rolls %d \n", playerNames[playerIndex].playerName, playerNames[playerIndex].initialRoll);
    }
    printf(">> %s has the highest value and will begin the game.\n>> The order of a single round is; \n", playerNames[maxIndex].playerName);
    
    struct player temp[SIZE];

    // this creates a temp array and rearranges the array of piece structer according to the player order.
    for (short playerIndex = 0; playerIndex < SIZE; playerIndex++) 
    {
        temp[playerIndex] = playerNames[(playerIndex + maxIndex) % SIZE];
    }
    for (short playerIndex = 0; playerIndex < SIZE; playerIndex++) 
    {
        playerNames[playerIndex] = temp[playerIndex];

        // this prints the order of a round.
        printf(">> %d = %s \n", playerIndex + 1, playerNames[playerIndex].playerName);
    }
    pieceReset();
}

// this func will reset the piece's state, direction, capture state, and other details.
void pieceReset() 
{
    for (short playerIndex = 0; playerIndex < 4; playerIndex++) 
    {
        for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
        {
            // to recognize the player according to the playerNames array.
            strcpy(piecesArray[playerIndex][pieceIndex].playerName, playerNames[playerIndex].playerName);

            // to give piece for its names.			
            sprintf(piecesArray[playerIndex][pieceIndex].pieceName, "%c%d", playerNames[playerIndex].playerName[0], pieceIndex + 1);
            
            piecesArray[playerIndex][pieceIndex].state = BASE;	
            piecesArray[playerIndex][pieceIndex].capture = NOT_CAPTURED;	
            piecesArray[playerIndex][pieceIndex].homeStraight = NO;
            piecesArray[playerIndex][pieceIndex].direction = NOT_SET;			
            piecesArray[playerIndex][pieceIndex].homeStraightLocation = 0 ;	
            
            if (strncmp(piecesArray[playerIndex][pieceIndex].playerName,"YELLOW",1)  == 0 ) 
            {
                piecesArray[playerIndex][pieceIndex].location = YELLOW_START;
            } 
            else if (strncmp(piecesArray[playerIndex][pieceIndex].playerName,"BLUE",1)  == 0) 
            {
                piecesArray[playerIndex][pieceIndex].location = BLUE_START;
            } 
            else if (strncmp(piecesArray[playerIndex][pieceIndex].playerName,"RED",1) == 0) 
            {
                piecesArray[playerIndex][pieceIndex].location = RED_START;
            } 
            else if (strncmp(piecesArray[playerIndex][pieceIndex].playerName,"GREEN",1) == 0) 
            {
                piecesArray[playerIndex][pieceIndex].location = GREEN_START;
            }
        }
    }
}

void pieceCaptureReset(short opponentIndex,short opponentPieceNumber,short myPieceNumber,short playerIndex,short diceValue)
{
    piecesArray[opponentIndex][opponentPieceNumber].state = BASE;
    piecesArray[opponentIndex][opponentPieceNumber].capture = NOT_CAPTURED;	
    piecesArray[playerIndex][myPieceNumber].capture = CAPTURED;	
    piecesArray[opponentIndex][opponentPieceNumber].direction = NOT_SET;
    playerNames[opponentIndex].basePieceCount ++;
}

// this checks whether there are any pieces to be captured and if it is, reset its members.
bool pieceCapture(short playerIndex)
{
    for (short opponentIndex = (playerIndex + 1)%4 ; playerIndex != opponentIndex % 4 ; opponentIndex++) 
    {
        for (short myPieceNumber = 0; myPieceNumber < 4; myPieceNumber++) 
        {
            for (short opponentPieceNumber = 0; opponentPieceNumber < 4; opponentPieceNumber++) 
            {
                if (piecesArray[playerIndex][myPieceNumber].direction == CLOCKWISE)
                {
                    if ( piecesArray[opponentIndex][opponentPieceNumber].location == (piecesArray[playerIndex][myPieceNumber].location + diceValue) % 52 && piecesArray[opponentIndex][opponentPieceNumber].state == BOARD && piecesArray[playerIndex][myPieceNumber].state == BOARD)
                    {
                        if (strncmp(piecesArray[opponentIndex][opponentPieceNumber].playerName,"YELLOW",1) == 0 ) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = YELLOW_START;
                        } 
                        else if (strncmp(piecesArray[opponentIndex][opponentPieceNumber].playerName,"BLUE",1) == 0) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = BLUE_START;
                        } 
                        else if (strncmp(piecesArray[opponentIndex][opponentPieceNumber].playerName,"RED",1) == 0) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = RED_START;
                        } 
                        else if (strncmp (piecesArray[opponentIndex][opponentPieceNumber].playerName,"GREEN",1) == 0) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = GREEN_START;
                        }
                        pieceCaptureReset(opponentIndex,opponentPieceNumber,myPieceNumber,playerIndex,diceValue);
                        printf(">> %s %s piece lands on square %d, captures %s %s piece, and returns it to the base.\n", piecesArray[playerIndex][myPieceNumber].playerName, piecesArray[playerIndex][myPieceNumber].pieceName, (piecesArray[playerIndex][myPieceNumber].location + diceValue) % 52, piecesArray[opponentIndex][opponentPieceNumber].playerName, piecesArray[opponentIndex][opponentPieceNumber].pieceName);
                        piecesArray[playerIndex][myPieceNumber].location = (piecesArray[playerIndex][myPieceNumber].location + diceValue) % 52;  
                        playerNames[playerIndex].captureFlag = true;
			            return false;
                    }   
                }
                else if( piecesArray[playerIndex][myPieceNumber].direction == COUNTER_CLOCKWISE )
                {
                    if ( piecesArray[opponentIndex][opponentPieceNumber].location == (piecesArray[playerIndex][myPieceNumber].location - diceValue + 52) % 52 && piecesArray[opponentIndex][opponentPieceNumber].state == BOARD && piecesArray[playerIndex][myPieceNumber].state == BOARD)
                    {
                        if (strncmp(piecesArray[opponentIndex][opponentPieceNumber].playerName,"YELLOW",1) == 0) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = YELLOW_START;
                        } 
                        else if (strncmp(piecesArray[playerIndex][opponentPieceNumber].playerName,"BLUE",1) == 0) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = BLUE_START;
                        } 
                        else if (strncmp(piecesArray[playerIndex][opponentPieceNumber].playerName,"RED",1) == 0) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = RED_START;
                        } 
                        else if (strncmp(piecesArray[playerIndex][opponentPieceNumber].playerName,"GREEN",1) == 0) 
                        {
                            piecesArray[opponentIndex][opponentPieceNumber].location = GREEN_START;
                        }
                        pieceCaptureReset(opponentIndex,opponentPieceNumber,myPieceNumber,playerIndex,diceValue);
                        printf(">> %s %s piece lands on square %d, captures %s %s piece, and returns it to the base.\n", piecesArray[playerIndex][myPieceNumber].playerName, piecesArray[playerIndex][myPieceNumber].pieceName, (piecesArray[playerIndex][myPieceNumber].location - diceValue + 52) % 52, piecesArray[opponentIndex][opponentPieceNumber].playerName, piecesArray[opponentIndex][opponentPieceNumber].pieceName);
                        piecesArray[playerIndex][myPieceNumber].location = (piecesArray[playerIndex][myPieceNumber].location - diceValue + 52) % 52; 
                        playerNames[playerIndex].captureFlag = true;
			            return false;
                    }  
                }
            }
        }
    }
    return true;
}

// to check the direction of a piece.
short pieceDirection(short playerIndex,short pieceIndex) 
{
    piecesArray[playerIndex][pieceIndex].direction = ((short)rand()) % 2;
    if (piecesArray[playerIndex][pieceIndex].direction == CLOCKWISE )
    {
        return 0;
    }
    else if (piecesArray[playerIndex][pieceIndex].direction == COUNTER_CLOCKWISE)
    {
        return 1;
    }
}

bool check_All_The_Pieces_Are_In_Home(short playerIndex) 
{
    for (short pieceIndex = 0 ; pieceIndex < 4 ; pieceIndex++ )
    {
        if (piecesArray[playerIndex][pieceIndex].homeStraightLocation != 6)
        {
            return false;
        }
    }
    return true;
}

// to get the pieces out from the base.
bool getPieceFromBase(short playerIndex) 
{
    for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
    {	
        // if there is any piece still in the base,this will take it to the board and sets its state
        if (piecesArray[playerIndex][pieceIndex].state == BASE) 
        {
            piecesArray[playerIndex][pieceIndex].state = BOARD;
            printf(">> %s moves %s piece to the starting point \n", piecesArray[playerIndex][pieceIndex].playerName, piecesArray[playerIndex][pieceIndex].pieceName);
            playerNames[playerIndex].basePieceCount --;
            if ( pieceDirection(playerIndex,pieceIndex) == 0 )
            {
                piecesArray[playerIndex][pieceIndex].direction = CLOCKWISE;
                return false;
            }
            else
            {
                piecesArray[playerIndex][pieceIndex].direction = COUNTER_CLOCKWISE;
                return false;
            }
        }
    }
    return true;
}

bool homeStraight(short diceValue,short playerIndex)
{   
    //piece capture should be as captured.piece should pass the approch once if it is going on clockwise.
    for (playerIndex,pieceIndex = 0 ; pieceIndex < 4 ; pieceIndex++ )
    {
        if (piecesArray[playerIndex][pieceIndex].capture == CAPTURED && piecesArray[playerIndex][pieceIndex].direction == CLOCKWISE)
        {   
            if (piecesArray[playerIndex][pieceIndex].state == HOME )
            {   
                if (piecesArray[playerIndex][pieceIndex].homeStraightLocation + diceValue < 6 )
                {   
                    piecesArray[playerIndex][pieceIndex].homeStraightLocation += diceValue;
                    printf(">> %s piece %s is %d units away from the home\n",playerNames[playerIndex].playerName,piecesArray[playerIndex][pieceIndex].pieceName,(6 - piecesArray[playerIndex][pieceIndex].homeStraightLocation));
                    return false;
                }   
                else if ( piecesArray[playerIndex][pieceIndex].homeStraightLocation + diceValue == 6)
                {
                    piecesArray[playerIndex][pieceIndex].homeStraightLocation += diceValue;
                    printf(">> %s piece %s Has reached HOME \n",playerNames[playerIndex].playerName,piecesArray[playerIndex][pieceIndex].pieceName);
                    piecesArray[playerIndex][pieceIndex].state == REMOVED;
                    return false;
                } 
                 
            }  
            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"YELLOW",1)  == 0) && ((piecesArray[playerIndex][pieceIndex].location < YELLOW_APPROACH && piecesArray[playerIndex][pieceIndex].location + diceValue > YELLOW_APPROACH) || piecesArray[playerIndex][pieceIndex].location == 0)  && piecesArray[playerIndex][pieceIndex].state == BOARD )
            { 
                if ( piecesArray[playerIndex][pieceIndex].location == 0 )
                {
                    piecesArray[playerIndex][pieceIndex].location = YELLOW_APPROACH;
                }  
                piecesArray[playerIndex][pieceIndex].homeStraightLocation = piecesArray[playerIndex][pieceIndex].location + diceValue - YELLOW_APPROACH;
                printf(">> YELLOW %s piece is now on HOMEPATH cell %d \n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;
            }

            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"BLUE",1)  == 0) && piecesArray[playerIndex][pieceIndex].location <= BLUE_APPROACH && piecesArray[playerIndex][pieceIndex].location + diceValue > BLUE_APPROACH && piecesArray[playerIndex][pieceIndex].state == BOARD) 
            {   
                piecesArray[playerIndex][pieceIndex].homeStraightLocation = (piecesArray[playerIndex][pieceIndex].location + diceValue) - BLUE_APPROACH;
                printf(">> BLUE %s piece is now on HOMEPATH cell %d \n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;

            }

            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"RED",1) == 0) && piecesArray[playerIndex][pieceIndex].location + diceValue > RED_APPROACH && piecesArray[playerIndex][pieceIndex].location <= RED_APPROACH && piecesArray[playerIndex][pieceIndex].state == BOARD) 
            {   
                piecesArray[playerIndex][pieceIndex].homeStraightLocation = (piecesArray[playerIndex][pieceIndex].location + diceValue) - RED_APPROACH;
                printf(">> RED %s piece is now on HOMEPATH %d\n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;
            }

            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"GREEN",1)  == 0) && piecesArray[playerIndex][pieceIndex].location + diceValue > GREEN_APPROACH && piecesArray[playerIndex][pieceIndex].location <= GREEN_APPROACH && piecesArray[playerIndex][pieceIndex].state == BOARD) 
            {  
                piecesArray[playerIndex][pieceIndex].homeStraightLocation = (piecesArray[playerIndex][pieceIndex].location + diceValue) - GREEN_APPROACH;
                printf(">> GREEN %s piece is now on HOMEPATH %d\n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;
            }
        }

        else if( piecesArray[playerIndex][pieceIndex].capture == CAPTURED && piecesArray[playerIndex][pieceIndex].direction == COUNTER_CLOCKWISE)      
        {
            if (piecesArray[playerIndex][pieceIndex].state == HOME)
            {        
                //piecesArray[playerIndex][pieceIndex].homeStraightLocation += diceValue;
                if (piecesArray[playerIndex][pieceIndex].homeStraightLocation + diceValue < 6)
                {
                    piecesArray[playerIndex][pieceIndex].homeStraightLocation += diceValue;         
                    printf(">> %s piece %s is %d units away from the home\n",playerNames[playerIndex].playerName,piecesArray[playerIndex][pieceIndex].pieceName,(6 - piecesArray[playerIndex][pieceIndex].homeStraightLocation));
                    return false;
                }
                else if ( piecesArray[playerIndex][pieceIndex].homeStraightLocation + diceValue == 6)
                {
		    piecesArray[playerIndex][pieceIndex].homeStraightLocation += diceValue;
                    printf(">> %s piece %s has reached HOME \n",playerNames[playerIndex].playerName,piecesArray[playerIndex][pieceIndex].pieceName);
                    piecesArray[playerIndex][pieceIndex].state == REMOVED;
                    return false;
                }    
            }
            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"YELLOW",1)  == 0) && piecesArray[playerIndex][pieceIndex].location - diceValue < 0 && piecesArray[playerIndex][pieceIndex].state == BOARD)
            {  
                piecesArray[playerIndex][pieceIndex].homeStraightLocation = YELLOW_APPROACH  - ( piecesArray[playerIndex][pieceIndex].location - diceValue + 52 )%52 ;
                printf("YELLOW %s peice is now on HOMEPATH %d\n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;
            }

            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"BLUE",1)  == 0) && piecesArray[playerIndex][pieceIndex].location - diceValue < BLUE_APPROACH && piecesArray[playerIndex][pieceIndex].location >= BLUE_APPROACH && piecesArray[playerIndex][pieceIndex].state == BOARD) 
            {   
                piecesArray[playerIndex][pieceIndex].homeStraightLocation =  BLUE_APPROACH - ( piecesArray[playerIndex][pieceIndex].location - diceValue );
                printf("BLUE %s peice is now on HOMEPATH %d\n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;
            }

            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"RED",1) == 0) && piecesArray[playerIndex][pieceIndex].location - diceValue < RED_APPROACH && piecesArray[playerIndex][pieceIndex].location >= RED_APPROACH && piecesArray[playerIndex][pieceIndex].state == BOARD) 
            {   
                piecesArray[playerIndex][pieceIndex].homeStraightLocation = RED_APPROACH - (piecesArray[playerIndex][pieceIndex].location - diceValue) ;
                printf("RED %s peice is now on HOMEPATH %d\n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;
            }

            else if ((strncmp(piecesArray[playerIndex][pieceIndex].playerName,"GREEN",1)  == 0) && piecesArray[playerIndex][pieceIndex].location - diceValue < GREEN_APPROACH && piecesArray[playerIndex][pieceIndex].location >= GREEN_APPROACH && piecesArray[playerIndex][pieceIndex].state == BOARD)  
            {   
                piecesArray[playerIndex][pieceIndex].homeStraightLocation = GREEN_APPROACH - (piecesArray[playerIndex][pieceIndex].location - diceValue) ;
                printf("GREEN %s peice is now on HOMEPATH %d\n",piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                piecesArray[playerIndex][pieceIndex].state = HOME;
                return false;
            }       
        }
    }
    return true;
}
             

void gameRounds() 
{
    bool nextRound=true;
    while (nextRound) 
    {
        ROUND;

        // start game rounds
        for (short playerIndex = 0; playerIndex < 4; playerIndex++) 
        {
            short pieceIndex = 0;
            diceValue = random();
            printf(">> %s player rolled %d \n", playerNames[playerIndex].playerName, diceValue);
            if (homeStraight(diceValue,playerIndex))
            {   
                // if no pieces on the homestraight , this will happen.
                if (diceValue == 6) 
                {	
                    if (pieceCapture(playerIndex)) 
                    {   
                        // if there is nothing to capture , this will happen.	
                        if (getPieceFromBase(playerIndex)) 
                        {	
                            // if there is no piece to take from base , this will happen.
                            if ( piecesArray[playerIndex][pieceIndex].direction == CLOCKWISE )
                            {
                                for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
                                {
                                    if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                    {
                                        PIECEINFO_CLOCKWISE;
					                    UPDATE_LOCATION_CLOCKWISE;
                                        break;
                                    }    
                                }			                
                            }
                            else if ( piecesArray[playerIndex][pieceIndex].direction == COUNTER_CLOCKWISE )
                            {
                                for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
                                {
                                    if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                    {
                                        PIECEINFO_COUNTER_CLOCKWISE;
                                        UPDATE_LOCATION_COUNTER_CLOCKWISE;
                                        break;
                                    }    
                                }			 
                            }
                        }
                    }
                    // when we get 6 as the first roll value , this will give player to another chance to roll the dice.(second chance) 
                    diceValue = random();
                    printf(">> %s player rolled %d \n", playerNames[playerIndex].playerName, diceValue);
                    if (homeStraight(diceValue,playerIndex))
                    {               
                         // if there is no pieces on the homestraight , this will happen.(second chance)
                        if (diceValue == 6) 
                        {
                            if (pieceCapture(playerIndex)) 
                            {	
                                // if there is nothing to capture , this will happen.(second chance).
                                if (getPieceFromBase(playerIndex)) 
                                {       
                                    // if there is no piece to take from base , this will happen.(second chance).
                                    if ( piecesArray[playerIndex][pieceIndex].direction == CLOCKWISE )
                                    {
                                        for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
                                        {
                                            if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                            {
                                                PIECEINFO_CLOCKWISE;
                                                UPDATE_LOCATION_CLOCKWISE;
                                                break;
                                            }
                                        }   
                                    }			 
                                    else if ( piecesArray[playerIndex][pieceIndex].direction == COUNTER_CLOCKWISE)
                                    {
                                        for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
                                        {
                                            if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                            {
                                                PIECEINFO_COUNTER_CLOCKWISE;
                                                UPDATE_LOCATION_COUNTER_CLOCKWISE;
                                                break;
                                            }
                                        }
                                    }
                                }
                            
                            }
                            // this will give the third chance to the player if he gets two consecative sixes. 
                            diceValue = random();
                            printf(">> %s player rolled %d \n", playerNames[playerIndex].playerName, diceValue);
                            if (diceValue == 6) 
                            {	
                                printf(">> Pass the turn to the next player !!!\n");
                            } 
                            else 
                            {	
                                // if the third roll doesnot equals to 6,this section will happen.
                                if (homeStraight(diceValue,playerIndex))
                                {
                                    if (pieceCapture(playerIndex)) 
                                    {
                                        if ( piecesArray[playerIndex][pieceIndex].direction == CLOCKWISE )
                                        {
                                            for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
                                            {
                                                if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                                {
                                                    PIECEINFO_CLOCKWISE;
                                                    UPDATE_LOCATION_CLOCKWISE;
                                                    
                                                    break;
                                                }
                                            }
                                        }
                                        else if ( piecesArray[playerIndex][pieceIndex].direction == COUNTER_CLOCKWISE )
                                        {
                                            if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                                {
                                                    PIECEINFO_COUNTER_CLOCKWISE;
                                                    UPDATE_LOCATION_COUNTER_CLOCKWISE;
                                                    
                                                    break;
                                                }
                                        }
                                    }				
                                }
                            }
                        }
                     
                         // if the second chance is not equals to six , this will happen.(second chance)
                        else 
                        {
                            if (homeStraight(diceValue,playerIndex))
                            {
                                if (pieceCapture(playerIndex)) 
                                {
                                    if ( piecesArray[playerIndex][pieceIndex].direction == CLOCKWISE )
                                    {
                                        for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
                                        {
				       		                if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                            {
                                                PIECEINFO_CLOCKWISE;
                                                UPDATE_LOCATION_CLOCKWISE;
                                                
                                                break;
                                            }
                                        }
                                    }
                                    else if ( piecesArray[playerIndex][pieceIndex].direction == COUNTER_CLOCKWISE )
                                    {
                                        for (short pieceIndex = 0; pieceIndex < 4; pieceIndex++) 
                                        {
                                            if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                                            {
                                                PIECEINFO_COUNTER_CLOCKWISE;
                                                UPDATE_LOCATION_COUNTER_CLOCKWISE;
                                                
                                                break;
                                            }
                                        }
                                    }			
                                }			
                            }
                        }
                    }
                }
                else
                { 
                    // if the first chance is not equals to six,this will happen.
                    for (pieceIndex = 0 ; pieceIndex < 4 ; pieceIndex++)
                    {
                        if (piecesArray[playerIndex][pieceIndex].state == BOARD) 
                        {
                            if (homeStraight(diceValue,playerIndex))
                            {
                                if (pieceCapture(playerIndex))
                                {
                                    if ( piecesArray[playerIndex][pieceIndex].direction == CLOCKWISE )
                                    {
                                        PIECEINFO_CLOCKWISE;    
                                        UPDATE_LOCATION_CLOCKWISE;
                                        break;
                                    }
                                    else if ( piecesArray[playerIndex][pieceIndex].direction == COUNTER_CLOCKWISE)
                                    {			
                                        PIECEINFO_COUNTER_CLOCKWISE;    
                                        UPDATE_LOCATION_COUNTER_CLOCKWISE;
                                        break;
                                    }    
                                }
                                break;                               				
                            }
                            break;
                        }
                    }	
                }
            }

            if (playerNames[playerIndex].captureFlag == true)
            {
                playerNames[playerIndex].captureFlag = false;
                playerIndex--;
            }
        }
        for (short playerIndex =0 ;playerIndex < 4 ; playerIndex++)
        {   
            printf("\n>> %s player now has %d/4 pieces on the board and %d/4 pieces on the base.\n",playerNames[playerIndex].playerName,(4-playerNames[playerIndex].basePieceCount),playerNames[playerIndex].basePieceCount);
            for (short pieceIndex = 0 ;pieceIndex < 4 ; pieceIndex++)
            {   
                if (piecesArray[playerIndex][pieceIndex].state !=BASE )
                {
                    printf("\n==========================================\n");
                    break;
                }
            }
            for (short pieceIndex = 0 ;pieceIndex < 4 ; pieceIndex++)
            {   
                if (piecesArray[playerIndex][pieceIndex].state == BOARD )
                {
                    printf(">> Location of %s piece %s is on %d \n" , piecesArray[playerIndex][pieceIndex].playerName,piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].location);
                }
                if (piecesArray[playerIndex][pieceIndex].state == HOME )
                {
                    printf(">> Location of %s piece %s is on Homepath Cell %d \n" , piecesArray[playerIndex][pieceIndex].playerName,piecesArray[playerIndex][pieceIndex].pieceName,piecesArray[playerIndex][pieceIndex].homeStraightLocation);
                }
                if (piecesArray[playerIndex][pieceIndex].state == REMOVED)
                {
                    printf(">> %s %s Piece Has Reached HOME !!! \n",piecesArray[playerIndex][pieceIndex].playerName,piecesArray[playerIndex][pieceIndex].pieceName);
                }
            }
            for (short pieceIndex = 0 ;pieceIndex < 4 ; pieceIndex++)
            {   
                if (piecesArray[playerIndex][pieceIndex].state != BASE)
                {
                    printf("\n==========================================\n");
                    break;
                }
            }
        }		
        for (short playerIndex = 0 ;playerIndex < 4 ; playerIndex++)
        {		
            if (check_All_The_Pieces_Are_In_Home(playerIndex))
            {
                printf(" ;) %s PLAYER WINS !!! \n",playerNames[playerIndex].playerName);
                nextRound =  false;
            }    
        }
    }
}
