# README  {#mainpage}


## Documentation:

- please have a look at the examples
- the application developer guide in the doc folder (also online: https://support.mz-automation.de/doc/libtase2/manual/latest/tase2_developer_guide.html) 
- the online API documentation for the C library: https://support.mz-automation.de/doc/libtase2/c/latest/
- the online API documentation for the .NET API: https://support.mz-automation.de/doc/libtase2/net/latest/
- the online API documentation for the Java API: https://support.mz-automation.de/doc/libtase2/java/latest/
- support online: https://mz-automation.atlassian.net/servicedesk/customer/portal/7

## Building the library:

CMake is required

Example on linux:

1) create a build folder

mkdir build

2) configure and create the makefiles

cd build
cmake ..

3) Build the library

make

Example for Visual Studio 2015 (see cmake --help for other "generator" options)

3a) For 32 bit build

  cd build

For Visual Studio 2015:

  cmake -G "Visual Studio 14 2015" ..

For Visual Studio 2017:

  cmake -G "Visual Studio 15 2017" ..

3b) For 64 bit build

  cd build


For Visual Studio 2015:

  cmake -G "Visual Studio 14 2015 Win64" ..

For Visual Studio 2017:

  cmake -G "Visual Studio 15 2017 Win64" ..
  
For Visual Studio 2019

  cmake -G "Visual Studio 16 2019" .. -A x64

## Building the .NET wrapper:

Open the libtase2.NET.sln solution file in the NET folder with Visual Studio or MonoDevelop.

You can build all the targets there or only the libtase2.NET project.


## Building the .NET wrapper for .NET core 2.0:

Visual Studio 2017: Open the NET/core/2.0/libtase2.NET.core.2.0.sln file. Build the library and examples there.

.NET core SDK 2.0:

1) goto the NET/core/2.0 directory

2) Build the library and examples

dotnet build

3) Run the examples (e.g. tase2_client)

cd tase2_client
dotnet run

## Solution for .NET Standard 2.0 and .NET Core 3.0

There is also a solution using .NET Standard 2.0 for the library and .NET Core 3.0 for the C# example application.

This solution can be found in NET/core/3.0


## Building a demo application with the binary distribution on Linux

This section shows how to build and run a simple client/server application with the demo distribution on Linux.

Install the TASE.2 library:

  cd lib
  
  sudo cp libtase2.so /usr/bin
  
  sudo ldconfig

Compile and start the server application:

  cd examples/server2
  
  gcc -c -I../../inc/ iccp_server2.c
  
  gcc -o iccp_server2 iccp_server2.o -L../../lib -ltase2 -lpthread
  
  ./iccp_server2
  
  
Compile and start the client application:

  cd examples/basic_client
  
  gcc -c -I../../inc iccp_client1.c
  
  gcc -o iccp_client1 iccp_client1.o -L../../lib -ltase2 -lpthread
  
  ./iccp_client1


You can also build the demo applications with cmake:

  mkdir build
  cd build
  cmake ..
  make
  
Then you can find the examples in the cmake/examples folder.


