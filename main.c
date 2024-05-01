#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROUNDS 15
#define DECK_SIZE 60
#define PLAYERS 4
#define MAX_HAND_SIZE 15


typedef enum {
    RED, GREEN, BLUE, YELLOW, WIZZARD, JESTER
} Color;


typedef struct {
    int value;
    Color color;
} Card;

typedef struct {
    int called_stiche;
    int gewonnene_runden;
    int gewonnene_stiche;
    char name[50];
    Card hand[MAX_HAND_SIZE];
    int score;
} Player;

Color trump_color;
Card deck[DECK_SIZE];
Player players[PLAYERS];
int num_players;
int current_round = 0;

const char* get_color_name(Color color) {
    switch (color) {
        case RED: return "RED";
        case GREEN: return "GREEN";
        case BLUE: return "BLUE";
        case YELLOW: return "YELLOW";
        case WIZZARD: return "WIZZARD";
        case JESTER: return "JESTER";
        default: return "UNKNOWN_COLOR";
    }
}


void print_hand(int player) {
    printf("%s, das sind deine Karten:\n", players[player].name);
    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        if (players[player].hand[i].value != 0) {  // Wir gehen davon aus, dass eine Karte mit dem Wert 0 nicht existiert
            printf("%d: %d of %s\n" ,i, players[player].hand[i].value, get_color_name(players[player].hand[i].color));
        }
    }
    printf("Trumpf : %s\n", get_color_name(trump_color));
}

void initialize_players() {
    num_players = 4;
    for (int i = 0; i < num_players; i++) {
        sprintf(players[i].name, "Player %d", i + 1);  // Namen setzen
        players[i].score = 0;  // Punktzahl initialisieren
    }
}

void initialize_deck() {
    int i;
    for (i = 0; i < DECK_SIZE; i++) {
        deck[i].value = i % 13 + 1;  // Werte von 1 bis 13
        deck[i].color = i / 13;  // Farben von RED bis YELLOW
    }
}

