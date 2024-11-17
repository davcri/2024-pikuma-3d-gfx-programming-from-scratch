#if !defined(SWAP_H)
#define SWAP_H

void swap_int(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void swap_float(float *a, float *b)
{
    float tmp = *a;
    *a = *b;
    *b = tmp;
}
#endif // SWAP_H