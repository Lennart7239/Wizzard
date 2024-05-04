#include "constants.h"


const char *get_color_name(Color color)
{
    switch (color)
    {
        case RED:return "Rot";
        case GREEN:return "Gruen";
        case BLUE:return "Blau";
        case YELLOW:return "Gelb";
        default:return "UNKNOWN_COLOR";
    }
}


void print_card(Card card)
{
    switch (card.value)
    {
        case 14:printf("Wizzard\n");
            break;
        case 0:printf("Narre\n");
            break;
        case -1:break;
        default:printf("%se %d\n", get_color_name(card.color), card.value);
            break;
    }
}


void print_stich(Card stich[PLAYERS])
{
    printf("Aktuell liegende Karten:\n\n");
    printf("Trumpf:    ");
    print_card(trump);
    printf("\n");
    for (int i = 0; i < PLAYERS; i++)
    {
        printf("\t   ");
        print_card(stich[i]);
    }
}


void print_hand(int player)
{
    printf("\n%s Karten:\n\n", players[player].name);
    for (int i = 0; i < MAX_HAND_SIZE; i++)
    {
        if (players[player].hand[i].value < 15 && players[player].hand[i].value >= 0)
        {  // Wir gehen davon aus, dass eine Karte mit dem Wert 0 nicht existiert
            printf("\t%d: ", i + 1);
            print_card(players[player].hand[i]);
        }
    }
    printf("\n");
}


Outputdata outputToBot(int player)
{
    Outputdata output;
    //Hand übergeben
    for (int i = 0; i < MAX_HAND_SIZE; i++)
    {
        output.hand[i] = players[player].hand[i];
    }
    //TODO: OutputData vervollständigen
    return output;
}


int isValidInput() {
    int input;
    nochmal:
    if(scanf("%d", &input) == 1)
    {
        return input;
    }
    else {
        while(getchar() != '\n');
        printf("Falsche eingabe\n");
        goto nochmal;
    }
}


int getInput(int modus, char name[50])
{
    if (modus == 1)
    {    //stiche callen
        int input;
        nochmal:
        printf("%s wie viele Stiche denkst du, wirst du machen?\n", name);
        input = isValidInput();
        if (input < 0 || input > current_round)
        {
            printf("Falsche Eingabe. Bitte gib eine Zahl zwischen 0 und %d ein.\n", current_round);
            goto nochmal;
        }
        return input;
    }
    else if (modus == 2)
    {    //Karte legen
        printf("\n%s, welche Karte willst du ausspielen?\n", name);
        int card_index;
        card_index = isValidInput();
        card_index -= 1;
        if (card_index < 0 || card_index > current_round)
        {
            printf("\nWird nicht akzeptiert. Versuche es erneut.\n");
            goto nochmal;
        }
        return card_index;
    }
    return 0;
}


void initialize_players()
{
    for (int i = 0; i < PLAYERS; i++)
    {
        sprintf(players[i].name, "Player %d", i + 1);  // Namen setzen
        players[i].score = 0;  // Punktzahl initialisieren
    }
}


void initialize_deck() {
    int i;
    for (i = 0; i < DECK_SIZE; i++) {
        deck[i].value = i % 15;  // Werte von 1 bis 13
        deck[i].color = i%4;  // Farben von RED bis YELLOW
    }
}


