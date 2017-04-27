
#include "Material.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;

bool Material::ReadMaterial(const char *filename) {


  bool debug = false;
  char current_material[20];
  std::ifstream ifs(filename);
  std::string line;

  int count = 0;
  while (std::getline(ifs, line))
  {

    std::istringstream ss(line);
    ss.clear();

    Entry e;

    if (line.compare(0, 6, "newmtl") == 0) //This is a vertex
    {
      ss.ignore(6);

      std::istream_iterator<char> it(ss);
      std::istream_iterator<char> end;
      std::string results(it, end);
      e = Entry();
      data.push_back(e);
      materialNameMap[results] = count++;
      cout << results << ":" << count-1 << endl;

    }

    if (line.compare(0, 2, "Ka") == 0) //This is a vertex
    {
      ss.ignore(2);
      glm::vec3 diff = glm::vec3();
      for(int i=0; i<3; i++) {
        ss >> diff[i];
      }
      cout << diff[0] << "," << diff[1] << "," << diff[2]<< endl;
      data[count-1].Ka = diff;
    }

    if (line.compare(0, 2, "Kd") == 0) //This is a vertex
    {
      ss.ignore(2);
      glm::vec3 diff = glm::vec3();
      for(int i=0; i<3; i++) {
        ss >> diff[i];
      }
      cout << diff[0] << "," << diff[1] << "," << diff[2]<< endl;
      data[count-1].Kd = diff;
    }

    if (line.compare(0, 2, "Ke") == 0) //This is a vertex
    {
      ss.ignore(2);
      glm::vec3 diff = glm::vec3();
      for(int i=0; i<3; i++) {
        ss >> diff[i];
      }
      cout << diff[0] << "," << diff[1] << "," << diff[2]<< endl;
      data[count-1].Ke = diff;
    }

    if (line.compare(0, 2, "Ks") == 0) //This is a vertex
    {
      ss.ignore(2);
      glm::vec3 diff = glm::vec3();
      for(int i=0; i<3; i++) {
        ss >> diff[i];
      }
      cout << diff[0] << "," << diff[1] << "," << diff[2]<< endl;
      data[count-1].Ks = diff;
    }


  }

}


int Material::MaterialLookup(std::string s) {
  return materialNameMap[s];
}
