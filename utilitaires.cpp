#include <stdio.h>
#include "image_ppm.h"
#include <math.h>

OCTET *seuillageGrey(OCTET *ImgIn, int nTaille, int s1, int s2 = 0, int s3 = 0)
{
    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nTaille);

    if (s2 == 0 && s3 == 0 && s1 != 0)
    {
        // 1 seuil

        for (int j = 0; j < nTaille; j++)
        {
            if (ImgIn[j] < s1)
                ImgOut[j] = 0;
            else
                ImgOut[j] = 255;
        }
    }
    else if (s2 != 0 && s3 == 0 && s1 != 0)
    {
        // 2 seuils
        if (s1 >= s2)
        {
            printf("Usage: Seuil1 doit être strictement inférieur Seuil2 \n");
            exit(1);
        }

        for (int j = 0; j < nTaille; j++)
        {
            if (ImgIn[j] < s1)
                ImgOut[j] = 0;
            else if (ImgIn[j] < s2)
                ImgOut[j] = 128;
            else
                ImgOut[j] = 255;
        }
    }
    else if (s2 != 0 && s3 != 0 && s1 != 0)
    {
        // 3 seuils

        if (s1 >= s2 || s2 >= s3)
        {
            printf("Usage: Seuil1 doit être strictement inférieur Seuil2 qui est lui même strictement inférieur à  Seuil3 \n");
            exit(1);
        }
        for (int j = 0; j < nTaille; j++)
        {
            if (ImgIn[j] < s1)
                ImgOut[j] = 0;
            else if (ImgIn[j] < s2)
                ImgOut[j] = 100;
            else if (ImgIn[j] < s3)
                ImgOut[j] = 200;
            else
                ImgOut[j] = 255;
        }
    }
    else
    {
        perror("paramètres fonction seuillage incorrects");
    }
    return ImgOut;
}

void histoGrey(OCTET *ImgIn, int nTaille)
{
    int *OCCGRIS;

    allocation_tableau(OCCGRIS, int, 256);

    FILE *histo = fopen("./histo.dat", "w");

    for (int j = 0; j < nTaille; j++)
    {
        OCCGRIS[ImgIn[j]] += 1;
    }
    for (int i = 0; i < 256; i++)
    {
        fprintf(histo, "%d %d\n", i, OCCGRIS[i]);
    }
    FILE *f = popen("gnuplot", "w");
    fprintf(f, "set term png\n");
    fprintf(f, "set output \"histo.png\"\n");
    fprintf(f, "plot \"histo.dat\" with lines\n");
    fprintf(f, "set term x11\n");
}

void profilGrey(OCTET *ImgIn, int nW, int nH, bool col, int indice)
{
    int *OCCGRIS;
    int n = col ? nW : nH;
    allocation_tableau(OCCGRIS, int, n);

    FILE *profil = fopen("./profil.dat", "w");

    for (int j = 0; j < n; j++)
    {
        if (col == 0)
        {
            OCCGRIS[j] = ImgIn[indice * n + j];
        }
        else if (col == 1)
        {
            OCCGRIS[j] = ImgIn[j * n + indice];
        }
    }

    for (int i = 0; i < n; i++)
    {
        fprintf(profil, "%d %d\n", i, OCCGRIS[i]);
    }
    FILE *f = popen("gnuplot", "w");
    fprintf(f, "set term png\n");
    fprintf(f, "set output \"profil.png\"\n");
    fprintf(f, "plot \"profil.dat\" with lines\n");
    fprintf(f, "set term x11\n");
}

OCTET *seuillageCouleur(OCTET *ImgIn, int nTaille, int SR, int SG, int SB)
{
    OCTET *ImgOut;
    int R, G, B;
    allocation_tableau(ImgOut, OCTET, nTaille * 3);
    for (int i = 0; i < nTaille * 3; i += 3)
    {
        R = ImgIn[i];
        G = ImgIn[i + 1];
        B = ImgIn[i + 2];
        if (R < SR)
            ImgOut[i] = 0;
        else
            ImgOut[i] = 255;
        if (G < SG)
            ImgOut[i + 1] = 0;
        else
            ImgOut[i + 1] = 255;
        if (B < SB)
            ImgOut[i + 2] = 0;
        else
            ImgOut[i + 2] = 255;
    }

    return ImgOut;
}

