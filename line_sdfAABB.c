#include "svpng.inc"
#include <math.h>
#include <string.h> // memset()
#define PI 3.14159265359f
#define W 512
#define H 512
#define N 64

static unsigned char img[W * H * 3];

void alphablend(int x, int y, float alpha, float r, float g, float b) {
    unsigned char* p = img + (y * W + x) * 3;
    p[0] = (unsigned char)(p[0] * (1 - alpha) + r * alpha * 255);
    p[1] = (unsigned char)(p[1] * (1 - alpha) + g * alpha * 255);
    p[2] = (unsigned char)(p[2] * (1 - alpha) + b * alpha * 255);
}

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

void lineSDFAABB(float ax, float ay, float bx, float by, float r) {
    int x0 = (int)floorf(fminf(ax, bx) - r);
    int x1 = (int) ceilf(fmaxf(ax, bx) + r);
    int y0 = (int)floorf(fminf(ay, by) - r);
    int y1 = (int) ceilf(fmaxf(ay, by) + r);
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++)
            alphablend(x, y, fmaxf(fminf(0.5f - capsuleSDF(x, y, ax, ay, bx, by, r), 1.0f), 0.0f), 0.0f, 0.0f, 0.0f);
}

int main()
{
    unsigned char *p = img;
    memset(img, 255, sizeof(img));

    float cx = W * 0.5f, cy = H * 0.5f;
    for (int j = 0; j < 5; j++) {
        float r1 = fminf(W, H) * (j + 0.5f) * 0.085f;
        float r2 = fminf(W, H) * (j + 1.5f) * 0.085f;
        float t = j * PI / 64.0f, r = (j + 1) * 0.5f;
        for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f) {
            float ct = cosf(t), st = sinf(t);
            lineSDFAABB(cx + r1 * ct, cy - r1 * st, cx + r2 * ct, cy - r2 * st, r);
        }
    }
    svpng(fopen("line_sdfAABB.png", "wb"), W, H, img, 0);
}