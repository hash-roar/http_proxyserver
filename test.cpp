
// #include "./log.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <vector>
using namespace std;
// #define out(x) std::cout<<x<<std::endl;



int main()
{
   YAML::Node config = YAML::LoadFile("config.yaml");
   YAML::Node http = config["http"];
   vector<YAML::Node> serevr_list;
   for (auto item : http)
   {
      serevr_list.push_back(item);
   }
   cout<<serevr_list[0]["server"]["listen"].Type()<<endl;
   return 0;
}

