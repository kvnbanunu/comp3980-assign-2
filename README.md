# COMP3980 Assignment 1

Program written by Kevin Nguyen.

This program reads an input text file, character by character, applies a filter, and writes the processed character to an output text file.

## **Table of Contents**

1. [Cloning the Repository](#cloning-the-repository)
2. [Prerequisites](#Prerequisites)
3. [Running the `generate-cmakelists.sh` Script](#running-the-generate-cmakelistssh-script)
4. [Running the `change-compiler.sh` Script](#running-the-change-compilersh-script)
5. [Running the `build.sh` Script](#running-the-buildsh-script)
5. [Running the `build-all.sh` Script](#running-the-build-allsh-script)
6. [Adding the input file](#adding-the-input-file)
7. [Running the program](#running-the-program)

## **Cloning the Repository**

Clone the repository using the following command:

```bash
git clone https://github.com/kvnbanunu/comp3980-assign-1.git
```

Navigate to the cloned directory:

```bash
cd comp3980-assign-1
```

Ensure the scripts are executable:

```bash
chmod +x *.sh
```

## **Prerequisites**

- to ensure you have all of the required tools installed, run:
```bash
./check-env.sh
```

If you are missing tools follow these [instructions](https://docs.google.com/document/d/1ZPqlPD1mie5iwJ2XAcNGz7WeA86dTLerFXs9sAuwCco/edit?usp=drive_link).

## **Running the generate-cmakelists.sh Script**

You will need to create the CMakeLists.txt file:

```bash
./generate-cmakelists.sh
```

## **Running the change-compiler.sh Script**

Tell CMake which compiler you want to use:

```bash
./change-compiler.sh -c <compiler>
```

To the see the list of possible compilers:

```bash
cat supported_cxx_compilers.txt
```

## **Running the build.sh Script**

To build the program run:

```bash
./build.sh
```

## **Running the build-all.sh Script**

To build the program with all compilers run:

```bash
./build-all.sh
```

## **Adding the input file**

An input text file is necessary to run this program.

I have included a sample one in the root of this folder.

Move it into the build directory with:

```bash
mv input.txt build/
```

## **Running the Program**

To run the program...
Navigate into the build directory

```bash
cd build/
```

Ensure you have proper permissions:

```bash
chmod +x comp3980-assign-1
```

Execute the program with the following arguments:

"-i" [input text file]

"-o" [output text file] (optional)

    defaults to output.txt
    
"-f" [filter]

    upper
    
    lower
    
    null

```bash
./comp3980-assign-1 -i input.txt -o output.txt -f upper
```
