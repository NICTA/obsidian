Obsidian
========
Obsidian is a set of tools designed to perform probabilistic inference with
geophysical sensor data to produce models of the sub-surface. It is written
primarily in C++ and runs on GNU/Linux and Unix operating systems.

Description
-----------
The software is designed to perform geophysical inversions in regions relevant to the geothermal industry:
specifically sedimentary basins overlying a basement into which granites may or may not be intruding.
This flexibility allows us to model both Enhanced Geothermal Systems (EGS) such as the Cooper Basin,
and (Hot Sedimentary Acquifier) HSA systems such as the Otway basin.

Probabilistic inversions are generally computationally expensive, but Obsidian can
parallelise the workload over a cluster by delegating expensive sensor simulations
to workers to significantly improve performance. Obsidian has been successfully
used to perform large scale inversions on cluster architectures such as Amazon EC2.

Contact Us
----------
Please feel free to contact the developers at NICTA if you have questions
about building, running or interpreting results from Obsidian. Best to email
lachlan.mccalman@nicta.com.au.

System Support
--------------
Currently, Obsidian runs on Linux-based operating systems only.

Compiler Support
----------------
Obsidian successfully compiles under GCC 4.9.

Prerequisites
-------------
Obsidian requires the following libraries as prerequisites:

* Boost 1.55
* Eigen 3.2.0
* google-log (glog) 0.3.3
* google-test (gtest) 1.7.0
* zeromq 4.0.3
* cppzeromq 2358037407 (commit hash)
* Protocol-buffers (protobuf) 2.5.0
* leveldb 1.15.0

Please note that the actual source of google-test is required, so don't attempt
to use a pre-compiled version from a package manager. Obsidian compiles
libraries from google-test as part of the build process. The `prereqs` folder
contains some instructions for building each of the above libraries from source
code. There is also a script for downloading and building prerequisites in the
`prereqs` folder.

Building
--------
There is a `build.sh` script included with Obsidian in that can be used to easily
build the project using CMake, by specifying environment variables
corresponding to the locations of the prerequisite libraries. If you have
installed them all into `/usr/local`, you can probably ignore them and just run
CMake. Once you have run CMake in your build directory, a simple make command
will suffice to build the project.


Documentation
-------------
There is doxygen documentaion available
[here](http://nicta.github.io/obsidian). To compile the documentation locally,
just run
`make doc`

Current Limitations
-------------------
* The ground surface is currently assumed to be flat.
* Only a very basic visulation script is included.
* Rock properties are assumed to be multivariate Gaussians.


Licence
-------
Obsidian is released under the GNU Affero General Public License Version 3 or later.
Please see [COPYRIGHT](COPYRIGHT).

Obsidian incorporates a modifed version of cnpy, which is copyright (c)
Carl Rogers, 2011 and distributed under the MIT license. The modified copy of
cnpy can be found along with the license in the src/io directory.

Bug Reports
-----------
If you find a bug, please open an
[issue](http://github.com/NICTA/obsidian/issues).

Contributing 
------------
Contributions and comments are welcome. Please read our [style guide](docs/CodeGuidelines.md)
before submitting a pull request.
