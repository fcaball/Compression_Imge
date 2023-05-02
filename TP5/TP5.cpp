#include <stdio.h>
#include <fstream>
#include <float.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include <map>

using namespace std;

vector<float> vertices;
vector<int> triangles;

void split(string delimiter, string s, vector<string> &output)
{
  size_t pos = 0;
  std::string token;

  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    if (token != "")
      output.push_back(token);
    s.erase(0, pos + delimiter.length());
  }

  if (token != "")
  {
    split(delimiter, s, output);
  }
  else
  {
    if (s != "")
    {
      output.push_back(s);
    }
  }
}

int findKeyWithValue(map<float, int> M, int value)
{
  for (map<float, int>::iterator it = M.begin(); it != M.end(); it++)
    if (it->second == value)
      return it->first; // ici tu as ta string
}

std::vector<vector<float>> calculBoiteEnglobante()
{
  vector<float> min;
  min.push_back(FLT_MAX);
  min.push_back(FLT_MAX);
  min.push_back(FLT_MAX);
  vector<float> max;
  max.push_back(FLT_MIN);
  max.push_back(FLT_MIN);
  max.push_back(FLT_MIN);

  for (size_t i = 0; i < vertices.size(); i += 3)
  {

    if (vertices[i] < min[0])
    {
      min[0] = vertices[i];
    }

    if (vertices[i] > max[0])
    {
      max[0] = vertices[i];
    }

    if (vertices[i + 1] < min[1])
    {
      min[1] = vertices[i + 1];
    }
    if (vertices[i + 1] > max[1])
    {
      max[1] = vertices[i + 1];
    }

    if (vertices[i + 2] < min[2])
    {
      min[2] = vertices[i + 2];
    }
    if (vertices[i + 2] > max[2])
    {
      max[2] = vertices[i + 2];
    }
  }
  min[0] -= 0.1;
  min[1] -= 0.1;
  min[2] -= 0.1;

  max[0] += 0.1;
  max[1] += 0.1;
  max[2] += 0.1;

  float range = FLT_MIN;

  if (range < (max[1] - min[1]))
  {
    range = (max[1] - min[1]);
  }

  if (range < (max[0] - min[0]))
  {
    range = (max[0] - min[0]);
  }

  if (range < (max[2] - min[2]))
  {
    range = (max[2] - min[2]);
  }

  // cout << "BBmin : " << min[0] << " " << min[1] << " " << min[2] << endl;
  // cout << "BBmax : " << max[0] << " " << max[1] << " " << max[2] << endl;

  max.clear();
  max.push_back(range);

  std::vector<vector<float>> BoiteEnglobante;

  BoiteEnglobante.clear();
  BoiteEnglobante.push_back(min);
  BoiteEnglobante.push_back(max);

  return BoiteEnglobante;
}

