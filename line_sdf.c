#include "svpng.inc"
#include <math.h>
#include <string.h> // memset()
#define PI 3.14159265359f
#define W 512
#define H 512
#define N 64

static unsigned char img[W * H * 3];

float capsuleSDF(float px, float py, float ax, float ay, float bx, float by, float r)
{
    float vx = px - ax;
    float vy = py - ay;
    float ux = bx - ax;
    float uy = by - ay;

    float t = fmaxf(fminf((vx * ux + vy * uy) / (ux * ux + uy * uy), 1.0f), 0.0f);

    float dx = vx - t * ux;
    float dy = vy - t * uy;

    return sqrtf(dx * dx + dy * dy) - r;
}

float sample(float x, float y) {
    float s = 0.0f, cx = W * 0.5f, cy = H * 0.5f;
    for (int j = 0; j < 5; j++) {
        float r1 = fminf(W, H) * (j + 0.5f) * 0.085f;
        float r2 = fminf(W, H) * (j + 1.5f) * 0.085f;
        float t = j * PI / 64.0f, r = (j + 1) * 0.5f;
        for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f) {
            float ct = cosf(t), st = sinf(t);
            s = fmaxf(s, fminf(0.5f - capsuleSDF(x, y, cx + r1 * ct, cy - r1 * st, cx + r2 * ct, cy - r2 * st, r), 1.0f));
        }
    }
    return s;
}

int main()
{
    unsigned char *p = img;
    memset(img, 255, sizeof(img));
    for (int y = 0; y < H; y++)
    {
        for (int x = 0; x < W; x++, p += 3)
        {
            p[0] = p[1] = p[2] = (unsigned char)((1 - sample(x, y)) * 255);
        }
    }

    svpng(fopen("line_sdf.png", "wb"), W, H, img, 0);
}