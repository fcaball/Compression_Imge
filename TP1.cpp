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
    for (size_t i = 0; i < ntaille*3; i += 3)
    {
        result.push_back({ImgIn[i], ImgIn[i + 1], ImgIn[i + 2]});
    }
    return result;
}

float distance(RGB a, RGB b)
{
    return sqrt(((int)a.r - (int)b.r) * ((int)a.r - (int)b.r) + ((int)a.g - (int)b.g) * ((int)a.g - (int)b.g) + ((int)a.b - (int)b.b) * ((int)a.b - (int)b.b));
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
            if (distance(ImgIn[i],Cs[min]) > d)
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
        int r=0,g=0,b=0;
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
            r = r/nbPix;
            g = g/nbPix;
            b = b/nbPix;
            Cs[i]={ (unsigned int) r,(unsigned int) g, (unsigned int) b};
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

    if (argc != 9)
    {
        printf("usage:ImgIn.ppm ImgOut.ppm R1 G1 B1 R2 G2 B2");
        exit(1);
    }
    int R1, G1, B1, R2, G2, B2;
    R1 = atoi(argv[3]);
    G1 = atoi(argv[4]);
    B1 = atoi(argv[5]);
    R2 = atoi(argv[6]);
    G2 = atoi(argv[7]);
    B2 = atoi(argv[8]);

    RGB C1 = {R1, G1, B1};
    RGB C2 = {R2, G2, B2};
    int w, h;
    // lire_nb_lignes_colonnes_image_pgm((char *)argv[1], &w, &h);
    lire_nb_lignes_colonnes_image_ppm((char *)argv[1], &w, &h);

    int ntaille = w * h;
    allocation_tableau(ImgIn, OCTET, ntaille * 3);
    allocation_tableau(ImgOut, OCTET, ntaille * 3);
    // lire_image_pgm((char *)argv[1], ImgIn, ntaille);
    lire_image_ppm((char *)argv[1], ImgIn, ntaille);

    vector<RGB> ImgInRGB = OCTETtoRGB(ImgIn, ntaille);

    vector<RGB> centroid2 = {C1, C2};
    kmean(centroid2, ImgInRGB, 0.1);

    for (size_t i = 0; i < ntaille; i ++)
    {
        ImgOut[i*3] = (unsigned char) centroid2[ImgInRGB[i].centroid].r;
        ImgOut[i*3 + 1] =(unsigned char)  centroid2[ImgInRGB[i].centroid].g;
        ImgOut[i*3 + 2] = (unsigned char) centroid2[ImgInRGB[i].centroid].b;
    }

    ecrire_image_ppm((char *)argv[2], ImgOut, w, h);
    cout<<"image crée"<<endl; 

    // g++ TP1.cpp -o tp1 && ./tp1 Nicoon.ppm Nicoon2Color.ppm 95 155 93 118 116 126^C

    return 0;
}
