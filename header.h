#define SEGMENT_SIZE 6 * sizeof(int)
#define SIZEofBUFF 20

sem_t *sem;

typedef struct {
    int custid;
    char LastName[SIZEofBUFF];
    char FirstName[SIZEofBUFF];
    int balance;
} MyRecord;
