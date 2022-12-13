# API Documentation

This is the documentation for libtase2.NET a .NET wrapper for the libtase2 TASE.2/ICCP protocol library.

##Building the library and the .NET wrapper

### Build the library (native DLL or .so file)

*CMake 3.0 is required*

Example on linux:

1) create a build folder

mkdir build

2) configure and create the makefiles

```
cd build
cmake ..
```

3) Build the library

```
make
```

Example for Visual Studio 2015 (see cmake --help for other "generator" options)

3a) For 32 bit build

```
cd build
cmake -G "Visual Studio 14 2015" ..
```

3b) For 64 bit build

```
cd build
cmake -G "Visual Studio 14 2015 Win64" ..
```

4) Installing the native DLL/shared library

Put the library in a folder where the OS can find it:

e.g /usr/lib for Linux or C:\Windows\System32 for Windows.

### Build the .NET wrapper:


Open the libtase2.NET.sln solution file in the NET folder with Visual Studio or MonoDevelop.

You can build all the targets there or only the libtase2.NET project.


### Build the .NET wrapper for .NET core 2.0:

Visual Studio 2017: Open the NET/core/2.0/libtase2.NET.core.2.0.sln file. Build the library and examples there.

.NET core SDK 2.0:

1) goto the NET/core/2.0 directory

2) Build the library and examples

```
dotnet build
```

3) Run the examples (e.g. tase2_client)

```
cd tase2_client
dotnet run
```