# ACT Developer Guide

The core functionality of the ACT platforms resides in this git repository that is
managed by senior ACT personnel. As an intern you have access to this 
repository and it will provide guidance to you when you are solving tasks. 

The ACT platform has functions that handles decoding and encoding of binary EMP
messages. These functions may be used when solving tasks to reduce the
complexity and to focus your effort on the logic operations of the tasks. 

As an intern you are encouraged to fork this repository. Once you have completed any of the tasks you are welcome to send the link to your forked repository to [act-support](mailto:act-support@pantor.com).

## Setting up the development environment

The ACT software can be built on MAC OS and Linux, we recommend Windows users to use a virtual machine software and install a Linux operating system. A recommendation is using [VirtualBox](https://www.virtualbox.org/wiki/Downloads) together with [Ubuntu](http://www.ubuntu.com/download).
 
To install VirtualBox, the process is nearly identical on Windows, Linux and Mac. Since VirtualBox, or another VM software will be necessary for Windows, we recommend following this guide:
http://www.psychocats.net/ubuntu/virtualbox

And we recommend using Ubuntu version 16.04.1 LTS as a disk image for VirtualBox: 
http://www.ubuntu.com/download/desktop

Other versions of Linux might lack dependencies that our tools require, hence we recommend sticking to this guide to streamline the procedure.

#### Installing CMake

Building the code-tree will require the program CMake.

**Check if CMake is installed**

Open up a terminal and type:

```
$ cmake --version
cmake version 2.8.12.2
```

If CMake is not installed, you will receive a "command not found" error. Make sure that your CMake version is 2.6 or newer, otherwise the tree will not compile. 


**Mac:** 

A neat way to install tools and programs is through Homebrew. If you already have Homebrew installed you can skip this step.

To check if you have brew installed, do the version check again, although substitute "cmake" with "brew".

```
$ brew --version
Homebrew 0.9.9 (git revision 2f20; last commit 2016-05-15)
Homebrew/homebrew-core (git revision f259; last commit 2016-05-17)
```

You can install Homebrew via the terminal. Simply paste the following:

```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

With Homebrew installed, it is easy to install loads of utilities. To install CMake, input the following to your terminal:

```
$ brew install cmake
```

**Ubuntu:** simply input into terminal:

```
$ sudo apt-get install cmake
```

Now you have cmake installed!

### 1: Cloning the ACT repository

GitHub offers two ways to clone directories via the terminal; either via an HTTPS-link or via an SSH-link to the repository. We recommend you to use the latter, although this requires some setup in the form of generating an SSH-key, adding it to your ssh-agent and to your GitHub account. 

#### 1.1: Cloning with HTTPS:

Open up a terminal and jump to a directory where you want to store the repository. 

```
$ cd Documents/
```

Then simply clone the repository as follows:

```
$ git clone https://github.com/pantor-engineering/act.git
```

You may be prompted to enter your GitHub username and password, after which you can enter the repository locally:

```
$ cd act
```
 
#### 1.2 Cloning with SSH (Recommended):

As mentioned above, you need an SSH key linked to your GitHub account (and preferably added to your ssh-agent) in order to clone and alter the repository via SSH. If you already have an SSH key, you can link this to your GitHub account instead. 

 **1.) Check for existing SSH-keys:**

Open a Terminal and input the following to list your existing SSH keys. If you do not have any keys you will get no response.

```
$ ls -al ~/.ssh
#Lists the files in your .ssh directory, if they exist.
```

Notice that public ssh keys are by default named one of the following:

```
> id_rsa.pub
> id_dsa.pub
> id_ecdsa.pub
> id_ed25519.pub 
```

 **2.) Generating a new SSH key**

Open a Terminal and enter the text below, substituting the email with your GitHub email address. If it prompts you to overwrite an existing file, this means you already have a generated ssh-key with the same name. You should not overwrite this if you are using it for other services.

```
$ ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
#Creates a new ssh key, using the provided email as a label
Generating public/private rsa key pair.
```

You will be prompted to enter a path to a file in which to save the key. By pressing Enter you accept the default file location (recommended)

```
Enter a file in which to save the key (/Users/you/.ssh/id_rsa): [Press enter]
```

If you wish to change the name of the file containing the private/public key you should enter a path and filename manually.

You will then be prompted to enter a passphrase for your SSH-key. This will be used when you attempt to add your SSH key to your GitHub account for example. By adding the key to your ssh-agent, you will not have to enter the passphrase each time you use the key, as shown below.


**3.) Adding your SSH key to the ssh-agent**

First ensure that ssh-agent is enabled:

```
#start the ssh-agent in the background
$ eval "$(ssh-agent -s)"
Agent pid 59566
```
 
Now you can add your SSH key to the ssh-agent. If you used an existing key rather than generating a new one, or chose to have a different name on your SSH key, you need to replace the *id_rsa* in the command below with the name of your private key file.

```
$ ssh-add ~/.ssh/id_rsa
```

**4.) Add the SSH key to you GitHub account.**

Instructions on how to add your SSH key to your GitHub account can be found here:
[Adding a new SSH key to your GitHub account]: https://help.github.com/articles/adding-a-new-ssh-key-to-your-github-account/ "addtogithub"
[Adding a new SSH key to your GitHub account]\


**5.) Cloning the repository**

Now all that is left is to clone the repository:

```
$ git clone git@github.com:pantor-engineering/act.git
$ cd act
```

In order to use the tools provided however, the code needs to be built properly which leads us to the next section.

### 2: Building the core toolset

The code-tree is built with CMake, which uses the provided CMakeLists.txt files from the next section in order to generate makefiles which in turn are used to compile the tree. Looking into those textfiles will hopefully give you an idea on how to add your solutions for compilation and to link them to the core library. 

#### 2.1 Compiling the core-tree

Do the following in your terminal, make sure you are standing in the act directory:

```
$ cd act
#Jump to the correct directory

$ mkdir build
#Make a new directory called "build"

$ cd build
#Jump to that directory

$ cmake ..

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
Note that cleaning will remove the executable used in chapter 3, and 'make' will generate a new one.

For further instructions on the make commands:

```
$ make help
```

If you are uncertain or wish to gain additional information regarding Makefiles and CMake, read up on content not provided by the ACT organization.


### 3: Running the build

With the build compiled, there will now exist executables that can be run. You can see sample solutions provided under the `/act/solutions/` directory (and the executables in `/act/build/solutions/`). There is also a "tools" directory which illustrates the funcitonality of the core decoder-encoder provided. Having a look at those might give some instructions on how they can be run (looking at the main function will give an intuition of what flags that can be passed). An example will be given on how to run an executable properly below. The secho.cc tool works like an echo-tool, with the exception of an internal MsgSeqNo counter it will add and send (encode) to the messages in std::cout.

With that said, the core-program will therefor need input. In the simplified trading platform that ACT provides, there is only two streams of data: Standard Input (std::cin) and Standard Output (std::cout). Thus, these streams will handle input and output for both Client and Exchange (ESE) messages.

Now with some description at hand, let's go through how to run the program. You may want the specifications document readily available while doing this. Please see: http://act.pantor.com/spec/
and read up on "EMP text encoding" and "Message layout"

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

The different tasks you will attempt provides input examples in text format, which can be used to further test the functionality of the decoder-encoder. The [specification](http://act.pantor.com/spec) also provides text encoding examples of the Epsilon Message Protocol. Make sure to properly review the EMP specification before/whilst solving tasks, in order to understand how the messages should be interpreted and also figure out how the internal (decoded) representation is made in the ACT-platform. 

Hopefully this is enough information for you to start your progress as an intern at ACT. If you encounter any difficulties, do not hesitate to contact [ACT support](mailto:act-support@pantor.com) via email.
