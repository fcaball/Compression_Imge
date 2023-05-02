#include <stdio.h>
#include "utilitaires.cpp"

#include <iostream>

using namespace std;

double PSNRGrey(OCTET *S, OCTET *R, int ntaille)
{
  double EQM;

  for (size_t i = 0; i < ntaille; i++)
  {
    EQM += ((S[i] - R[i]) * (S[i] - R[i]));
  }
  EQM /= ntaille;

  return 10 * log10((255 * 255) / EQM);
}

OCTET* Reconstruction_et_PSNR(OCTET *ImgIn, OCTET *BF, OCTET *BHF, OCTET *HBF, OCTET *HF, int nW, int nH,int N)
{
  OCTET *ImgOut;
  int nTaille = nW * nH;
  allocation_tableau(ImgOut, OCTET, nTaille);
  for (int i = 0; i < nH / 2; i++)
    {
        for (int j = 0; j < nW / 2; j++)
        {
            ImgOut[i * 2 * nW + j * 2] = std::min(255, std::max(0,((HBF[i * (nW / 2) + j] - 128) / 2) + BF[i * (nW / 2) + j] + ((BHF[i * (nW / 2) + j] - 128) / 2) + ((HF[i * (nW / 2) + j] - 128) / 4)));

            ImgOut[i * 2 * nW + j * 2 + 1] = std::min(255, std::max(0,BF[i * (nW / 2) + j] + ((HBF[i * (nW / 2) + j] - 128) / 2) - ((BHF[i * (nW / 2) + j] - 128) / 2) - ((HF[i * (nW / 2) + j] - 128) / 4)));

            ImgOut[i * 2 * nW + j * 2 + nW] =std::min(255, std::max(0, BF[i * (nW / 2) + j] - ((HBF[i * (nW / 2) + j] - 128) / 2) + ((BHF[i * (nW / 2) + j] - 128) / 2) - ((HF[i * (nW / 2) + j] - 128) / 4)));

            ImgOut[i * 2 * nW + j * 2 + nW + 1] = std::min(255, std::max(0,((HF[i * (nW / 2) + j] - 128) / 4) + BF[i * (nW / 2) + j] - ((HBF[i * (nW / 2) + j] - 128) / 2) - ((BHF[i * (nW / 2) + j] - 128) / 2)));
        }
    }
  

  
  ecrire_image_pgm((char*)"ReconstruiteQuantificationCoeff1_4_4_16.pgm",ImgOut,nH,nW);

  cout <<7-N<<" : "<< PSNRGrey(ImgIn, ImgOut, nTaille) << endl;
  return ImgOut;

}

void UneImageEnQuatre(OCTET *ImgIn, OCTET *&ImgHautGauche, OCTET *&ImgHautDroite, OCTET *&ImgBasGauche, OCTET *&ImgBasDroite, int nW, int nH)
{
  OCTET *ImgOut;
  allocation_tableau(ImgOut, OCTET, nW * nH);

  for (size_t i = 0; i < nW / 2; i++)
  {
    for (size_t j = 0; j < nH / 2; j++)
    {
      ImgHautGauche[i * (nW / 2) + j] = ImgOut[i * nW + j];
    }
  }

  for (size_t i = 0; i < nW / 2; i++)
  {
    for (size_t j = 0; j < nH / 2; j++)
    {
      ImgHautDroite[i * (nW / 2) + j] = ImgOut[(i)*nW + j + nW / 2];
    }
  }

  for (size_t i = 0; i < nW / 2; i++)
  {
    for (size_t j = 0; j < nH / 2; j++)
    {
      ImgBasGauche[i * (nW / 2) + j] = ImgOut[(i + nW / 2) * nW + j];
    }
  }

  for (size_t i = 0; i < nW / 2; i++)
  {
    for (size_t j = 0; j < nH / 2; j++)
    {
      ImgBasDroite[i * (nW / 2) + j] = ImgOut[(i + nW / 2) * nW + j + nW / 2];
    }
  }
}

OCTET *QuatresImagesEnUne(OCTET *ImgHautGauche, OCTET *ImgHautDroite, OCTET *ImgBasGauche, OCTET *ImgBasDroite, int WFinale, int HFinale)
{

  OCTET *ImgOut;
  allocation_tableau(ImgOut, OCTET, WFinale * HFinale);

  for (size_t i = 0; i < WFinale / 2; i++)
  {
    for (size_t j = 0; j < HFinale / 2; j++)
    {
      ImgOut[i * WFinale + j] = ImgHautGauche[i * (WFinale / 2) + j];
    }
  }
  for (size_t i = 0; i < WFinale / 2; i++)
  {
    for (size_t j = 0; j < HFinale / 2; j++)
    {
      ImgOut[(i)*WFinale + j + WFinale / 2] = ImgHautDroite[i * (WFinale / 2) + j];
    }
  }

  for (size_t i = 0; i < WFinale / 2; i++)
  {
    for (size_t j = 0; j < HFinale / 2; j++)
    {
      ImgOut[(i + WFinale / 2) * WFinale + j] = ImgBasGauche[i * (WFinale / 2) + j];
    }
  }
  for (size_t i = 0; i < WFinale / 2; i++)
  {
    for (size_t j = 0; j < HFinale / 2; j++)
    {
      ImgOut[(i + WFinale / 2) * WFinale + j + WFinale / 2] = ImgBasDroite[i * (WFinale / 2) + j];
    }
  }
  return ImgOut;
}

