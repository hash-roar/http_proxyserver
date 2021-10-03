
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
   YAML::Node server = http[0]["server"];
   cout<<server<<endl;
   YAML::Node location = server["locations"][0]["location"];
   cout<<location<<endl;

   vector<YAML::Node> serevr_list;
   for (auto item : http)
   {
      serevr_list.push_back(item);
   }
   cout<<serevr_list[0]["server"]["listen"].Type()<<endl;
   if (serevr_list[0]["server"]["ten"])
   {
      cout<<"zhaodao"<<endl;
   }
   YAML::Node nodes = YAML::Load("a:haha");
   // cout<<info["a"];
   
   return 0;
}

