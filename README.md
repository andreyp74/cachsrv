# Cache server
## Purpose of the project
Training project. 
A simple cache server that accumulates records and transmits them to the client based on one of two metrics: timeout or number of records.

## Requirements
* cmake 3.5+

## Configure and build
```
mkdir mbuild && cd mbuild
```
* Configure 32 bit example for Windows:
```
cmake -G "Visual Studio 15 2017" -A Win32 .. 
```
* Configure 64 bit example for Windows:
```
cmake -G "Visual Studio 15 2017" -A x64 ..
```
* Build Debug
```
cmake --build . --config Debug 
```
* Build Release
```
cmake --build . --config Release
```