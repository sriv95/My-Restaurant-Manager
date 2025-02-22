#include "nlohmann-json.hpp"
#include <string>

using json = nlohmann::json;

using namespace std;

void getData(json &,string);
void getAllData(json &);
void setData(json &,string);
void setAllData(json &);
int lenData(json &);
string updateFilePath(const string &);
bool checkData();
void newData();
string getPATH();
// void makecrash();
extern bool forcecloseTriggered;