void histoCouleur(OCTET *ImgIn, int nTaille)
{
    int *OCCROUGE;
    allocation_tableau(OCCROUGE, int, 256);

    int *OCCVERT;
    allocation_tableau(OCCVERT, int, 256);

    int *OCCBLEU;
    allocation_tableau(OCCBLEU, int, 256);

    FILE *histo = fopen("./histocouleur.dat", "w");
    for (int i = 0; i < nTaille * 3; i += 3)
    {
        OCCROUGE[ImgIn[i]] += 1;
        OCCVERT[ImgIn[i + 1]] += 1;
        OCCBLEU[ImgIn[i + 2]] += 1;
    }
    for (int i = 0; i < 256; i++)
    {
        fprintf(histo, "%d %d %d %d\n", i, OCCROUGE[i], OCCVERT[i], OCCBLEU[i]);
    }

    fclose(histo);
    FILE *f = popen("gnuplot", "w");
    fprintf(f, "set term png\n");
    fprintf(f, "set output \"histo.png\"\n");
    // fprintf(f, "set multiplot layout 1,3 \n");
    fprintf(f, "plot \"histocouleur.dat\" using 1:2 title \"histo-Red\" with lines, ");
    fprintf(f, " \"histocouleur.dat\" using 1:3 title \"histo-Green\" with lines,");
    fprintf(f, " \"histocouleur.dat\" using 1:4 title \"histo-Blue\" with lines\n");
    // fprintf(f, "unset multiplot");
}

OCTET *erosionGrey(OCTET *ImgIn, int nH, int nW)
{
    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nH * nW);

    for (int i = 0; i < nH; i++)
    {
        for (int j = 0; j < nW; j++)
        {
            ImgOut[i * nW + j] = 0;
            for (int ip = i - 1; ip < i + 1; ip++)
                for (int jp = j - 1; jp < j + 1; jp++)
                {
                    if (ip < 0 || ip >= nH || jp < 0 || jp >= nH)
                    {
                        continue;
                    }
                    else
                    {
                        if (ImgIn[ip * nW + jp] == 255)
                        {
                            ImgOut[i * nW + j] = 255;
                        }
                    }
                }
        }
    }
    return ImgOut;
}

OCTET *dilatationGrey(OCTET *ImgIn, int nH, int nW)
{
    OCTET *ImgOut;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);
    for (int i = 0; i < nH; i++)
        for (int j = 0; j < nW; j++)
        {
            ImgOut[i * nW + j] = 255;
            for (int ip = i - 1; ip <= i + 1; ip++)
                for (int jp = j - 1; jp <= j + 1; jp++)
                {
                    if (ip < 0 || ip >= nH || jp < 0 || jp >= nH)
                    {
                        continue;
                    }
                    else
                    {
                        if (ImgIn[ip * nW + jp] == 0)
                        {
                            ImgOut[i * nW + j] = 0;
                        }
                    }
                }
        }
    return ImgOut;
}

OCTET *fermetureGrey(OCTET *ImgIn, int nH, int nW)
{
    OCTET *ImgOut;
    OCTET *ImgOut2;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);
    allocation_tableau(ImgOut2, OCTET, nTaille);

    ImgOut = dilatationGrey(ImgIn, nH, nW);
    ImgOut2 = erosionGrey(ImgOut, nH, nW);
    return ImgOut2;
}

OCTET *ouvertureGrey(OCTET *ImgIn, int nH, int nW)
{
    OCTET *ImgOut;
    OCTET *ImgOut2;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);
    allocation_tableau(ImgOut2, OCTET, nTaille);

    ImgOut = erosionGrey(ImgIn, nH, nW);
    ImgOut2 = dilatationGrey(ImgOut, nH, nW);
    return ImgOut2;
}

