#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ROUNDS 10
#define DECK_SIZE 60
#define PLAYERS 4
#define MAX_HAND_SIZE 15


typedef enum
{
    HUMAN __attribute__((unused)), AI __attribute__((unused))       // 0 = Mensch, 1 = KI
} Mensch_AI;

typedef enum
{
    RED, GREEN, BLUE, YELLOW, NILL
} Color;

typedef struct
{
    int value;
    Color color;
} Card;

typedef struct
{
    int called_stiche;
    int gewonnene_runden;
    int gewonnene_stiche;
    char name[50];
    Card hand[MAX_HAND_SIZE];
    int score;
    int is_human;
} Player;


typedef struct
{
    int called_stiche[PLAYERS];
    int playernumber;
    Card liegendes_deck[PLAYERS];
    Card trick;
    Color trumpf;
    Card hand[MAX_HAND_SIZE];
    int scores[PLAYERS];
    int stiche[PLAYERS];
} Outputdata;


Card trick;
Card trump;
Card deck[DECK_SIZE];
Player players[PLAYERS];
int current_round;