void create4sousbandes(OCTET *ImgIn, OCTET *&BF, OCTET *&HF, OCTET *&HBF, OCTET *&BHF, int nW, int nH)
{
  for (size_t i = 0; i < nW / 2; i++)
  {
    for (size_t j = 0; j < nH / 2; j++)
    {
      int A = ImgIn[i * 2 * nW + j * 2];
      int B = ImgIn[(i + 1) * 2 * nW + j * 2];
      int C = ImgIn[i * 2 * nW + (j + 1) * 2];
      int D = ImgIn[(i + 1) * 2 * nW + (j + 1) * 2];
      BF[i * nW / 2 + j] = (A + B + C + D) / 4;
      if (A - B - C + D + 128 > 255)
      {
        HF[i * nW / 2 + j] = 255;
      }
      else
      {
        HF[i * nW / 2 + j] = A - B - C + D + 128;
      }

      if ((A - B + C - D) / 2 + 128 > 255)
      {
        HBF[i * nW / 2 + j] = 255;
      }
      else
      {
        HBF[i * nW / 2 + j] = (A - B + C - D) / 2 + 128; // HBF
      }

      if ((A + B - C - D) / 2 + 128 > 255)
      {
        BHF[i * nW / 2 + j] = 255;
      }
      else
      {
        BHF[i * nW / 2 + j] = (A + B - C - D) / 2 + 128; // BHF
      }
    }
  }
}

OCTET *transformeeOndulettesRecursive(OCTET *ImgIn, int N, int nW, int nH)
{

  if (N < 1)
  {
    return ImgIn;
  }

  OCTET *BF, *HBF, *BHF, *HF;
  allocation_tableau(BF, OCTET, (nW * nH) / 4);
  allocation_tableau(HBF, OCTET, (nW * nH) / 4);
  allocation_tableau(BHF, OCTET, (nW * nH) / 4);
  allocation_tableau(HF, OCTET, (nW * nH) / 4);
  create4sousbandes(ImgIn, BF, HF, HBF, BHF, nW, nH);

  Reconstruction_et_PSNR(ImgIn, BF, BHF, HBF, HF, nW, nH,N);

  return QuatresImagesEnUne(transformeeOndulettesRecursive(BF, N - 1, nW / 2, nH / 2), HBF, BHF, HF, nW, nH);
}

int main(int argc, char *argv[])
{
  char cNomImgLue[250];
  int nH, nW;

  if (argc != 3)
  {
    printf("Usage: ImageIn.pgm ImageSortie.pgm\n");
    exit(1);
  }
  sscanf(argv[1], "%s", cNomImgLue);

  OCTET *ImgIn, *ImgOut;

  lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
  int nTaille = nH * nW;

  allocation_tableau(ImgIn, OCTET, nTaille);
  allocation_tableau(ImgOut, OCTET, nTaille);
  lire_image_pgm(cNomImgLue, ImgIn, nH * nW);

  OCTET *BF, *HBF, *BHF, *HF;
  allocation_tableau(BF, OCTET, nTaille / 4);
  allocation_tableau(HBF, OCTET, nTaille / 4);
  allocation_tableau(BHF, OCTET, nTaille / 4);
  allocation_tableau(HF, OCTET, nTaille / 4);

  OCTET *BF2, *HBF2, *BHF2, *HF2;

  allocation_tableau(BF2, OCTET, nTaille / 8);
  allocation_tableau(HBF2, OCTET, nTaille / 8);
  allocation_tableau(BHF2, OCTET, nTaille / 8);
  allocation_tableau(HF2, OCTET, nTaille / 8);

  // create4sousbandes(ImgIn,BF,HF,HBF,BHF,nW,nH);
  // int coeff=16;
  // for (size_t i = 0; i < nTaille/4; i++)
  // {
  //   BF[i]/=4;
  //   HF[i]/=coeff*coeff;
  //   BHF[i]/=coeff;
  //   HBF[i]/=coeff;
  // }

  // for (size_t i = 0; i < nTaille/4; i++)
  // {
  //   BF[i]*=4;
  //   HF[i]*=coeff*coeff;
  //   BHF[i]*=coeff;
  //   HBF[i]*=coeff;
  // }
  // OCTET* IMG=Reconstruction_et_PSNR(ImgIn,BF,BHF,HBF,HF,nW,nH,6);

  // OCTET *ImgInter = transformeeOndulettesRecursive(ImgIn, 1, nW, nH);

  int *value;
  allocation_tableau(value, int, 256);

  for (int i = 0; i < nH; i++)
  {
    for (int j = 0; j < nW; j++)
    {
      value[ImgIn[i * nW + j]] += 1;
    }
  }

  double entropie = 0.0;
  double prob = 0.0;
  for (int i = 0; i < 256; i++)
  {
    prob = (double)value[i] / (double)nTaille;
    if (prob != 0)
      entropie -= prob * log2(prob);
  }
  cout<<"E : "<<entropie<<endl;
  // ecrire_image_pgm((char *)argv[2], BF, nH / 2, nW / 2);

  return 0;
}