void shuffle_deck() {
    srand(time(NULL));
    int i;
    for (i = 0; i < DECK_SIZE; i++) {
        int j = rand() % DECK_SIZE;
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void deal_cards(int round_hand_size) {
    int i;
    for (i = 0; i < round_hand_size; i += 4) {
        for (int k = 0; k < num_players; ++k) {
            players[k].hand[i] = deck[i+k];
            }
        }
    for (int j = i; j < 15; ++j) {
        for (int k = 0; k < num_players; ++k) {
            players[k].hand[j].color = (Color) NULL;
        }
    }
}


int determine_single_round_winner(Card played_Cards[]) {
    Card trick = played_Cards[0];
    int winning_player = 0;
    for (int i = 0; i < num_players; i++) {
        Card current_card = played_Cards[i];
        if (current_card.color == trick.color && current_card.value > trick.value) {
            trick = current_card;
            winning_player = i;
        } else if (current_card.color == trump_color && trick.color != trump_color) {
            trick = current_card;
            winning_player = i;
        }
    }
    return winning_player;
}


int play_cards_ein_stich(int start_spieler) {
    // Karten, die in diesem Stich ausgespielt wurden
    Card played_Cards[PLAYERS];  // Karten, die in diesem Stich ausgespielt wurden
    Color trick_Color = (Color) NULL;
    int offset = start_spieler;
    for(int x = 0; x < num_players; x++){
        print_hand(x+offset%4);  // Zeige die Karten des Spielers an
        setbuf(stdout, NULL);
        printf("Trick : %s \n", get_color_name(trick_Color));
        printf("%s, welche Karte möchtest du ausspielen?\n", players[x+offset%4].name);
        int card_index;
        scanf("%d", &card_index);
        Card played_card = players[x+offset%4].hand[card_index];
        if (trick_Color == (Color)NULL && played_card.color != JESTER && played_card.color != WIZZARD) {
            trick_Color = played_card.color;
        }
        // Überprüfen, ob die ausgespielte Karte den Regeln entspricht
        if (played_card.color != trick_Color && played_card.color != trump_color && played_card.color != JESTER && played_card.color != WIZZARD) {
            for (int i = 0; i < 14; i++) {
                if (players[x+offset%4].hand[i].color == trick_Color || players[x+offset%4].hand[i].color == trump_color){
                    printf("Du musst die Farbe bedienen, wenn du kannst. Versuche es erneut.\n");
                    return play_cards_ein_stich(start_spieler);
                }
            }
        }


        // Karte aus der Hand des Spielers entfernen
        for (int i = card_index; i < 15; i++) {
            players[x+offset%4].hand[i] = players[x+offset%4].hand[i + 1];
        }

        printf("%s spielt %d of %s\n________________________________________________\n", players[x+offset%4].name, played_card.value, get_color_name(played_card.color));

        // Karte zum Stich hinzufügen
        played_Cards[x+offset%4] = played_card;
    }
    return determine_single_round_winner(played_Cards);
}

int max(int player1, int player2, int player3, int player4){
    if(player1 > player2 && player1 > player3 && player1 > player4){
        return 0;
    } else if(player2 > player1 && player2 > player3 && player2 > player4){
        return 1;
    } else if(player3 > player1 && player3 > player2 && player3 > player4){
        return 2;
    } else {
        return 3;
    }
}

void play_round(int round, int start_spieler_runde){
    // Jeder Spieler erhält eine Anzahl von Karten entsprechend der Rundenzahl
    deal_cards(round);
    int called_stiche = 0;

    trump_color = deck[round*num_players+1].color;
    // Jeder Spieler macht eine Vorhersage über die Anzahl der Stiche, die er machen wird
    for (int i = 0; i < num_players; i++) {
        nochmal:
        print_hand(i);  // Zeige die Karten des Spielers an
        setbuf(stdout, NULL);
        printf("%s, wie viele Stiche denkst du, wirst du machen?\n", players[i].name);
        scanf("%d", &players[i].called_stiche);
        //Anzahl der gecallten Stiche darf nicht gleich anzahl der Runden sein
        if(i == num_players-1 && called_stiche+players[i].called_stiche == round){
            printf("Die Summe der Vorhersagen ist gleich der Anzahl der Stiche, die in dieser Runde gemacht werden. Bitte korrigiere deine Vorhersage.\n");
            goto nochmal;
        }
        called_stiche += players[i].called_stiche;
        printf("________________________________________________\n");
    }
    // Die Spieler spielen ihre Karten in einer festgelegten Reihenfolge aus

    int start_spieler_stich = start_spieler_runde;
    for (int i = 0; i < round; i++) {
            start_spieler_stich = play_cards_ein_stich(start_spieler_stich);
            players[start_spieler_stich].gewonnene_stiche +=1;
        int winner = max(players[0].gewonnene_stiche, players[1].gewonnene_stiche, players[2].gewonnene_stiche, players[3].gewonnene_stiche);
        printf("%s hat den Stich gewonnen!\n________________________________________________\n", players[winner].name);
    }
    // Die Punktzahlen der Spieler werden basierend auf den Vorhersagen und den tatsächlichen Stichen aktualisiert
    for (int i = 0; i < num_players; i++) {
        if (players[i].gewonnene_stiche == players[i].called_stiche) {
            players[i].gewonnene_stiche = 20 + 10 * players[i].called_stiche;
        } else {
            players[i].gewonnene_stiche = players[i].score -(10 * abs(players[i].gewonnene_stiche - players[i].called_stiche));
        }
    }
}


int get_highest_score_player() {
    int max_score = 0;
    int max_score_player = 0;
    for (int i = 0; i < num_players; i++) {
        if (players[i].score > max_score) {
            max_score = players[i].score;
            max_score_player = i;
        }
    }
    return max_score_player;
}

int main() {
    // Initialisieren und mischen Sie das Deck.
    initialize_deck();
    shuffle_deck();

    // Initialisieren Sie die Spieler.
    initialize_players();

    // Teilen Sie die Karten aus.

    // Spielen Sie das Spiel.
    int start_spieler_runde = 0;

    for (int round = 1; round < ROUNDS; round++) {
        deal_cards(round);
        play_round(round, start_spieler_runde);
        start_spieler_runde = (start_spieler_runde + 1) % num_players;
    }
    int winner = get_highest_score_player();
    printf("Das Spiel ist vorbei. %s hat mit %d Punkten gewonnen!\n", players[winner].name, players[winner].score);

    return 0;
}