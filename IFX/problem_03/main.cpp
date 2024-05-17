#include <iostream>
#include <cassert>
#include <initializer_list>

using namespace std;
template <typename T, size_t SIZE>
class array
{
private:
    T elements[SIZE];

public:
    // Default constructor
    array() {}
    // Constructor with initializer list
    array(std::initializer_list<T> values)
    {
        assert(values.size() <= SIZE);
        size_t i = 0;
        for (const auto &value : values)
        {
            elements[i++] = value;
        }
    }

    // Subscript operator overloading
    T &operator[](size_t index)
    {
        assert(index < SIZE);
        return elements[index];
    }

    // Addition operator overloading
    template <size_t SIZE2>
    array<T, SIZE + SIZE2> operator+(const array<T, SIZE2> &other) const
    {
        array<T, SIZE + SIZE2> result;
        for (size_t i = 0; i < SIZE; ++i)
        {
            result[i] = elements[i];
        }
        for (size_t i = 0; i < SIZE2; ++i)
        {
            result[SIZE + i] = other.elements[i];
        }
        return result;
    }

    // Equality operator overloading
    bool operator==(const array<T, SIZE> &other) const
    {
        for (size_t i = 0; i < SIZE; ++i)
        {
            if (elements[i] != other.elements[i])
            {
                return false;
            }
        }
        return true;
    }

    // Size function
    size_t size() const
    {
        return SIZE;
    }
    // Method to print the elements of the array
    void print() const
    {
        std::cout << "[ ";
        for (size_t i = 0; i < SIZE; ++i)
        {
            std::cout << elements[i];
            if (i < SIZE - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << " ]" << std::endl;
    }
};

int main()
{
    array<uint32_t, 5> arr1 = {1, 2, 3, 4, 5};

    cout << "arr1" << endl;
    arr1.print();
    arr1[0] = 0;

    assert(arr1[0] == 0);
    cout << "arr1" << endl;
    arr1.print();
    array<uint32_t, 5> arr2 = {6, 7, 8, 9, 10};
    cout << "arr2" << endl;
    arr2.print();
    array<uint32_t, 10> arr3 = arr1 + arr2;
    cout << "arr3 = arr1+arr2" << endl;
    arr3.print();

    assert(arr3.size() == 10);
    assert((arr3 == array<uint32_t, 10>{0, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

    array<uint32_t, 5> arr4;
    arr4 = arr2 = arr1;
    cout << "arr4= arr2 = arr1" << endl;
    cout << "arr4" << endl;
    arr4.print();
    arr2.print();
    cout << "arr1" << endl;
    arr1.print();
    assert((arr4 == array<uint32_t, 5>{0, 2, 3, 4, 5}));
    assert((arr2 == array<uint32_t, 5>{0, 2, 3, 4, 5}));
    return 0;
}
