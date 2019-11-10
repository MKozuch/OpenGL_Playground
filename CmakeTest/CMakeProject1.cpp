#include "CMakeProject1.h"
#include "TutorialConfig.h"

using namespace std;

void printVer();

int main(int argc, char* argv[])
{
   for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-v") == 0) {
         printVer();
         break;
      }
   }

	return 0;
}

void printVer()
{
   cout << "Hello CMake." << endl;
   cout << OUTPUT_NAME << " Version " << Tutorial_VERSION_MAJOR << "." << Tutorial_VERSION_MINOR << endl;

#ifdef FOO_ENABLE
   std::cout << FOO_STRING << endl;
#endif
}