OCTET *differenceGrey(OCTET *ImgD, OCTET *ImgS, int nW, int nH)
{
    OCTET *ImgOut;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);

    for (int i = 0; i < nH; i++)
        for (int j = 0; j < nW; j++)
        {
            if (ImgD[i * nW + j] == ImgS[i * nW + j] && ImgS[i * nW + j] == 0)
                ImgOut[i * nW + j] = 0;
            else if (ImgD[i * nW + j] == ImgS[i * nW + j] && ImgS[i * nW + j] == 255)
                ImgOut[i * nW + j] = 0;
            else
                ImgOut[i * nW + j] = 255;
        }
    return ImgOut;
}

OCTET *inverseGrey(OCTET *ImgIn, int nTaille)
{

    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nTaille);

    for (int j = 0; j < nTaille; j++)
    {
        ImgOut[j] = ImgIn[j] * (-1) + 255;
    }
    return ImgOut;
}

OCTET *filtreflou1Grey(OCTET *ImgIn, int nH, int nW)
{

    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nH * nW);

    for (int i = 0; i < nH; i++)
        for (int j = 0; j < nW; j++)
        {
            if (i - 1 > 0 && i + 1 < nH && j - 1 > 0 && j + 1 < nH)
            {
                ImgOut[i * nW + j] = (ImgIn[i * nW + j] + ImgIn[(i - 1) * nW + j] + ImgIn[(i + 1) * nW + j] + ImgIn[i * nW + (j - 1)] + ImgIn[i * nW + (j + 1)]) / 5;
            }
            else
            {
                ImgOut[i * nW + j] = ImgIn[i * nW + j];
            }
        }
    return ImgOut;
}

OCTET *filtreflou2Grey(OCTET *ImgIn, int nH, int nW)
{

    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nH * nW);

    for (int i = 0; i < nH * nW; i++)
        ImgOut[i] = ImgIn[i];

    for (int i = 1; i < nH - 1; i++)
        for (int j = 1; j < nW - 1; j++)
        {
            int som =
                ImgIn[(i - 1) * nW + j - 1] + ImgIn[(i - 1) * nW + j] + ImgIn[(i - 1) * nW + j + 1] + ImgIn[i * nW + j - 1] + ImgIn[i * nW + j] + ImgIn[i * nW + j + 1] + ImgIn[(i + 1) * nW + j - 1] + ImgIn[(i + 1) * nW + j] + ImgIn[(i + 1) * nW + j + 1];

            ImgOut[i * nW + j] = som / 9;
        }
    return ImgOut;
}

OCTET *filtrefloucouleur(OCTET *ImgIn, int nW, int nH)
{
    int nTaille = nW * nH;
    OCTET *R, *G, *B, *Rf, *Gf, *Bf, *ImgOut;
    allocation_tableau(R, OCTET, nTaille);
    allocation_tableau(G, OCTET, nTaille);
    allocation_tableau(B, OCTET, nTaille);
    allocation_tableau(Rf, OCTET, nTaille);
    allocation_tableau(Gf, OCTET, nTaille);
    allocation_tableau(Bf, OCTET, nTaille);

    allocation_tableau(ImgOut, OCTET, nTaille * 3);
    planR(R, ImgIn, nTaille);
    planV(G, ImgIn, nTaille);
    planB(B, ImgIn, nTaille);

    for (int i = 0; i < nTaille; i++)
    {
        Rf[i] = R[i];
        Gf[i] = G[i];
        Bf[i] = B[i];
    }

    for (int i = 1; i < nH - 1; i++)
    {
        for (int j = 1; j < nW - 1; j++)
        {
            int somR =
                R[(i - 1) * nW + j - 1] + R[(i - 1) * nW + j] + R[(i - 1) * nW + j + 1] + R[i * nW + j - 1] + R[i * nW + j] + R[i * nW + j + 1] + R[(i + 1) * nW + j - 1] + R[(i + 1) * nW + j] + R[(i + 1) * nW + j + 1];

            Rf[i * nW + j] = somR / 9;

            int somG =
                G[(i - 1) * nW + j - 1] + G[(i - 1) * nW + j] + G[(i - 1) * nW + j + 1] + G[i * nW + j - 1] + G[i * nW + j] + G[i * nW + j + 1] + G[(i + 1) * nW + j - 1] + G[(i + 1) * nW + j] + G[(i + 1) * nW + j + 1];

            Gf[i * nW + j] = somG / 9;

            int somB =
                B[(i - 1) * nW + j - 1] + B[(i - 1) * nW + j] + B[(i - 1) * nW + j + 1] + B[i * nW + j - 1] + B[i * nW + j] + B[i * nW + j + 1] + B[(i + 1) * nW + j - 1] + B[(i + 1) * nW + j] + B[(i + 1) * nW + j + 1];

            Bf[i * nW + j] = somB / 9;
        }
    }

    for (int i = 0; i < nTaille; i++)
    {
        ImgOut[3 * i] = Rf[i];
        ImgOut[3 * i + 1] = Gf[i];
        ImgOut[3 * i + 2] = Bf[i];
    }

    return ImgOut;
}

