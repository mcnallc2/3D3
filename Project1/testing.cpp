#include <string>
#include <thread>
#include <iostream>

using namespace std;

int
main(int argc, char *argv[])
{
    string input = argv[1];
    string output = "my name is " + input + "\n";
    cout << output;

  return 0;
}