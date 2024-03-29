# Computational Mock-up in Fuse


### Concept: Fuse client and IOCTL

This project is a prototype demonstrating a potential API to computational storage.

The Computational Storage (or Active Storage) is implemented as a fuse client (named EXCBR daemon).
The daemon is using a source repository to present files and subdirectories in a target directory.
Any access to the target directory content is intercepted by EXCBR client: write requests are intercepted 
and the content is propagated to the original source directory. Read requests are handled in a similar way.
A specific call based on IOCTL is supported. Any application using this specific ICOTL to access a file in the
target directory will be intercepted by the EXCBR daemon nd the IOCTL call will be served as a computational request.

### Build the binary

make 
This should build two binaries: - EXCBR_csfs, the Computational daemon based on Fuse
- EXCBR_csfs_client, a test application running making call to the computational storage API

### Running Computational Storage

 1- populate the directory REPO with the files of your choices
 2- make run
this will start the Computational daemon using REPO as a source repository and TEST as a mirror repository.
Which means that every files pre-existing in REPO will appear in TEST, any further file created during the 
experiment in the TEST directory will also appears consequently in REPO. 
Once the EXCBR daemon is stopped, access to all the files created in TEST are lost. Re-starting EXCBR daemon allows
to retrieve access to these files. 

 3- Launch the test application EXCBR_csfs_client

This test application is making call the computational storage API (using IOTCL). The code is a source
of inspiration for any more advanced usage to computational storage.
Additionally to the IOCTL, the test application is linked with a dictionary of functions supported as well by the
EXBR daemon.

### Key data structure

To build an application using computational storage. The application has to be compiled with EXCBR_CSFS_fnct.h
which contains all the declaration of the CS functions.

The application uses 3 dedicated data type, the cs_args_t structure, and two predefined types: CS_FNCT_ID and CS_TYPE.
CS_TYPE is used to specify the operand type of the active function. CS_FNCT_ID is the unique identifier of the active function.
Because C is a typed language, CS_FNCT_ID is actually a class of functions, and the two fields CS_FNCT_ID plus CS_TYPE
are needed to build the unique identifier.

The structure cs_args_t is populated with these information, but the targeted file and the extend.

A typical code fragment would be:

    ret = ioctl(fd, CS_OPT, &cs_compad);

where fd is the file descriptor, CS_OPT marked that the IOCTL is to be interpreted as computation storage by EXCBR daemon.
The structure cs_compad is of type cs_args:

struct cs_args_t
        {
        CS_FNCT_ID fct_id; // function name
        CS_TYPE type_t;    // type of the operands
        size_t in_bfsz;    // length in Byte of the file extent
        size_t offset;     // first Byte to read
        size_t out_bfsz;   // reserved for future usage
        CS_RET out_bf;     // return value is a single elt
        } cs_args;


### Running a simple example

Checking if CSFS can count the vowel from a text file containing the string "hello world"

    /csfs_application -v TEST/test1.txt 
    Opening file TEST/test1.txt 
    TEST/test1.txt is 12 byte long
    Proceeding to IOCTL CS_COUNT_VOWEL on file TEST/test1.txt
    Result of: returns the number of char elt being vowel IOCTL on TEST/test1.txt set to 3

### Files List

- Readme.md, this file.
- Makefile, builder for the binaries. 
    - make all: build all binaries, CSFS fuse daemon, CS application and a tool binaries used for debugging
    - make run: launch the CSFS daemon using TEST and REPO as destination and source directories
    - make test: launch unit tests to check the posix support of the CSFS daemon
    - make free: unmount the fuse client mount point used by the CSFS daemon

- multifile.py, simple python example to create a Zarr multifile
- app_using_cs.c, client application using the computational storage function to accelerate processing.
- EXCBR_common.h, definitions which are shared between the storage system and the client application. This
includes the list of supported function (min, max, avg...) and the data type
- EXCBR_CSFS_fnct.c, implementation of the computational storage functions. This implementation is only
used by the Computational file system
- EXCBR_CSFS.c, Fuse implementation of the computational storage daemon file system.
- minitest_EXCBR_csfs.sh, simple test suite to validate the posix abilities of the fuse client
- array_as_file.c, serialize/deserialize an array of various data type as a binary file.
- REPO, Source directory used as playground for tests
- TEST, Target directory used as playgroubd for tests
- CSFS_Python, directory containing the files for python usage of CSFS daemon.


### Python, Zarr and Dask

the CSFS_Python directory contains a proxy library and a Python toy example.
The proxy lib is a dynamically linked wrapper around the ioctl and structure packing.
This allows to call the C function from python. An example using the ActiveStorage Python
framework from U. Readings is provided for illustrative purpose.




