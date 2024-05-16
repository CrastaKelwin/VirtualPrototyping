#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using namespace tlm;
using namespace tlm_utils;

// Memory module representing a simple memory
struct Memory : sc_module
{
    // TLM-2 target socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_target_socket<Memory> mem_target_socket;

    enum
    {
        SIZE = 1024
    };

    // Memory array
    unsigned char *mem;

    SC_CTOR(Memory) : mem_target_socket("mem_target_socket")
    {
        mem_target_socket.register_b_transport(this, &Memory::b_transport);
        mem_target_socket.register_transport_dbg(this, &Memory::transport_dbg);
        mem_target_socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);

        // Allocate memory
        mem = new unsigned char[SIZE];
    }

    // TLM-2 blocking transport method
    virtual void b_transport(tlm_generic_payload &trans, sc_time &delay)
    {
        tlm_command cmd = trans.get_command();
        sc_dt::uint64 adr = trans.get_address();
        unsigned char *ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        unsigned char *byt = trans.get_byte_enable_ptr();
        unsigned int wid = trans.get_streaming_width();

        // Check address range and unsupported features
        if (adr >= sc_dt::uint64(SIZE) || byt != 0 || len > 4 || wid < len)
            SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

        // Implement read and write commands
        if (cmd == TLM_READ_COMMAND)
        {
            memcpy(ptr, &mem[adr], len);
        }
        else if (cmd == TLM_WRITE_COMMAND)
        {
            memcpy(&mem[adr], ptr, len);
        }

        // Set response status
        trans.set_response_status(TLM_OK_RESPONSE);
    }

    // TLM-2 debug transport method
    virtual unsigned int transport_dbg(tlm_generic_payload &trans)
    {
        tlm_command cmd = trans.get_command();
        sc_dt::uint64 adr = trans.get_address();
        unsigned char *ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        unsigned char *byt = trans.get_byte_enable_ptr();
        unsigned int wid = trans.get_streaming_width();

        // Check address range and unsupported features
        if (adr >= sc_dt::uint64(SIZE) || byt != 0 || len > 4 || wid < len)
            SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");


        printf(" address = 0x%llX data 0x%X\n",adr,*ptr);
        // Implement read and write commands
        if (cmd == TLM_READ_COMMAND)
            memcpy(ptr, &mem[adr], len);
        else if (cmd == TLM_WRITE_COMMAND)
            memcpy(&mem[adr], ptr, len);

        // Set response status
        trans.set_response_status(TLM_OK_RESPONSE);

        return len;
    }

    // TLM-2 DMI method
    virtual bool get_direct_mem_ptr(tlm_generic_payload &trans, tlm_dmi &dmi_data)
    {
        dmi_data.allow_read_write();
        dmi_data.set_dmi_ptr(reinterpret_cast<unsigned char *>(mem));
        dmi_data.set_start_address(0);
        dmi_data.set_end_address(SIZE * 4 - 1);
        dmi_data.set_read_latency(SC_ZERO_TIME);
        dmi_data.set_write_latency(SC_ZERO_TIME);
        return true;
    }

    // Destructor
    ~Memory()
    {
        // Deallocate memory
        delete[] mem;
    }
};

// Testbench module
SC_MODULE(Testbench)
{
    // TLM-2 initiator socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<Testbench> tb_initiator_socket;

    SC_CTOR(Testbench)
        : tb_initiator_socket("tb_initiator_socket")
    {
        SC_THREAD(run);
    }

    void run()
    {
        // Buffers for data comparison
        unsigned char buff1[Memory::SIZE];
        unsigned char buff2[Memory::SIZE];

        for (int i = 0; i < Memory::SIZE; i++)
        {
            buff1[i] = i;
        }
        // Write data from buff1 to memory
        for (int i = 0; i < Memory::SIZE; i++)
        {
            tlm_generic_payload trans;
            trans.set_command(TLM_WRITE_COMMAND);
            trans.set_address(i);
            trans.set_data_ptr(reinterpret_cast<unsigned char *>(&buff1[i]));
            trans.set_data_length(1);
            trans.set_streaming_width(1);
            trans.set_byte_enable_ptr(0);
            trans.set_dmi_allowed(false);
            trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

            sc_time delay = sc_time(10, SC_NS);
            tb_initiator_socket->b_transport(trans, delay);

            if (trans.is_response_error())
                SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
        }

        // Compare buff1 and buff2
        bool match = true;
        // Read data from memory to buff2
        for (int i = 0; i < Memory::SIZE; i++)
        {
            tlm_generic_payload trans;
            trans.set_command(TLM_READ_COMMAND);
            trans.set_address(i);
            trans.set_data_ptr(reinterpret_cast<unsigned char *>(&buff2[i]));
            trans.set_data_length(1);
            trans.set_streaming_width(1);
            trans.set_byte_enable_ptr(0);
            trans.set_dmi_allowed(true);
            trans.set_response_status(TLM_INCOMPLETE_RESPONSE);

            sc_time delay = sc_time(10, SC_NS);
            tb_initiator_socket->b_transport(trans, delay);
            if (buff1[i] != buff2[i])
            {
                printf("Error address = 0x%X data 0x%X 0x%X\n", i, buff1[i], buff2[i]);
                match = false;
                break;
            }

            if (trans.is_response_error())
                SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
        }

        // Print result
        if (match)
            cout << "Memory content matches!" << endl;
        else
            cout << "Memory content does not match!" << endl;
    }
};

int sc_main(int argc, char *argv[])
{
    // Instantiate Memory module and Testbench
    Memory memory("memory");
    Testbench testbench("testbench");

    // Connect testbench to memory
    testbench.tb_initiator_socket.bind(memory.mem_target_socket);

    // Start simulation
    sc_start();

    return 0;
}
