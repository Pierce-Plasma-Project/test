/*
 * Exhaustive certifiers for the computational leaves of R(4,4) = 18.
 * No SAT solver involved: everything here is direct enumeration, so it is an
 * independent check on the SAT pipeline (and vice versa).
 *
 * Certifies:
 *   [A] R(3,3) > 5 : an explicit 2-coloring of K5 with no mono triangle.
 *   [B] R(3,3) <= 6: ALL 2^15 colorings of K6 contain a mono triangle.
 *   [C] R(3,4) > 8 : exhaustive sweep of ALL 2^28 colorings of K8, counting
 *                    those with no red K3 and no blue K4 (witnesses exist);
 *                    also verifies the classical cyclic witness C8(1,4).
 *   [D] R(4,4) > 17: the Paley graph on Z_17 (edge red iff difference is a
 *                    quadratic residue) has no monochromatic K4 — checked
 *                    over all C(17,4) = 2380 four-subsets.
 *
 * Edge indexing for K_n: edge {i,j}, i<j  ->  i*(2n-i-1)/2 + (j-i-1).
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static int eidx(int n, int i, int j)
{
    if (i > j) { int t = i; i = j; j = t; }
    return i * (2 * n - i - 1) / 2 + (j - i - 1);
}

/* ---------- [A] + [B]: R(3,3) ---------- */

static void r33(void)
{
    /* [A] pentagon witness on K5: red = 5-cycle, blue = complement */
    int red5[5][2] = {{0,1},{1,2},{2,3},{3,4},{4,0}};
    uint32_t g5 = 0;
    for (int k = 0; k < 5; k++) g5 |= 1u << eidx(5, red5[k][0], red5[k][1]);
    int bad = 0;
    for (int a = 0; a < 5; a++) for (int b = a+1; b < 5; b++)
        for (int c = b+1; c < 5; c++) {
            uint32_t t = (1u<<eidx(5,a,b)) | (1u<<eidx(5,a,c)) | (1u<<eidx(5,b,c));
            if ((g5 & t) == t || (g5 & t) == 0) bad++;
        }
    printf("[A] K5 pentagon coloring: monochromatic triangles = %d  "
           "=> R(3,3) > 5  %s\n", bad, bad == 0 ? "OK" : "FAIL");

    /* [B] all 2^15 colorings of K6 */
    uint32_t tri[20]; int nt = 0;
    for (int a = 0; a < 6; a++) for (int b = a+1; b < 6; b++)
        for (int c = b+1; c < 6; c++)
            tri[nt++] = (1u<<eidx(6,a,b)) | (1u<<eidx(6,a,c)) | (1u<<eidx(6,b,c));
    long survivors = 0;
    for (uint32_t g = 0; g < (1u << 15); g++) {
        int mono = 0;
        for (int k = 0; k < nt; k++) {
            uint32_t m = g & tri[k];
            if (m == tri[k] || m == 0) { mono = 1; break; }
        }
        if (!mono) survivors++;
    }
    printf("[B] K6 sweep: 32768 colorings, %ld with no mono triangle  "
           "=> R(3,3) <= 6  %s\n", survivors, survivors == 0 ? "OK" : "FAIL");
}

/* ---------- [C]: R(3,4) > 8 by full sweep of K8 ---------- */

static void r34_lower(void)
{
    uint32_t tri[56], quad[70]; int nt = 0, nq = 0;
    for (int a = 0; a < 8; a++) for (int b = a+1; b < 8; b++)
        for (int c = b+1; c < 8; c++) {
            tri[nt++] = (1u<<eidx(8,a,b)) | (1u<<eidx(8,a,c)) | (1u<<eidx(8,b,c));
            for (int d = c+1; d < 8; d++)
                quad[nq++] = (1u<<eidx(8,a,b)) | (1u<<eidx(8,a,c)) |
                             (1u<<eidx(8,a,d)) | (1u<<eidx(8,b,c)) |
                             (1u<<eidx(8,b,d)) | (1u<<eidx(8,c,d));
        }
    long witnesses = 0; uint32_t first = 0; int have = 0;
    for (uint32_t g = 0; g < (1u << 28); g++) {
        int bad = 0;
        for (int k = 0; k < nt; k++)
            if ((g & tri[k]) == tri[k]) { bad = 1; break; }    /* red K3  */
        if (bad) continue;
        for (int k = 0; k < nq; k++)
            if ((g & quad[k]) == 0)     { bad = 1; break; }    /* blue K4 */
        if (!bad) { witnesses++; if (!have) { first = g; have = 1; } }
    }
    printf("[C] K8 sweep: 268435456 colorings, %ld avoid red K3 and blue K4  "
           "=> R(3,4) > 8  %s\n", witnesses, witnesses > 0 ? "OK" : "FAIL");
    if (have) printf("    first witness (red edge mask, 28 bits): 0x%07X\n",
                     first);

    /* classical cyclic witness: red iff difference in {1,4,7} (mod 8) */
    uint32_t cyc = 0;
    for (int i = 0; i < 8; i++) for (int j = i+1; j < 8; j++) {
        int d = j - i;
        if (d == 1 || d == 4 || d == 7) cyc |= 1u << eidx(8, i, j);
    }
    int ok = 1;
    for (int k = 0; k < nt && ok; k++) if ((cyc & tri[k]) == tri[k]) ok = 0;
    for (int k = 0; k < nq && ok; k++) if ((cyc & quad[k]) == 0)     ok = 0;
    printf("    cyclic witness C8(1,4): %s\n", ok ? "verified" : "FAIL");
}

/* ---------- [D]: R(4,4) > 17 via Paley(17) ---------- */

static void r44_lower(void)
{
    int qr[17] = {0};
    for (int x = 1; x < 17; x++) qr[(x * x) % 17] = 1;
    int red[17][17] = {{0}};
    for (int i = 0; i < 17; i++) for (int j = 0; j < 17; j++)
        if (i != j) red[i][j] = qr[(i - j + 17) % 17];

    /* well-definedness: -1 is a QR mod 17, so the relation is symmetric */
    int sym = qr[16];
    long redK4 = 0, blueK4 = 0, total = 0;
    for (int a = 0; a < 17; a++) for (int b = a+1; b < 17; b++)
        for (int c = b+1; c < 17; c++) for (int d = c+1; d < 17; d++) {
            total++;
            int r = red[a][b] + red[a][c] + red[a][d] +
                    red[b][c] + red[b][d] + red[c][d];
            if (r == 6) redK4++;
            if (r == 0) blueK4++;
        }
    printf("[D] Paley(17): symmetric=%s, %ld 4-subsets checked, "
           "red K4 = %ld, blue K4 = %ld  => R(4,4) > 17  %s\n",
           sym ? "yes" : "NO", total, redK4, blueK4,
           (sym && redK4 == 0 && blueK4 == 0 && total == 2380) ? "OK" : "FAIL");
}

int main(void)
{
    r33();
    r34_lower();
    r44_lower();
    return 0;
}
