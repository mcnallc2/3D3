#include <string>
#include <thread>
#include <iostream>

using namespace std;

int
main()
{

  string str = "12345";
  int num = atoi(str.c_str());
  cout << endl << "number is " << num << endl;

  return 0;
}