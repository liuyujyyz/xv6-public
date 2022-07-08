#include "types.h"
#include "stat.h"
#include "user.h"


int
main(int argc, char *argv[])
{
    if (argc < 2){
        printf(1, "no sleep duration");
        exit();
    }
    int sleep_duration = atoi(argv[1]);
    sleep(sleep_duration * 100);
    exit();
}