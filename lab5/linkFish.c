#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BIG_BUF 156
#define SMALL_BUF 6

pid_t getpid(void);
int seed;

struct Card {
    char code[3];
    int rank;
    int suit;
    struct Card *next;
};

void initrand(int myid) {
    seed = myid - 1;
}

int nextrand() {
    return (seed++ % 99) + 1;
}

void readDeck(char (*deck)[3]) {
    char inbuf[BIG_BUF];
    int i, k, n;

    // Read < .txt file
    while((n = read(STDIN_FILENO, inbuf, BIG_BUF)) > 0) {
        i = 0, k = 0;
        inbuf[n] = '\0';
        while(i < n && inbuf[i] == ' ') i++;
        while (i < n) {
            deck[k][2] = '\0';
            deck[k][0] = inbuf[i++];
            deck[k][1] = inbuf[i++];
            k++;
            while(i < n && (inbuf[i] == ' ' || inbuf[i] == '\n')) i++;
        }
    }
}

void insertToHand(struct Card **head, char buf[]) {
    struct Card *curr, *newCard = (struct Card *)malloc(sizeof(struct Card));

    strcpy(newCard->code, buf);
    switch (buf[0]) {
        case 'd': case 'D': newCard->suit = 0; break;
        case 'c': case 'C': newCard->suit = 1; break;
        case 'h': case 'H': newCard->suit = 2; break;
        case 's': case 'S': newCard->suit = 3; break;
        default:            break;
    }
    switch (buf[1]) {
        case '0':           exit(EXIT_SUCCESS);
        case '2':           newCard->rank = 0; break;
        case '3':           newCard->rank = 1; break;
        case '4':           newCard->rank = 2; break;
        case '5':           newCard->rank = 3; break;
        case '6':           newCard->rank = 4; break;
        case '7':           newCard->rank = 5; break;
        case '8':           newCard->rank = 6; break;
        case '9':           newCard->rank = 7; break;
        case 't': case 'T': newCard->rank = 8; break;
        case 'j': case 'J': newCard->rank = 9; break;
        case 'q': case 'Q': newCard->rank = 10; break;
        case 'k': case 'K': newCard->rank = 11; break;
        case 'a': case 'A': newCard->rank = 12; break;
        default:            break;
    }
    newCard->next = NULL;
    if(*head == NULL || (*head)->rank < newCard->rank ||
      ((*head)->rank == newCard->rank && (*head)->suit < newCard->rank)) {
        newCard->next = *head;
        *head = newCard;
    }
    else {
        // Still have some issue (suit not sorted correctly)
        curr = *head;
        while(curr->next != NULL && curr->next->rank > newCard->rank) {
            curr = curr->next;
        }
        while(curr->next != NULL &&
          (curr->next->rank == newCard->rank && curr->next->suit > newCard->suit)) {
            curr = curr->next;
        }
        newCard->next = curr->next;
        curr->next = newCard;
    }

    
}

void initHand(int *child, struct Card **head, int num) {
    int i;
    char buf[SMALL_BUF];

    for(i = 0; i < num; i++) {
        read(child[0], buf, SMALL_BUF);
        insertToHand(head, buf);
    }
}

void rdcHand(struct Card **hand, struct Card **reduced) {
    struct Card *curr, *prev, *pair, *temp;

    prev = *hand;
    curr = prev->next;
    while(curr != NULL) {
        if(curr->next != NULL && curr->rank == curr->next->rank) {
            pair = curr;
            curr = curr->next->next;
            pair->next->next = NULL;
            if(*reduced == NULL) {
                *reduced = pair;
            } else {
                temp = *reduced;
                while(temp->next != NULL)
                    temp = temp->next;
                temp->next = pair;
            }
            prev->next = curr;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void printHand(int id, struct Card *hand, char *str) {
    // sprintf for hands
    struct Card *temp;
    char cards[BIG_BUF] = "";

    temp = hand;
    while(temp != NULL) {
        (temp == hand) ? sprintf(cards,"%s", temp->code) :
        sprintf(cards, "%s %s", cards, temp->code);
        temp = temp->next;
    }
    printf("Child %d, pid %d: %s hand <%s> \n", id, getpid(), str, cards);
}

void startGame(const int N_CHILD) {
    int toParent[N_CHILD][2];
    int toChild[N_CHILD][2];
    pid_t shut_down[N_CHILD];
    char cmdBuf[BIG_BUF];
    int pid, i, j, loop = 1;
    int nCard = (N_CHILD < 5) ? 7 : 5;
    
    // Create pipe
    for(i = 0; i < N_CHILD; i++) {
        if(pipe(toParent[i]) < 0 || pipe(toChild[i]) < 0) {
            printf("Pipe creation error\n");
            exit(1);
        }
    }
    // Fork
    for(i = 0; i < N_CHILD; i++) {
        pid = fork();
        if(pid < 0) {
            printf("Error");
            exit(1);
        } 
        else if (pid == 0) { /* child */
            struct Card *hand = NULL;
            struct Card *reduced = NULL;

            // usable pipe-> read: toChild[i][0] write: toParent[i][1]
            for(j = 0; j < N_CHILD; j++){
                close(toChild[j][1]);
                close(toParent[j][0]);
                if(j != i) {
                    close(toChild[j][0]);
                    close(toParent[j][1]);
                }
            }

            initHand(toChild[i], &hand, nCard); // Get initial card
            printHand(i+1, hand, "initial");
            rdcHand(&hand, &reduced);
            printHand(i+1, hand, "reduced");

            while(loop) {
                // Main functions -- need while loop here

                // TODO: checkReduce (3 situations, no need to write a function)
                // 1. initial reduce
                // 2. got card from another player then reduce
                // 3. gofish reduce
                //
                // TODO: Ask card function (Go fish)
                // Check if deck is empty
                // TODO: check hand is empty, which stops the child process
                // First one with most pair wins
                loop = 0;
            }

            // printf("Child %d, pid %d: fin\n", i+1, getpid());

            // Finish child process
            close(toChild[i][0]);
            close(toParent[i][1]);
            exit(0);
        }
        else {
            shut_down[i] = pid;
        }
    }

    if(pid > 0) { /* parent */
        int loop = 1, k = 0;
        char deck[52][3];

        // usable pipe-> read: toParent[i][0] write: toChild[i][1]
        for(i = 0; i < N_CHILD; i++) {
            close(toChild[i][0]);
            close(toParent[i][1]);
        }
        
        printf("Parent: the child players are ");
        for(i = 0; i < N_CHILD; i++)
            printf("%d ", shut_down[i]);
        printf("\n");
        readDeck(deck);

        // Initialize player hand
        for(i = 0; i < nCard; i++)
            for(j = 0; j < N_CHILD; j++)
                write(toChild[j][1], deck[k++], SMALL_BUF);

        // Game loop
        while(loop) {
            // TODO: Add parent control
            // For Debug
            loop = 0;
            // printf("Parent exit loop. PID: %d\n", getpid());
        }

        // Close all pipe at the end
        for(j = 0; j < N_CHILD; j++) {
            close(toChild[j][1]);
            close(toParent[j][0]);
        }
    }

    /* prevent zombie */
    for(i = 0; i < N_CHILD; i++)
        waitpid(shut_down[i], NULL, 0);
}


int main(int argc, char *argv[]) {
    int nChild = atoi(argv[1]);

    if(nChild < 2 || nChild > 8) {
        printf("Player range should be 2 to 8...\n");
        return 0;
    }
    startGame(nChild);
    return 0;
}