#include <iostream>
#include <vector>
#include <amp.h>
 
using namespace concurrency;
using namespace std;
 
#define YESNO(value) (value ? "YES" : "NO")
 
int main() {
 
    wcout << "Listing avaliable C++ AMP accelerators" <<
        endl << endl;
 
    std::vector<accelerator> accelerators = accelerator::get_all();
 
    if (accelerators.empty()) {
        wcout << "No accelerators found!" << endl;
        return 0;
    }
 
    wcout << "Found: " << accelerators.size() << endl << endl;
 
    int n = 0;
    for_each(accelerators.cbegin(), accelerators.cend(), [&n](const accelerator& a) {
        wcout << ++n << ": "
            << a.description << " (" << a.version << ")" << endl
            << "           has display: "
            << YESNO(a.has_display) << endl
            << "           is emulated: "
            << YESNO(a.is_emulated) << endl
            << "      double precision: "
            << YESNO(a.supports_double_precision) << endl
            << " limited dbl precision: "
            << YESNO(a.supports_limited_double_precision) << endl
            << "           device path: "
            << a.device_path << endl
            << endl;
    });
 
    return 1;
}