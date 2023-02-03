#include "utilitaires.cpp"
#include <iostream>
#include <math.h>
#include <vector>
#include <float.h>

using namespace std;

void ReduceImage(OCTET *ImgToDivide, int witdh, int height, OCTET *&ImgDivided)
{

    for (size_t i = 0; i < witdh / 2; i++)
    {
        for (size_t j = 0; j < height / 2; j++)
        {

            int pix = ImgToDivide[2 * i * witdh + 2 * j] + ImgToDivide[2 * i * witdh + 2 * (j + 1)] + ImgToDivide[2 * (i + 1) * witdh + 2 * j] + ImgToDivide[2 * (i + 1) * witdh + 2 * (j + 1)];
            char valPix = pix / 4;
            ImgDivided[i * (witdh / 2) + j] = valPix;
        }
    }
}



double PSNR(OCTET* S, OCTET* R,int ntaille)
{
    double EQMr;
    double EQMg;
    double EQMb;

    for (size_t i = 0; i <ntaille; i++)
    {
        EQMr += ((S[i*3] - R[i*3]) * (S[i*3] - R[i*3]));
        EQMg += ((S[i*3+1] - R[i*3+1]) * (S[i*3+1] - R[i*3+1]));
        EQMb += ((S[i*3+2] - R[i*3+2]) * (S[i*3+2] - R[i*3+2]));
    }
    EQMr /= ntaille;
    EQMg /= ntaille;
    EQMb /= ntaille;

    double EQM = EQMr + EQMg + EQMb;
    EQM /= 3;

    return 10 * log10((255 * 255) / EQM);
}

void ResizeImage(OCTET *ImgToResize, int witdh, int height, OCTET *&ImgResized)
{
    OCTET *ImgTemp;
    allocation_tableau(ImgTemp, OCTET, witdh * height);

    for (size_t i = 0; i < witdh / 2; i++)
    {
        for (size_t j = 0; j < height / 2; j++)
        {

            ImgTemp[i * witdh * 2 + (j * 2)] = ImgToResize[i * (witdh / 2) + j];
            ImgTemp[(i * 2) * witdh + (j * 2) + 1] = ImgToResize[i * (witdh / 2) + j];
            ImgTemp[(i * 2 + 1) * witdh + (j * 2)] = ImgToResize[i * (witdh / 2) + j];
            ImgTemp[(i * 2 + 1) * witdh + (j * 2) + 1] = ImgToResize[i * (witdh / 2) + j];
        }
    }

    for (size_t i = 0; i < witdh; i++)
    {
        for (size_t j = 0; j < height; j++)
        {
            int pix = 0;
            int cpt = 0;

            if (i == 0 || j == 0 || i == witdh - 1 || j == height - 1)
            {
                ImgResized[i * witdh + j] = ImgTemp[i * witdh + j];
            }
            else
            {
                for (int ip = i - 1; ip <= i + 1; ip++)
                    for (int jp = j - 1; jp <= j + 1; jp++)
                    {
                        // if (ip < 0 || ip >= witdh || jp < 0 || jp >= height)
                        // {
                        //     continue;
                        // }
                        // else
                        // {
                        cpt++;
                        pix += ImgTemp[ip * witdh + jp];
                        // }
                    }
                if (cpt != 0)
                    pix = (int)(pix / cpt);
                ImgResized[i * witdh + j] = pix;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    OCTET *ImgIn, *ImgOut, *plan1, *plan2, *plan3, *ImgReduce1, *ImgResize1, *ImgResize2, *ImgReduce2;

    if (argc != 3)
    {
        printf("usage:ImgIn.ppm ImgOut.ppm");
        exit(1);
    }

    int w, h;
    // lire_nb_lignes_colonnes_image_pgm((char *)argv[1], &w, &h);
    lire_nb_lignes_colonnes_image_ppm((char *)argv[1], &w, &h);

    int ntaille = w * h;
    allocation_tableau(ImgIn, OCTET, ntaille * 3);
    allocation_tableau(ImgOut, OCTET, ntaille * 3);
    allocation_tableau(plan1, OCTET, ntaille);
    allocation_tableau(plan2, OCTET, ntaille);
    allocation_tableau(plan3, OCTET, ntaille);
    allocation_tableau(ImgReduce1, OCTET, ntaille / 4);
    allocation_tableau(ImgResize1, OCTET, ntaille);
    allocation_tableau(ImgResize2, OCTET, ntaille);
    allocation_tableau(ImgReduce2, OCTET, ntaille / 4);
    // lire_image_pgm((char *)argv[1], ImgIn, ntaille);
    lire_image_ppm((char *)argv[1], ImgIn, ntaille);

    planR(plan1, ImgIn, ntaille);
    ReduceImage(plan1, w, h, ImgReduce2);

    // plan1 = RGBtoY(ImgIn, ntaille);

    planB(plan2, ImgIn, ntaille);
    // plan2 = RGBtoCr(ImgIn, ntaille);
    ReduceImage(plan2, w, h, ImgReduce1);

    planV(plan3, ImgIn, ntaille);
    // plan3 = RGBtoCb(ImgIn, ntaille);

    // ecrire_image_pgm("Y.pgm", plan1, w, h);
    // ecrire_image_pgm("Cr.pgm", plan2, w, h);
    // ecrire_image_pgm("Cb.pgm", plan3, w, h);

    // ecrire_image_pgm("ReduceCr.pgm", ImgReduce1, w / 2, h / 2);
    // ecrire_image_pgm("ReduceCb.pgm", ImgReduce2, w / 2, h / 2);

    ResizeImage(ImgReduce1, w, h, ImgResize1);
    ResizeImage(ImgReduce2, w, h, ImgResize2);
    // ecrire_image_pgm("ResizeCr.pgm", ImgResize1, w, h);
    // ecrire_image_pgm("ResizeCb.pgm", ImgResize2, w, h);

    for (size_t i = 0; i < ntaille; i++)
    {
        ImgOut[i*3+1]=plan3[i];
        ImgOut[i*3]=ImgResize2[i];
        ImgOut[i*3+2]=ImgResize1[i];
    }
    // ImgOut = YcbCrTOrgb(plan1, ImgResize2, ImgResize1, ntaille);

    ecrire_image_ppm((char *)argv[2], ImgOut, w, h);
    cout<<PSNR(ImgIn,ImgOut,ntaille);
    cout << "image crée" << endl;

    return 0;
}
