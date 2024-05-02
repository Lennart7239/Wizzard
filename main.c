#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROUNDS 10
#define DECK_SIZE 60
#define PLAYERS 2
#define MAX_HAND_SIZE 15



typedef enum {
    RED, GREEN, BLUE, YELLOW
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

Card trick;
Color trump_color;
Card deck[DECK_SIZE];
Player players[PLAYERS];

int current_round = 0;

const char* get_color_name(Color color) {
    switch (color) {
        case RED: return "RED";
        case GREEN: return "GREEN";
        case BLUE: return "BLUE";
        case YELLOW: return "YELLOW";
        default: return "UNKNOWN_COLOR";
    }
}

void print_cards(Card cards[], int i) {
        switch (cards[i].value) {
            case 14:
                printf("\tJoker\n", i);
                break;
            case 0:
                printf("\tWizzard\n", i);
                break;
            case -1:
                break;
            default:
                printf("\t\t%d of %s\n", cards[i].value, get_color_name(cards[i].color));
                break;
    }
}
void print_deck(Card deck[PLAYERS]) {
    printf("Aktuell liegende Karten:\n\n");
    printf("\tTrumpf\t:%s\n", get_color_name(trump_color));
    if(trick.value != 0) {
        printf("\tTrick\t:%s\n", get_color_name(trick.color));
    }
    for (int i = 1; i < PLAYERS; i++) {
        print_cards(deck, i);
    }
}

void print_hand(int player) {
    printf("\n%s Karten:\n\n", players[player].name);
    for (int i = 0; i < MAX_HAND_SIZE; i++) {
        if(players[player].hand[i].value < 15 && players[player].hand[i].value > 0) {  // Wir gehen davon aus, dass eine Karte mit dem Wert 0 nicht existiert
            printf("\t%d: %d of %s\n" ,i, players[player].hand[i].value, get_color_name(players[player].hand[i].color));
        }
    }

}

void initialize_players() {
    for (int i = 0; i < PLAYERS; i++) {
        sprintf(players[i].name, "Player %d", i + 1);  // Namen setzen
        players[i].score = 0;  // Punktzahl initialisieren
    }
}

void initialize_deck() {
    int i;
    for (i = 0; i < DECK_SIZE; i++) {
        deck[i].value = i % 13 + 1;  // Werte von 1 bis 13
        deck[i].color = i/15;  // Farben von RED bis YELLOW
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
    for (i = 0; i < round_hand_size; i++) {
        for (int k = 0; k < PLAYERS; ++k) {
            players[k].hand[i] = deck[PLAYERS*i+k];
            }
        }
    for (int j = i; j < 15; ++j) {
        for (int k = 0; k < PLAYERS; ++k) {
            players[k].hand[j].color = (Color) NULL;
            players[k].hand[j].value = 0;
        }
    }
}

void clean_hand(int player, int card_index) {
    // Karte aus der Hand des Spielers entfernen und alle anderen Karten aufrücken
    for (int i = card_index; i < MAX_HAND_SIZE - 1; i++) {
        players[player].hand[i] = players[player].hand[i + 1];
    }

    // Letzte Karte in der Hand auf leer setzen
    players[player].hand[MAX_HAND_SIZE - 1].value = 0;
    players[player].hand[MAX_HAND_SIZE - 1].color = RED;
}


int determine_single_round_winner(Card played_Cards[PLAYERS]){
    Card trick = played_Cards[0];
    int winning_player = 0;
    for (int i = 0; i < PLAYERS; i++) {
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
    Card played_Cards[PLAYERS];  // Karten, die in diesem Stich ausgespielt wurden
    for(int i = 0; i < PLAYERS; i++){
        played_Cards[i].value = -1;
        played_Cards[i].color = (Color) NULL;
    }
    trick.value = 0;
    trick.color = (Color) NULL;
    for(int x = 0; x < PLAYERS; x++){
        int offset = (x+start_spieler)%PLAYERS;
        nochmal:
        print_deck(played_Cards);
        print_hand(offset);  // Zeige die Karten des Spielers an
        setbuf(stdout, NULL);

        printf("\n%s, welche Karte willst du ausspielen?\n", players[offset].name);
        int card_index;
        scanf("%d", &card_index);
        if(card_index < 0 || card_index >= current_round){
            printf("\nWird nicht akzeptiert. Versuche es erneut.\n");
            goto nochmal;
        }
        Card played_card = players[offset].hand[card_index];
        if (trick.value == 0 && played_card.value != 14 && played_card.value != 0) {
            trick.color = played_card.color;
            trick.value = played_card.value;
        }
        // Überprüfen, ob die ausgespielte Karte den Regeln entspricht
        if (played_card.color != trick.color && played_card.color != trump_color && played_card.value != 14 && played_card.value != 0) {
            for (int i = 0; i < current_round; i++) {
                if (players[offset].hand[i].color == trick.color || players[offset].hand[i].color == trump_color){
                    if(players[offset].hand[i].value < 14 && players[offset].hand[i].value > 1) {
                        printf("\nDu musst die Farbe bedienen, wenn du kannst. Versuche es erneut.\n");
                        goto nochmal;
                    }
                }
            }
        }
        // Karte aus der Hand des Spielers entfernen
        clean_hand(offset,card_index);

        printf("%s spielt %d of %s\n________________________________________________\n", players[offset].name, played_card.value, get_color_name(played_card.color));

        // Karte zum Stich hinzufügen
        played_Cards[x] = played_card;
    }

return (determine_single_round_winner(played_Cards)+start_spieler)%PLAYERS;
}

int max(Player players[PLAYERS]){
    int max = 0;
    for (int i = 0; i < PLAYERS; i++) {
        if (players[i].gewonnene_stiche > max) {
            max = players[i].gewonnene_stiche;
        }
    }
    return max;
}

void play_round(int round, int start_spieler_runde){
    printf("Neue Runde\n________________________________________________\n________________________________________________\n");
    // Jeder Spieler erhält eine Anzahl von Karten entsprechend der Rundenzahl
    deal_cards(round);
    int called_stiche = 0;

    trump_color = deck[round*PLAYERS+1].color;
    // Jeder Spieler macht eine Vorhersage über die Anzahl der Stiche, die er machen wird
    for (int i = 0; i < PLAYERS; i++) {
        nochmal:
        int offset = (i+start_spieler_runde)%PLAYERS;
        print_hand(offset);  // Zeige die Karten des Spielers an
        printf("Trumpf: %s\n", get_color_name(trump_color));
        setbuf(stdout, NULL);
        printf("%s, wie viele Stiche denkst du, wirst du machen?\n", players[offset].name);
        scanf("%d", &players[offset].called_stiche);
        //Anzahl der gecallten Stiche darf nicht gleich anzahl der Runden sein
        if(i == PLAYERS-1 && called_stiche+players[offset].called_stiche == round){
            printf("Die Summe der Vorhersagen ist gleich der Anzahl der Stiche, die in dieser Runde gemacht werden. Bitte korrigiere deine Vorhersage.\n");
            goto nochmal;
        }
        called_stiche += players[offset].called_stiche;
        printf("________________________________________________\n");
    }
    // Die Spieler spielen ihre Karten in einer festgelegten Reihenfolge aus

    int start_spieler_stich = start_spieler_runde;
    for (int i = 0; i < round; i++) {
            start_spieler_stich = play_cards_ein_stich(start_spieler_stich);
            players[start_spieler_stich].gewonnene_stiche +=1;
        int winner = start_spieler_stich;
        printf("%s hat den Stich gewonnen!\n________________________________________________\n", players[winner].name);
    }
    // Die Punktzahlen der Spieler werden basierend auf den Vorhersagen und den tatsächlichen Stichen aktualisiert
    for (int i = 0; i < PLAYERS; i++) {
        if (players[i].gewonnene_stiche == players[i].called_stiche) {
            players[i].score = 20 + 10 * players[i].called_stiche;
        } else {
            players[i].score = players[i].score -(10 * abs(players[i].gewonnene_stiche - players[i].called_stiche));
        }
    }
}

int compare_scores(const void *a, const void *b) {
    Player *playerA = (Player *)a;
    Player *playerB = (Player *)b;
    return playerB->score - playerA->score;
}

void sort_players_by_score() {
    qsort(players, PLAYERS, sizeof(Player), compare_scores);
}

void print_scoreboard() {
    sort_players_by_score();
    printf("Das Spiel ist vorbei! Das Endergebnis ist:\n");
    for (int i = 0; i < PLAYERS; i++) {
        printf("%s: %d\n", players[i].name, players[i].score);
    }
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

    for (int round = 1; round <= ROUNDS; round++) {
        current_round = round;
        shuffle_deck();
        deal_cards(round);
        play_round(round, start_spieler_runde);
        start_spieler_runde = (start_spieler_runde + 1) % PLAYERS;
    }
    print_scoreboard();

    return 0;
}