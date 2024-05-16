#include <systemc.h>
#include <fstream>

SC_MODULE(Memory)
{
    // Ports
    sc_in<sc_uint<32>> address;
    sc_in<bool> read_or_write_en;
    sc_in<sc_uint<32>> data_to_mem;
    sc_out<sc_uint<32>> data_from_mem;

    // Memory size (1 KB)
    static const int MEMORY_SIZE = 1024;

    // Memory array
    unsigned int *mem;

    // Constructor
    SC_CTOR(Memory)
    {
        // Allocate memory
        mem = new unsigned int[MEMORY_SIZE];

        // Initialize memory
        for (int i = 0; i < MEMORY_SIZE; ++i)
        {
            mem[i] = 0;
        }

        // Process to handle read and write operations
        SC_METHOD(memoryOperation);
        sensitive << read_or_write_en << address;
    }

    // Destructor
    ~Memory()
    {
        // Deallocate memory
        delete[] mem;
    }

    // Memory read and write operation
    void memoryOperation()
    {
        if (read_or_write_en.read() == 0)
        { // Read operation
            data_from_mem = mem[address.read()];
        cout<<"Read "<<data_from_mem.read() <<" to "<<address.read()<<endl;
        }
        else
        { // Write operation
        cout<<"written "<<data_to_mem.read() <<" to "<<address.read()<<endl;
            mem[address.read()] = data_to_mem.read();
        }
    }
};

SC_MODULE(Memory_TB)
{
    // Ports
    sc_out<sc_uint<32>> address;
    sc_out<bool> read_or_write_en;
    sc_out<sc_uint<32>> data_to_mem;
    sc_in<sc_uint<32>> data_from_mem;

    // Memory size (1 KB)
    static const int MEMORY_SIZE = 8;

    // Buffers for data comparison
    unsigned int buff1[MEMORY_SIZE];
    unsigned int buff2[MEMORY_SIZE];

    void test()
    {

        for (int i = 0; i < MEMORY_SIZE; ++i)
        {
            buff1[i] = i;
        }

        // Write data from buff1 to memory
        for (int i = 0; i < MEMORY_SIZE; ++i)
        {
            address.write(i);
            data_to_mem.write(buff1[i]);
            read_or_write_en.write(true); // Write enable
            wait(1, SC_NS);           // Allow process to execute
        }

        // Read data from memory to buff2 and compare with buff1
        for (int i = 0; i < MEMORY_SIZE; ++i)
        {
            address.write(i);
            read_or_write_en.write(false); // Read enable
            wait(1, SC_NS);           // Allow process to execute
            buff2[i] = data_from_mem.read();
        }

        bool match = true;
        for (int i = 0; i < MEMORY_SIZE; ++i)
        {
            if (buff1[i] != buff2[i])
            {
                match = false;
                break;
            }
        }

        // Print result
        if (match)
        {
            cout << "Memory content matches!" << endl;
        }
        else
        {
            cout << "Memory content does not match!" << endl;
        }
    }
    // Constructor
    SC_CTOR(Memory_TB)
    {
        // Write data to buff1
        SC_THREAD(test);
    }
};

int sc_main(int argc, char *argv[])
{

    sc_signal<sc_uint<32>> address;
    sc_signal<bool> read_or_write_en;
    sc_signal<sc_uint<32>> data_from_mem, data_to_mem;

    Memory mem("memory");
    // Ports
    mem.address(address);
    mem.read_or_write_en(read_or_write_en);
    mem.data_from_mem(data_from_mem);
    mem.data_to_mem(data_to_mem);

    // Instantiate Memory_TB module
    Memory_TB tb("tb");
    tb.address(address);
    tb.read_or_write_en(read_or_write_en);
    tb.data_from_mem(data_from_mem);
    tb.data_to_mem(data_to_mem);

    sc_start();
    return 0;
}
