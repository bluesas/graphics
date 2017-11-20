#include "svpng.inc"
#include <sys/time.h>
#include <math.h>   // fmaxf(), sinf(), cosf()
#include <stdlib.h> // abs()
#include <string.h> // memset()
#define PI 3.14159265359f
#define W 512
#define H 512

long long int 
get_current_time_stamp()
{
    struct timeval timer_usec;
    long long int timestamp_usec; /* timestamp in microsecond */
    if (!gettimeofday(&timer_usec, NULL))
    {
        timestamp_usec = ((long long int)timer_usec.tv_sec) * 1000000ll +
                         (long long int)timer_usec.tv_usec;
    }
    else
    {
        timestamp_usec = -1;
    }

    return timestamp_usec;
}

static unsigned char img[W * H * 3];

void setpixel(int x, int y)
{
    unsigned char *p = img + (y * W + x) * 3;
    p[0] = p[1] = p[2] = 0;
}

void bresenham(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    while (setpixel(x0, y0), x0 != x1 || y0 != y1)
    {
        int e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

int main()
{
    long long int start = get_current_time_stamp();
    printf("start painting at %lld\n", start);
    memset(img, 255, sizeof(img));
    float cx = W * 0.5f - 0.5f, cy = H * 0.5f - 0.5f;
    for (int j = 0; j < 5; j++)
    {
        float r1 = fminf(W, H) * (j + 0.5f) * 0.085f;
        float r2 = fminf(W, H) * (j + 1.5f) * 0.085f;
        float t = j * PI / 64.0f;
        for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f)
        {
            float ct = cosf(t), st = sinf(t);
            bresenham((int)(cx + r1 * ct), (int)(cy - r1 * st), (int)(cx + r2 * ct), (int)(cy - r2 * st));
        }
    }
    long long int end = get_current_time_stamp();
    printf("end painting at %lld and time cost %lld\n", end, end - start);

    svpng(fopen("line_bresenham.png", "wb"), W, H, img, 0);
}
