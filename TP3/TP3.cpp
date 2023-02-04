#include <stdio.h>
#include "utilitaires.cpp"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  char cNomImgLue[250];
  int nH, nW, nTaillle;

  if (argc != 3)
  {
    printf("Usage: ImageIn.pgm ImageSortie.pgm\n");
    exit(1);
  }
  sscanf(argv[1], "%s", cNomImgLue);

  OCTET *ImgIn, *ImgOut;

  lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
  nTaillle = nH * nW;

  allocation_tableau(ImgIn, OCTET, nTaillle);
  allocation_tableau(ImgOut, OCTET, nTaillle);
  lire_image_pgm(cNomImgLue, ImgIn, nH * nW);

  int *OCCGRIS;
  allocation_tableau(OCCGRIS, int, 256);

  for (int i = 0; i < nH; i++)
    for (int j = 0; j < nW; j++)
    {
      OCCGRIS[ImgIn[i * nW + j]] += 1;
    }

  double entropie = 0.0;
  double p_i = 0.0;
  for (int i = 0; i < 256; i++)
  {
    p_i = (double)OCCGRIS[i] / (double)nTaillle;
    if (p_i != 0)
      entropie -= (p_i)*log2(p_i);
  }
  std::cout << "Valeur de l'entropie : " << entropie << std::endl;

  // Valeur de l'entropie : 6.88684

  // for (size_t i = 0; i < nW; i++)
  // {
  //   for (size_t j = 0; j < nH; j++)
  //   {
  //     if (j == 0)
  //       ImgOut[i * nW + j] = ImgIn[i * nW + j];
  //     else
  //       ImgOut[i * nW + j] = (ImgIn[i * nW + j - 1] - ImgIn[i * nW + j]) + 128;
  //   }
  // }
  double E = 0;

  // for (size_t i = 0; i < nTaillle; i++)
  // {
 

  //   // OCTET r = (((ImgIn[i * nW + j - 1] + ImgIn[i * nW + j - 1]) / 2) - ImgIn[i * nW + j]);
  //   // ImgOut[i * nW + j] = r + 128;
  // }


  for (int i = 0; i < nH; i++)
    {
        for (int j = 0; j < nW; j++)
        {
            int minvois = min((int)ImgIn[i * nW + j - 1], (int)ImgIn[i * nW + j - nW]);
            int maxvois = max((int)ImgIn[i * nW + j - 1], (int)ImgIn[i * nW + j - nW]);

            if ((int)ImgIn[i * nW + j - nW -1] >= maxvois)
            {
                ImgOut[i * nW + j] = minvois - ImgIn[i * nW + j] + 128;
            }
            else if ((int)ImgIn[i * nW + j - nW - 1] <= minvois)
            {
                ImgOut[i * nW + j] = maxvois - ImgIn[i * nW + j] + 128;
            }
            else
            {
                ImgOut[i * nW + j] = (ImgIn[i * nW + j - 1] + ImgIn[i * nW + j - nW] - ImgIn[i * nW + j - nW - 1]) - ImgIn[i * nW + j] + 128;
            }
        }
    }

  // Taux de compression : 1.79 pour l'image d'erreur de pred

  histoGrey(ImgOut, nTaillle);

  ecrire_image_pgm((char *)argv[2], ImgOut, nW, nH);

  return 0;
}
