## Setting up the environment

The core functionality of the ACT platforms resides in a git repository that is
managed by senior ACT personnel. As an intern you will get access to this 
repository and it will provide guidance to you when you are solving tasks. 

The ACT platform has functions that handles decoding and encoding of binary EMP
messages. These functions may be used when solving tasks to reduce the
complexity and to focus your effort on the logic operations of the tasks. 

#### NOTE: This guide is for Mac and Linux users only.

 As the ACT platform is written on UNIX machines, we recommend Windows users to use some sort of virtual machine software and install a Linux operating system. A recommendation is using [VirtualBox](https://www.virtualbox.org/wiki/Downloads) together with [Ubuntu](http://www.ubuntu.com/download).

Even though we do not support it, it might be possible to compile the core-toolset with Visual Studios (VS) on Windows. However this requires some porting of system library header files and is not yet functionally tested by the ACT staff. 

### 1: Building the core toolset

The code-tree is built with CMake, which uses the provided CMakeLists.txt files in order to generate makefiles which in turn are used to compile the tree. Looking into those textfiles will hopefully give you an idea on how to add your solutions for compilation and to link them to the core library. However, this requires CMake to be installed on your machine.

#### 1.1 Installing CMake

**1.) Check if CMake is installed**

Open up a terminal and type:

```
$ cmake --version
cmake version 2.8.12.2
```

If CMake is not installed, you will receive a "command not found" error. Make sure that your CMake version is 2.6 or newer, otherwise the tree will not compile. 

**2.) Install Homebrew (Mac users)**

A neat way to install tools and programs is through Homebrew. If you already have Homebrew installed you can skip this step.

To check if you have brew installed, do the same as step 1.) although substituting "cmake" with "brew".

```
$ brew --version
Homebrew 0.9.9 (git revision 2f20; last commit 2016-05-15)
Homebrew/homebrew-core (git revision f259; last commit 2016-05-17)
```

You can install Homebrew via the terminal. Simply paste the following:

```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

If you are on a *Linux* machine, there is a similar tool called [Linuxbrew](http://linuxbrew.sh/), which is a fork of Homebrew. Follow the instructions given in the link provided.

**3.) Installing CMake**

With Homebrew installed, it is easy to install loads of utilities. To install CMake, input the following to your terminal:

```
$ brew install cmake
```

#### 1.2 Compiling the core-tree

With CMake installed, the code can now be compiled. Do the following in your terminal, make sure you are standing in the act directory:

```
$ cd act
#Jump to the correct directory

$ mkdir build
#Make a new directory called "build"

$ cd build
#Jump to that directory

$ cmake ..
$ cmake --build

```

Now the build directory will replica the directory structure of the act directory, although having different contents. You will see that the different directories in /build have a file called Makefile. This will be invoked to compile the build.

Make sure you are still standing in the top of the build directory, and not in any of the subdirectories. To compile enter the following:

```
$ make
#This will compile the build
```

If you want to clean the directory of executables, simply enter:

```
$ make clean
```

For further instructions on the make commands:

```
$ make help
```

If you are uncertain or wish to gain additional information regarding Makefiles and CMake, read up on content not provided by the ACT organization.


### 2: Running the build

With the build compiled, there will now exist executables that can be run. You can see sample solutions provided under the `/act/solutions/` directory (and the executables in `/act/build/solutions/`). There is also a "tools" directory which illustrates the funcitonality of the core decoder-encoder provided. Having a look at those might give some instructions on how they can be run (looking at the main function will give an intuition of what flags that can be passed). An example will be given on how to run an executable properly below. The example will utilize the secho.cc tool as it simply echoes the input messages, giving decoder-encoder intuition. 

With that said, the core-program will therefor need input. In the simplified trading platform that ACT provides, there is only two streams of data: Standard Input (std::cin) and Standard Output (std::cout). Thus, these streams will handle input and output for both Client and Exchange (ESE) messages.

Now with some description at hand, let's go through how to run the program.

Standing in the `act/build/tools/` directory:

There are two flags that can be passed, indicating if input and output should be text or binary.
```
$ ./secho t t
# Will run the program expecting EMP-messages in text format, and sending messages in text format

$ ./secho b b
# Will run the program expecting EMP-messages in binary (native), and sending messages in binary.

$ ./secho t b
# Will expect input in text format, and send messages in binary

$ ./secho b t
# Will expect input in binary, and send messages in text format
```
As you might have guessed, the flags `(b|t)` declare what format should be decoded (input) and how messages should be encoded (output). It is of course possible to build a main-function that only take one argument which will have uniform input and output protocol encoding. That is up to you as an intern to decide.

For illustrating purposes we will proceed with human readable format, text.

Providing input to the program can be done in different ways. Here are some examples:

```
# Simply running the program and providing input by hand
$ ./secho t t
Decoder: Text...
2|0|0|101|ERIC B
# InstrumentInfo message sent from ESE to the ACT-service.
2|1|0|101|ERIC B
# InstrumentInfo message sent from ACT to the client. Notice how the system keeps adding to MsgSeqno.
```

```
# Providing input from file
$ cat input.txt | ./secho t t
Decoder: Text...
4|1|0|11|100|1|1|ECX #IndexInfo message sent from ESE to ACT is decoded and then sent (encoded) to client
5|2|0|11|101|100000|100000|75|0|0|1 #IndexMemberInfo (same as above)
6|3|0|11|117.25 #IndexUpdate (same as above)
```

```
# Writing output to file
cat input.txt | ./secho t t > output.txt
```

Now lets look at the contents of input.txt and output.txt:

input.txt:
```
4|0|0|11|100|1|1|ECX
5|0|0|11|101|100000|100000|75|0|0|1
6|0|0|11|117.25
```

output.txt:
```
4|1|0|11|100|1|1|ECX
5|2|0|11|101|100000|100000|75|0|0|1
6|3|0|11|117.25
# Notice the difference in sequence numbers.
```

The pipe '|' is a command which forwards the output/contents of a program/file to something else. This can be used in order to forward output from one program as input to another. This is a suitable way to test the correctness of the implementations you will make as an intern. 

Testing that the text to binary decoder-encoder, and binary to text decoder-encoder is working properly:

```
cat input.txt | ./secho t b | ./secho b t
# This output should be equal the contents of output.txt
Decoder: Text...
Decoder: Binary...
4|1|0|11|100|1|1|ECX
5|2|0|11|101|100000|100000|75|0|0|1
6|3|0|11|117.25
```
As we can see, the output is identical to output.txt.

The different tasks you will attempt provides input examples in text format, which can be used to further test the functionality of the decoder-encoder. The [specification](/spec) also provides text encoding examples of the Epsilon Message Protocol. Make sure to properly review the EMP specification before/whilst solving tasks, in order to understand how the messages should be interpreted and also figure out how the internal (decoded) representation is made in the ACT-platform. 

Hopefully this is enough information for you to start your progress as an intern at ACT. If you encounter any difficulties, do not hesitate to contact [ACT support](mailto:act-support@pantor.com) via email.





