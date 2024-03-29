## Prerequisites

```
sudo apt-get install build-essential
```

## Download
Run the following command to download SystemC 2.3.3 (Includes TLM)
Core SystemC Language and Examples (tar.gz)

```
wget https://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.tar.gz
```
Or Download the required version from the below link
https://www.accellera.org/downloads/standards/systemc

## Unzip Downloaded file
```
tar -xvf systemc-2.3.3.tar.gz
```
Note : Change the fil name if you have downloaded any other versions

## Creating Object Directory
Enter the Directory and create ``objdir``
```
cd systemc-2.3.3
mkdir objdir
cd objdir
```
## Creating folder within user's home directory
create folder within user's home directory
```
sudo mkdir /usr/local/systemc-2.3.3
```

## Installing
Run the following command from the objdir directory created above
```
sudo ../configure --prefix=/usr/local/systemc-2.3.3/
```
this should create the required make files.

Now run make 
```
sudo make
```

now install make command as shown below

```
sudo make install
```
## Add path to environment variables
```
sudo nano ~/.bashrc
```
add the following line to the end of file
```
export SYSTEMC_HOME=/usr/local/systemc-2.3.3/
```
Once done press Ctrl+X when prompted tosave save and exit.
close all the command prompt windows.
## Compile and Run
Open a new command propt in your project directory.
create simple helloworld module
```
nano hello.cpp
```
paste the below code and save
```
#include <systemc.h>

SC_MODULE (hello_world) {
  SC_CTOR (hello_world) {
  }

  void say_hello() {
    cout << "Hello World SystemC-2.3.3.\n";
  }
};

int sc_main(int argc, char* argv[]) {
  hello_world hello("HELLO");
  hello.say_hello();
  return(0);
}
```
Run the following command to compile
```
g++ -I. -I$SYSTEMC_HOME/include -L. -L$SYSTEMC_HOME/lib-linux64 -Wl,-rpath=$SYSTEMC_HOME/lib-linux64 -o hello hello.cpp -lsystemc -lm
```
Now you might see a `hello` file has been created.

Run the following command to Execute

```
./hello
```