void shuffle_deck()
{
    srand(time(NULL));
    int i;
    for (i = 0; i < DECK_SIZE; i++)
    {
        int j = rand() % DECK_SIZE;
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}


void deal_cards(int round_hand_size)
{
    int i;
    for (i = 0; i < round_hand_size; i++)
    {
        for (int k = 0; k < PLAYERS; ++k)
        {
            players[k].hand[i] = deck[PLAYERS * i + k];
        }
    }
    for (int j = i; j < 15; ++j)
    {
        for (int k = 0; k < PLAYERS; ++k)
        {
            players[k].hand[j].color = NILL;
            players[k].hand[j].value = -1;
        }
    }
}


void clean_hand(int player, int card_index)
{
    // Karte aus der Hand des Spielers entfernen und alle anderen Karten aufrücken
    for (int i = card_index; i < MAX_HAND_SIZE - 1; i++)
    {
        players[player].hand[i] = players[player].hand[i + 1];
    }
    // Letzte Karte in der Hand auf leer setzen
    players[player].hand[MAX_HAND_SIZE - 1].value = -1;
    players[player].hand[MAX_HAND_SIZE - 1].color = NILL;
}


int determine_single_round_winner(Card played_Cards[PLAYERS])
{
    Card temp_trick = played_Cards[0];
    int winning_player = 0;
    for (int i = 0; i < PLAYERS; i++)
    {
        Card current_card = played_Cards[i];
        if ((current_card.color == temp_trick.color || current_card.color == trump.color) && current_card.value > temp_trick.value)
        {
            temp_trick = current_card;
            winning_player = i;
        }
    }
    return winning_player;
}


int play_stich(int start_spieler)
{
    Card played_Cards[PLAYERS];  // Karten, die in diesem Stich ausgespielt wurden
    for (int i = 0; i < PLAYERS; i++)
    {   //Hand jedes Spielers leeren
        played_Cards[i].value = -1;
        played_Cards[i].color = NILL;
    }
    //default werte für Trick
    trick.value = -1;
    trick.color = NILL;
    for (int x = 0; x < PLAYERS; x++)
    {   //Jeder Spieler spielt eine Karte
        int offset = (x + start_spieler) % PLAYERS; //spieler unter berücksichtigung von verschiebung des startspielers
        nochmal:  //rückkehrpunkt, falls falsche eingabe
        print_stich(played_Cards);  //Liegende Karten ausgeben
        print_hand(offset);  // Zeige die Karten des Spielers an
        int card_index = getInput(2, players[offset].name); //input von nutzer bekommen
        Card played_card = players[offset].hand[card_index];    //Gespielte Karte herausfinden
        //trick setzen, falls er noch nicht existiert
        if (trick.value == -1 && played_card.value != 14 && played_card.value != 0)
        {
            trick.color = played_card.color;
            trick.value = played_card.value;
        }
        // Überprüfen, ob die ausgespielte Karte den Regeln entspricht
        if (played_card.color != trick.color && played_card.color != trump.color && played_card.value != 14 &&
            played_card.value != 0)
        {
            for (int i = 0; i < current_round; i++)
            {
                if (players[offset].hand[i].color == trick.color || players[offset].hand[i].color == trump.color)
                {
                    if (players[offset].hand[i].value < 14 && players[offset].hand[i].value > 1)
                    {
                        printf("\nDu musst die Farbe bedienen, wenn du kannst. Versuche es erneut.\n");
                        goto nochmal;
                    }
                }
            }
        }
        // Karte aus der Hand des Spielers entfernen
        clean_hand(offset, card_index);

        printf("%s spielt ", players[offset].name);
        print_card(played_card);
        printf("\n________________________________________________\n");
        // Karte zum Stich hinzufügen
        played_Cards[x] = played_card;
    }
    return (determine_single_round_winner(played_Cards) + start_spieler) % PLAYERS;
}


void play_round(int round, int start_spieler_runde)
{
    printf("Neue Runde\n________________________________________________\n________________________________________________\n");
    // Jeder Spieler erhält eine Anzahl von Karten entsprechend der Rundenzahl
    deal_cards(round);
    int called_stiche = 0;
    trump = deck[round * PLAYERS + 1];
    // Jeder Spieler macht eine Vorhersage über die Anzahl der Stiche, die er machen wird
    for (int i = 0; i < PLAYERS; i++)
    {
        nochmal:
        int offset = (i + start_spieler_runde) % PLAYERS;
        printf("Trumpf:    ");
        print_card(trump);
        print_hand(offset);  // Zeige die Karten des Spielers an
        players[offset].called_stiche = getInput(1, players[offset].name);
        //Anzahl der gecallten Stiche darf nicht gleich anzahl der Runden sein
        if (i == PLAYERS - 1 && called_stiche + players[offset].called_stiche == round)
        {
            printf("Die Summe der Vorhersagen ist gleich der Anzahl der Stiche, die in dieser Runde gemacht werden. Bitte korrigiere deine Vorhersage.\n");
            goto nochmal;
        }
        called_stiche += players[offset].called_stiche;
        printf("________________________________________________\n");
    }
    // Die Spieler spielen ihre Karten in einer festgelegten Reihenfolge aus

    int start_spieler_stich = start_spieler_runde;
    for (int i = 0; i < round; i++)
    {
        start_spieler_stich = play_stich(start_spieler_stich);
        players[start_spieler_stich].gewonnene_stiche += 1;
        int winner = start_spieler_stich;
        printf("%s hat den Stich gewonnen!\n________________________________________________\n", players[winner].name);
    }
    // Die Punktzahlen der Spieler werden basierend auf den Vorhersagen und den tatsächlichen Stichen aktualisiert
    for (int i = 0; i < PLAYERS; i++)
    {
        if (players[i].gewonnene_stiche == players[i].called_stiche)
        {
            players[i].score = 20 + 10 * players[i].called_stiche;
        }
        else
        {
            players[i].score = players[i].score - (10 * abs(players[i].gewonnene_stiche - players[i].called_stiche));
        }
    }
}


int compare_scores(const void *a, const void *b)
{
    Player *playerA = (Player *) a;
    Player *playerB = (Player *) b;
    return playerB->score - playerA->score;
}


void sort_players_by_score()
{
    qsort(players, PLAYERS, sizeof(Player), compare_scores);
}


void print_scoreboard()
{
    sort_players_by_score();
    printf("Das Spiel ist vorbei! Das Endergebnis ist:\n");
    for (int i = 0; i < PLAYERS; i++)
    {
        printf("%s: %d\n", players[i].name, players[i].score);
    }
}

int main()
{
    setbuf(stdout, NULL);
    // Deck erstellen und mischen
    initialize_deck();
    shuffle_deck();
    //erstellen der Spieler
    initialize_players();
    // Spiel starten
    int start_spieler_runde = 0;
    for (int round = 1; round <= ROUNDS; round++)
    {
        current_round = round; //current_round setzen
        shuffle_deck();
        deal_cards(round);
        play_round(round, start_spieler_runde);
        start_spieler_runde = (start_spieler_runde + 1) % PLAYERS;  //Nächsten Spieler als Startspieler setzen
    }
    print_scoreboard();
    return 0;
}