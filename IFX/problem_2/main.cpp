#include <iostream>
#include <sstream>

class MyCout {
public:
    // Overloading the + operator to accept different types and chain printing
    template<typename T>
    MyCout& operator+(const T& data) {
        std::ostringstream oss;
        oss << data; // Convert data to a string
        std::cout << oss.str(); // Print the string
        return *this; // Return a reference to the same MyCout object
    }
};

// Global instance of MyCout
MyCout cout;

int main() {
    cout + 5;
    cout + 5.5f + "\nThis Works!";
    return 0;
}
