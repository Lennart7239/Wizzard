#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROUNDS 15
#define DECK_SIZE 60
#define MAX_PLAYERS 4
#define HAND_SIZE 15


typedef enum {
    RED, GREEN, BLUE, YELLOW, NO_COLOR
} Color;

typedef struct {
    int value;
    Color color;
} Card;

typedef struct {
    char name[50];
    Card hand[HAND_SIZE];
    int score;
    int tricks;
} Player;

Color trump_color;
Card deck[DECK_SIZE];
Player players[MAX_PLAYERS];
int num_players;

const char* get_color_name(Color color) {
    switch (color) {
        case RED: return "RED";
        case GREEN: return "GREEN";
        case BLUE: return "BLUE";
        case YELLOW: return "YELLOW";
        case NO_COLOR: return "NO_COLOR";
        default: return "UNKNOWN_COLOR";
    }
}

void print_hand(int player) {
    printf("%s, das sind deine Karten:\n", players[player].name);
    for (int i = 0; i < HAND_SIZE; i++) {
        if (players[player].hand[i].value != 0) {  // Wir gehen davon aus, dass eine Karte mit dem Wert 0 nicht existiert
            printf("%d: %d of %s\n", i, players[player].hand[i].value, get_color_name(players[player].hand[i].color));
        }
    }
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
        deck[i].value = i % 15 + 1;  // Werte von 1 bis 15
        deck[i].color = i / 15;  // Farben von RED bis YELLOW
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

void deal_cards() {
    int i, j;
    for (i = 0; i < num_players; i++) {
        for (j = 0; j < HAND_SIZE; j++) {
            players[i].hand[j] = deck[i * HAND_SIZE + j];
        }
    }
}

Card trick_cards[MAX_PLAYERS];  // Karten, die in diesem Stich ausgespielt wurden
Color trick_color;  // Farbe des ersten ausgespielten Karte in diesem Stich

void play_card(int player, int round) {
    print_hand(player);  // Zeige die Karten des Spielers an
    printf("Trumpf : %s\n", get_color_name(trump_color));
    printf("%s, welche Karte möchtest du ausspielen?\n", players[player].name);
    int card_index;
    scanf("%d", &card_index);
    Card played_card = players[player].hand[card_index];
    if (trick_color == NO_COLOR) {
        trick_color = played_card.color;
    }
    // Überprüfen, ob die ausgespielte Karte den Regeln entspricht
    TODO: // trick color initialisierung für ersten spieler richtig machen
    if (player != 0 && played_card.color != trick_color && played_card.color != trump_color){
        for (int i = 0; i < round; i++) {
            if (players[player].hand[i].color == trick_color || players[player].hand[i].color == trump_color){
                printf("Du musst die Farbe bedienen, wenn du kannst. Versuche es erneut.\n");
                return play_card(player, round);
            }
        }
    }

    // Karte aus der Hand des Spielers entfernen
    for (int i = card_index; i < round - 1; i++) {
        players[player].hand[i] = players[player].hand[i + 1];
    }

    printf("%s spielt %d of %s\n", players[player].name, played_card.value, get_color_name(played_card.color));

    // Karte zum Stich hinzufügen
    trick_cards[player] = played_card;
    if (player == 0) {
        trick_color = played_card.color;
    }
}

int determine_trick_winner(int round) {
    Card winning_card = trick_cards[0];
    int winning_player = 0;
    for (int i = 1; i < num_players; i++) {
        Card current_card = trick_cards[i];
        if (current_card.color == winning_card.color && current_card.value > winning_card.value) {
            winning_card = current_card;
            winning_player = i;
        } else if (current_card.color == trump_color && winning_card.color != trump_color) {
            winning_card = current_card;
            winning_player = i;
        }
    }
    return winning_player;
}

void play_round(int round) {
    // Jeder Spieler erhält eine Anzahl von Karten entsprechend der Rundenzahl
    for (int i = 0; i < num_players; i++) {
        for (int j = 0; j < round; j++) {
            players[i].hand[j] = deck[i * round + j];
        }
    }


    // Festlegen der Trumpffarbe

    trump_color = deck[0].color;
    // Jeder Spieler macht eine Vorhersage über die Anzahl der Stiche, die er machen wird
    for (int i = 0; i < num_players; i++) {
        print_hand(i);  // Zeige die Karten des Spielers an

        printf("%s, wie viele Stiche denkst du, wirst du machen?\n", players[i].name);
        scanf("%d", &players[i].score);  // Hier verwenden wir die Punktzahl vorübergehend, um die Vorhersage zu speichern
    }

    // Die Spieler spielen ihre Karten in einer festgelegten Reihenfolge aus
    for (int i = 0; i < round; i++) {
        for (int j = 0; j < num_players; j++) {
            play_card(j, round);
        }
        int trick_winner = determine_trick_winner(round);
        printf("%s hat den Stich gewonnen!\n", players[trick_winner].name);
        players[trick_winner].tricks++;
    }

    // Die Punktzahlen der Spieler werden basierend auf den Vorhersagen und den tatsächlichen Stichen aktualisiert
    for (int i = 0; i < num_players; i++) {
        if (players[i].score == players[i].tricks) {
            players[i].score = 20 + 10 * players[i].tricks;
        } else {
            players[i].score = -10 * abs(players[i].score - players[i].tricks);
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
    deal_cards();

    // Spielen Sie das Spiel.
    for (int round = 1; round < ROUNDS; round++) {
        play_round(round);
    }
    int winner = get_highest_score_player();
    printf("Das Spiel ist vorbei. %s hat mit %d Punkten gewonnen!\n", players[winner].name, players[winner].score);


    return 0;
}