OCTET *normeImgGrey(OCTET *ImgIn, int nW, int nH)
{
    OCTET *ImgOut;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);

    for (int i = 0; i < nH; i++)
    {
        for (int j = 0; j < nW; j++)
        {
            if (i - 1 < 0 || i == nH || j - 1 < 0 || j == nW)
            {
                ImgOut[i * nW + j] = ImgIn[i * nW + j];
            }
            else
            {
                int hor = ImgIn[i * nW + j + 1] - ImgIn[i * nW + j - 1];
                int vert = ImgIn[(i + 1) * nW + j] - ImgIn[(i - 1) * nW + j];
                ImgOut[i * nW + j] = std::sqrt(hor * hor + vert * vert);
            }
        }
    }
    return ImgOut;
}

OCTET *seuilMaxGradientGrey(OCTET *ImgIn, int S, int nW, int nH){
     OCTET *ImgOut;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);
    for (int i=0; i < nH; i++)
   for (int j=0; j < nW; j++)
     {
       if ( ImgIn[i*nW+j] < S) ImgOut[i*nW+j]=0; else ImgOut[i*nW+j]=255;
     }
     return ImgOut;
}

OCTET *seuilHisteresisGrey(OCTET *ImgIn, int SB, int SH, int nW, int nH)
{
    OCTET *ImgOut;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);

    // Première lecture
    for (int i = 0; i < nH; i++)
        for (int j = 0; j < nW; j++)
        {
            if (ImgIn[i * nW + j] <= SB)
                ImgOut[i * nW + j] = 0;
            else if (ImgIn[i * nW + j] >= SH)
                ImgOut[i * nW + j] = 255;
            else
                ImgOut[i * nW + j] = ImgIn[i * nW + j];
        }

    // Seconde lecture
    for (int i = 0; i < nH; i++)
        for (int j = 0; j < nW; j++)
        {
            if (!(i - 1 < 0 || i == nH || j - 1 < 0 || j == nW))
            {

                if (SB < ImgOut[i * nW + j] && ImgOut[i * nW + j] > SH && (ImgOut[(i - 1) * nW + j - 1] == 255 || ImgOut[(i - 1) * nW + j] == 255 || ImgOut[(i - 1) * nW + j + 1] == 255 || ImgOut[i * nW + j - 1] == 255 || ImgOut[i * nW + j] == 255 || ImgOut[i * nW + j + 1] == 255 || ImgOut[(i + 1) * nW + j - 1] == 255 || ImgOut[(i + 1) * nW + j] == 255 || ImgOut[(i + 1) * nW + j + 1] == 255))
                {
                    ImgOut[i * nW + j] = 255;
                }
                else
                {
                    ImgOut[i * nW + j] = 0;
                }
            }
        }
    return ImgOut;
}

OCTET *filtreGaussGrey(OCTET *ImgIn, int nW, int nH)
{
    OCTET *ImgOut;
    int nTaille = nH * nW;
    allocation_tableau(ImgOut, OCTET, nTaille);
    for (int i = 0; i < nTaille; i++)
        ImgOut[i] = ImgIn[i];

    for (int i = 1; i < nH - 1; i++)
        for (int j = 1; j < nW - 1; j++)
        {
            int som =
                ImgIn[(i - 1) * nW + j - 1] + 2 * ImgIn[(i - 1) * nW + j] + ImgIn[(i - 1) * nW + j + 1] + 2 * ImgIn[i * nW + j - 1] + 4 * ImgIn[i * nW + j] + 2 * ImgIn[i * nW + j + 1] + ImgIn[(i + 1) * nW + j - 1] + ImgIn[(i + 1) * nW + j] + ImgIn[(i + 1) * nW + j + 1];

            ImgOut[i * nW + j] = som / 16;
        }
    return ImgOut;
}

