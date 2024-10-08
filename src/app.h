#ifndef _APP
#define _APP

#include "io-buf.h"
#include "queue.h"
#include "b-tree.h"

typedef struct app app;

struct app {
    io_buf *in;
    io_buf *out;
    queue *queue;
};

void cli();

app *alloc_app();

void clear_app(app *app);

int main(int argc, char **argv);

#endif