int main(int argc, char *argv[])
{
  ifstream fichier((char *)argv[1], ios::in);
  string verif;
  std::getline(fichier, verif);
  if (verif == "OFF")
  {
    perror("Le fichier donné en paramètre n'est pas un fichier OFF");
  }

  string infos;
  std::getline(fichier, infos);
  vector<string> i;
  split(" ", infos, i);
  int nbVertices = stoi(i[0]);
  int nbTriangles = stoi(i[1]);

  // cout << nbVertices << " " << nbTriangles << endl;
  string lignevide;
  std::getline(fichier, lignevide);
  for (size_t i = 0; i < nbVertices; i++)
  {
    string line = "";
    std::getline(fichier, line);
    vector<string> coords;
    split(" ", line, coords);

    vertices.push_back(stof(coords[0]));
    vertices.push_back(stof(coords[1]));
    vertices.push_back(stof(coords[2]));
  }

  for (size_t i = 0; i < nbTriangles; i++)
  {
    string line;
    std::getline(fichier, line);
    vector<string> coords;
    split(" ", line, coords);
    if (coords[0] == "3")
    {
      triangles.push_back(stof(coords[1]));
      triangles.push_back(stof(coords[2]));
      triangles.push_back(stof(coords[3]));
    }
  }

  vector<vector<float>> boitenEglobante = calculBoiteEnglobante();

  vector<float> BBmin = boitenEglobante[0];
  float range = boitenEglobante[1][0];

  // cout << "range : " << range << endl;
  ofstream dataRMSE((char *)"RMSE.dat", std::ios::out);

  for (int Q = 5; Q < 30; Q++)
  {
    vector<int> verticesQuantified;
    for (size_t i = 0; i < vertices.size(); i += 3)
    {
      int cprime = (vertices[i] - BBmin[0]) * pow(2, 3 /* Q */) / range;
      verticesQuantified.push_back(cprime);

      int cprime1 = (vertices[i + 1] - BBmin[1]) * pow(2, 3 /* Q */) / range;
      verticesQuantified.push_back(cprime1);

      int cprime2 = (vertices[i + 2] - BBmin[2]) * pow(2, 3 /* Q */) / range;
      verticesQuantified.push_back(cprime2);
    }

    vector<float> verticesDeQuantified;

    for (size_t i = 0; i < verticesQuantified.size(); i += 3)
    {
      float c = (float)verticesQuantified[i] * range / pow(2, Q) + BBmin[0];
      verticesDeQuantified.push_back(c);

      float c1 = (float)verticesQuantified[i + 1] * range / pow(2, Q) + BBmin[1];
      verticesDeQuantified.push_back(c1);

      float c2 = (float)verticesQuantified[i + 2] * range / pow(2, Q) + BBmin[2];
      verticesDeQuantified.push_back(c2);
    }

    // ofstream fichierOut((char *)"newbunnyPQ3.off", ios::out);
    // fichierOut << "OFF" << endl;
    // fichierOut << nbVertices << " " << nbTriangles << " 0" << endl;
    // fichierOut << endl;
    // for (size_t i = 0; i < verticesDeQuantified.size(); i += 3)
    // {
    //   fichierOut << verticesDeQuantified[i] << " " << verticesDeQuantified[i + 1] << " " << verticesDeQuantified[i + 2] << endl;
    // }

    // for (size_t i = 0; i < triangles.size(); i += 3)
    // {
    //   fichierOut << "3 " << triangles[i] << " " << triangles[i + 1] << " " << triangles[i + 2] << endl;
    // }

    float RMSE;

    for (size_t i = 0; i < nbVertices; i++)
    {
      RMSE += (vertices[i * 3] - verticesDeQuantified[i * 3]) * (vertices[i * 3] - verticesDeQuantified[i * 3]);
      RMSE += (vertices[i * 3 + 1] - verticesDeQuantified[i * 3 + 1]) * (vertices[i * 3 + 1] - verticesDeQuantified[i * 3 + 1]);
      RMSE += (vertices[i * 3 + 2] - verticesDeQuantified[i * 3 + 2]) * (vertices[i * 3 + 2] - verticesDeQuantified[i * 3 + 2]);
    }
    RMSE /= nbVertices;
    RMSE = sqrt(RMSE);
    dataRMSE << Q << " " << RMSE << endl;
  

  // Alphabet = {0, 1, 2};

  vector<float> Sequence;
  for (size_t i = 0; i < 5; i++)
  {
    Sequence.push_back(triangles[i]);
    Sequence.push_back(triangles[i * 3 + 1]);
    Sequence.push_back(triangles[i * 3 + 2]);
  }
  // Sequence = {0, 1, 0, 2, 2, 0};

  for (size_t i = 0; i < Sequence.size(); i++)
  {
    cout << Sequence[i] << " ";
  }
  cout << endl;
  cout << endl;
  cout << endl;
  cout << endl;

  map<float, int> Alphabet;
  int k = 0;
  for (size_t i = 0; i < Sequence.size(); i++)
  {
    if (Alphabet.find(Sequence[i]) == Alphabet.end())
    {

      Alphabet[Sequence[i]] = k;
      k++;
    }
  }

  // for (const auto &[key, value] : Alphabet)
  // {
  //   cout << key << " ";
  // }
  // cout << endl;
  // cout << endl;
  // cout << endl;

  vector<int> Frequence;
  Frequence.resize(Alphabet.size());
  for (size_t i = 0; i < Sequence.size(); i++)
  {

    Frequence[Alphabet[Sequence[i]]]++;
  }

  int M = Sequence.size();
  vector<int> Cummulative;
  Cummulative.resize(Frequence.size());
  Cummulative[0] = 0;
  for (size_t j = 1; j < Frequence.size(); j++)
  {
    Cummulative[j] = Cummulative[j - 1] + Frequence[j - 1];
  }

  // for (size_t i = 0; i < Frequence.size(); i++)
  // {
  //   cout << Frequence[i] << " ";
  // }

  // cout << endl;

  // for (size_t i = 0; i < Cummulative.size(); i++)
  // {
  //   cout << Cummulative[i] << " ";
  // }
  // cout << endl;

  vector<long int> Xts;
  Xts.push_back(0);

  for (size_t i = 1; i <= Sequence.size(); i++)
  {
    long int Xt = floor(Xts[i - 1] / Frequence[Alphabet[Sequence[i - 1]]]) * M + Cummulative[Alphabet[Sequence[i - 1]]] + (Xts[i - 1] % Frequence[Alphabet[Sequence[i - 1]]]);
    Xts.push_back(Xt);
  }

  // for (size_t i = 0; i < Xts.size(); i++)
  // {
  //   cout << Xts[i] << " ";
  // }
  // cout << endl;
  // cout << endl;
  // cout << endl;

  vector<int> SequenceRetruvee;

  // for (size_t i = M; i > 0; i--)
  // {
  //   int m = Xts[i] % M;
  //   // cout << "xt " << Xts[i] << endl;
  //   // cout << "m " << m << endl;
  //   int k = 0;
  //   while (m >= Cummulative[k])
  //   {
  //     if (k >= Cummulative.size() - 1)
  //     {
  //       k = Cummulative.size();
  //       break;
  //     }
  //     k++;
  //   }
  //   // cout<<k-1<<endl;

  //   SequenceRetruvee.push_back(findKeyWithValue(Alphabet, k - 1));
  // }

  // for (int i = SequenceRetruvee.size() - 1; i >= 0; i--)
  // {
  //   cout << SequenceRetruvee[i] << " ";
  // }

  // cout << endl;

  double nbbit = log2((double)Xts[Xts.size() - 1]);
  // cout<<Xts[Xts.size()-1]<<endl;
  // cout << Q << " " << nbbit / (double)Sequence.size() << endl;
}
// fichierOut << endl;
// fichierOut.close();

return 0;
}
