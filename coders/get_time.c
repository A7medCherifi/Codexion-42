#include "codexion.h"

long get_time() {
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// int main()
// {
//     struct timespec ts;
//     clock_gettime(CLOCK_REALTIME, &ts);

//     printf("%ld\n", ts.tv_nsec);
//     ts.tv_nsec += 100 * 1000000;
//     if (ts.tv_nsec >= 10000000000) {
//         ts.tv_sec += ts.tv_nsec / 10000000000;
//         ts.tv_nsec %= 10000000000;
//     }
//     printf("%ld\n", ts.tv_nsec);
//     return (0);
// }