#ifndef SOLC_H
#define SOLC_H
typedef struct solc_t solc_t;
solc_t *solc_init(void); 
void solc_bg(solc_t *self);
#endif

