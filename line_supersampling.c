#include "svpng.inc"
#include <math.h>
#include <string.h> // memset()
#define PI 3.14159265359f
#define W 512
#define H 512
#define N 64

static unsigned char img[W * H * 3];

int capsule(float px, float py, float ax, float ay, float bx, float by, float r)
{
    float vx = px - ax;
    float vy = py - ay;
    float ux = bx - ax;
    float uy = by - ay;

    float t = fmaxf(fminf((vx * ux + vy * uy) / (ux * ux + uy * uy), 1.0f), 0.0f);

    float dx = vx - t * ux;
    float dy = vy - t * uy;

    return (dx * dx + dy * dy) < (r * r);
}

float sample(float x, float y)
{
    float s = 0.0f;

    float cx = W * 0.5f;
    float cy = H * 0.5f;

    for (int j = 0; j < 5; j++)
    {
        float r1 = fminf(W, H) * (j + 0.5) * 0.085f;
        float r2 = fminf(W, H) * (j + 1.5) * 0.085f;

        float t = j * PI / N;
        float r = (j + 1) * 0.5f;
        for (int i = 1; i <= N; i++, t += 2.0f * PI / N)
        {
            float cost = cosf(t);
            float sint = sinf(t);

            s = fmaxf(s, (capsule(x, y, cx + cost * r1, cy - sint * r1, cx + cost * r2, cy - sint * r2, r) ? 1.0f : 0.0f));
        }
    }

    return s;
}

int main()
{
    unsigned char *p = img;
    // memset(img, 255, sizeof(img));

    int sh = 5;
    int sw = 5;
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++, p += 3)
        {
            float sum = 0;
            for (int j = -sh / 2; j <= sh / 2; j++)
            {
                for (int i = -sw / 2; i <= sw / 2; i++)
                {
                    sum += sample(x + (float) i / sw, y + (float) j / sh);
                }
            }
            p[0] = p[1] = p[2] = (unsigned char)((1 - sum / (sh * sw)) * 255);
        }
    }

    svpng(fopen("line_supersampling.png", "wb"), W, H, img, 0);
}