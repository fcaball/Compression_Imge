#include "utilitaires.cpp"
#include <iostream>
#include <math.h>
#include <vector>
#include <float.h>

using namespace std;

struct RGB
{
    unsigned int r, g, b, centroid;
};

vector<RGB> OCTETtoRGB(OCTET *ImgIn, int ntaille)
{
    vector<RGB> result;
    for (size_t i = 0; i < ntaille * 3; i += 3)
    {
        result.push_back({ImgIn[i], ImgIn[i + 1], ImgIn[i + 2]});
    }
    return result;
}

float distance(RGB a, RGB b)
{
    return sqrt(((int)a.r - (int)b.r) * ((int)a.r - (int)b.r) + ((int)a.g - (int)b.g) * ((int)a.g - (int)b.g) + ((int)a.b - (int)b.b) * ((int)a.b - (int)b.b));
}

vector<RGB> computeIcentroidAleaInImg(vector<RGB> ImgIn, int I)
{
    vector<RGB> centroidsI;
    for (size_t k = 0; k < I; k++)
    {
        int i = rand() % ImgIn.size();
        centroidsI.push_back(ImgIn[i]);
    }
    return centroidsI;
}
void makePalette(vector<RGB> centroids, OCTET *ImgOut)
{
    for (size_t i = 0; i < centroids.size(); i++)
    {
        ImgOut[i * 3] = centroids[i].r;
        ImgOut[i * 3 + 1] = centroids[i].g;
        ImgOut[i * 3 + 2] = centroids[i].b;
    }
}

double PSNR(vector<RGB> S, vector<RGB> R)
{
    double EQMr;
    double EQMg;
    double EQMb;

    for (size_t i = 0; i < S.size(); i++)
    {
        EQMr += ((S[i].r - R[i].r) * (S[i].r - R[i].r));
        EQMg += ((S[i].g - R[i].g) * (S[i].g - R[i].g));
        EQMb += ((S[i].b - R[i].b) * (S[i].b - R[i].b));
    }
    EQMr /= S.size();
    EQMg /= S.size();
    EQMb /= S.size();

    double EQM = EQMr + EQMg + EQMb;
    EQM /= 3;

    return 10 * log10((255 * 255) / EQM);
}

void kmean(vector<RGB> &Cs, vector<RGB> &ImgIn, float threshold)
{
    int changes = 0;
    size_t k = Cs.size();
    size_t nbPixels = ImgIn.size();
    float min;
    float d;
    for (size_t i = 0; i < nbPixels; i++)
    {
        min = FLT_MAX;
        for (size_t j = 0; j < k; j++)
        {
            d = distance(ImgIn[i], Cs[j]);
            if (distance(ImgIn[i], Cs[min]) > d)
            {
                min = j;
            }
        }
        if (ImgIn[i].centroid != min)
        {
            ImgIn[i].centroid = min;
            changes++;
        }
    }

    for (size_t i = 0; i < k; i++)
    {

        int nbPix = 0;
        int r = 0, g = 0, b = 0;
        for (size_t j = 0; j < nbPixels; j++)
        {
            if (ImgIn[j].centroid == i)
            {
                nbPix++;
                r += ImgIn[j].r;
                g += ImgIn[j].g;
                b += ImgIn[j].b;
            }
        }
        if (nbPix != 0)
        {
            r = r / nbPix;
            g = g / nbPix;
            b = b / nbPix;
            Cs[i] = {(unsigned int)r, (unsigned int)g, (unsigned int)b};
        }
    }

    if (changes / nbPixels > threshold)
    {
        kmean(Cs, ImgIn, threshold);
    }
}

int main(int argc, char const *argv[])
{
    OCTET *ImgIn, *ImgOut;

    if (argc != 4)
    {
        printf("usage:ImgIn.ppm ImgOut.ppm NamePalette.ppm\n");
        exit(1);
    }

  
    int w, h;
    // lire_nb_lignes_colonnes_image_pgm((char *)argv[1], &w, &h);
    lire_nb_lignes_colonnes_image_ppm((char *)argv[1], &w, &h);

    int ntaille = w * h;
    allocation_tableau(ImgIn, OCTET, ntaille * 3);
    allocation_tableau(ImgOut, OCTET, ntaille * 3);
    // lire_image_pgm((char *)argv[1], ImgIn, ntaille);
    lire_image_ppm((char *)argv[1], ImgIn, ntaille);

    vector<RGB> ImgInRGB = OCTETtoRGB(ImgIn, ntaille);
 OCTET *ImgPalette;
    vector<RGB> centroid2 = computeIcentroidAleaInImg(ImgInRGB, 256);
    allocation_tableau(ImgPalette, OCTET, centroid2.size() * 3);

    makePalette(centroid2, ImgPalette);
    kmean(centroid2, ImgInRGB, 0.1);

    for (size_t i = 0; i < ntaille; i++)
    {
        ImgOut[i * 3] = centroid2[(unsigned char)ImgInRGB[i].centroid].r;
        ImgOut[i * 3 + 1] = centroid2[(unsigned char)ImgInRGB[i].centroid].g;
        ImgOut[i * 3 + 2] = centroid2[(unsigned char)ImgInRGB[i].centroid].b;
    }

    // for (size_t i = 0; i < ntaille; i++)
    // {
    //     ImgOut[i * 3] = centroid2[(unsigned char)ImgInRGB[i].centroid].r;
    //     ImgOut[i * 3 + 1] = centroid2[(unsigned char)ImgInRGB[i].centroid].g;
    //     ImgOut[i * 3 + 2] = centroid2[(unsigned char)ImgInRGB[i].centroid].b;
    // }

    ecrire_image_ppm((char *)argv[2], ImgOut, w, h);
    double psnr = PSNR(ImgInRGB, OCTETtoRGB(ImgOut, ntaille));
    cout << psnr << endl;

   
    int wq = 16;
    int hq = 16;
    // for (size_t i = 0; i < 256; i++)
    // {
    //     cout << ImgPalette[i * 3] << " " << ImgPalette[i * 3 + 1] << " " << ImgPalette[i * 3 + 2] << endl;
    // }

    ecrire_image_ppm((char *)argv[3], ImgPalette, wq, hq);
    cout << "image crée" << endl;

    // g++ TP1.cpp -o tp1 && ./tp1 Nicoon.ppm Nicoon2Color.ppm 95 155 93 118 116 126^C

    return 0;
}