OCTET *RGBtoY(OCTET *ImgIn, int nTaille)
{
    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nTaille);
    for (int i = 0; i < nTaille; i++)
    {
        ImgOut[i] = 0.299 * ImgIn[3 * i] + 0.587 * ImgIn[3 * i + 1] + 0.114 * ImgIn[3 * i + 2];
    }
    return ImgOut;
}

// a tester:
float erreurQuadratique(OCTET *ImgY, OCTET *ImgPPM, int nTaille)
{
    float k = 0;
    for (int j = 0; j < nTaille; j++)
    {
        k += (ImgY[j] - ImgPPM[j]) * (ImgY[j] - ImgPPM[j]);
    }
    k = k / nTaille;
    return k;
}

OCTET *RGBtoCb(OCTET *ImgIn, int nTaille)
{
    OCTET *ImgCb;
    allocation_tableau(ImgCb, OCTET, nTaille);
    for (int i = 0; i < nTaille; i++)
    {
        ImgCb[i] = -0.168736 * ImgIn[3 * i] - 0.331264 * ImgIn[3 * i + 1] + 0.5 * ImgIn[3 * i + 2] + 128;
    }
    return ImgCb;
}

OCTET *RGBtoCr(OCTET *ImgIn, int nTaille)
{
    OCTET *ImgCr;
    allocation_tableau(ImgCr, OCTET, nTaille);
    for (int i = 0; i < nTaille; i++)
        ImgCr[i] = 0.5 * ImgIn[3 * i] - 0.418688 * ImgIn[3 * i + 1] - 0.081312 * ImgIn[3 * i + 2] + 128;
    return ImgCr;
}

OCTET *modifY(OCTET *ImgY, int k, int nTaille)
{
    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nTaille);
    for (int i = 0; i < nTaille; i++)
    {
        int nY = ImgY[i] + k;

        ImgOut[i] = nY > 255 ? 255 : nY < 0 ? 0
                                            : nY;
    }
    return ImgOut;
}

OCTET *YcbCrTOrgb(OCTET *ImgY, OCTET *ImgCb, OCTET *ImgCr, int nTaille)
{
    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nTaille * 3);

    for (int i = 0; i < nTaille; i++)
    {
        float nR = ImgY[i] + 1.402 * (ImgCr[i] - 128);
        float nG = ImgY[i] - 0.34414 * (ImgCb[i] - 128) - 0.71414 * (ImgCr[i] - 128);
        float nB = ImgY[i] + 1.772 * (ImgCb[i] - 128);

        // RGB
        ImgOut[3 * i] = nR > 255 ? 255 : nR < 0 ? 0: nR;
        ImgOut[3 * i + 1] = nG > 255 ? 255 : nG < 0 ? 0: nG;
        ImgOut[3 * i + 2] = nB > 255 ? 255 : nB < 0 ? 0: nB;

    }
    return ImgOut;
}

OCTET *YcbCrTOrbg(OCTET *ImgY, OCTET *ImgCb, OCTET *ImgCr, int nTaille)
{
    OCTET *ImgOut;
    allocation_tableau(ImgOut, OCTET, nTaille * 3);

    for (int i = 0; i < nTaille; i++)
    {
        float nR = ImgY[i] + 1.402 * (ImgCr[i] - 128);
        float nG = ImgY[i] - 0.34414 * (ImgCb[i] - 128) - 0.71414 * (ImgCr[i] - 128);
        float nB = ImgY[i] + 1.772 * (ImgCb[i] - 128);
        // RBG
        ImgOut[3 * i] = nR > 255 ? 255 : nR < 0 ? 0: nR;
        ImgOut[3 * i + 2] = nG > 255 ? 255 : nG < 0 ? 0: nG;
        ImgOut[3 * i + 1] = nB > 255 ? 255 : nB < 0 ? 0: nB;
    }
    return ImgOut;
}