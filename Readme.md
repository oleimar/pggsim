
# pggsim: public-goods game C++ simulation code


## Overview

This repository contains C++ code and example data.
The executable program, built from this code, will run learning simulations of one or several groups of individuals that repeatedly play a public goods game with continuous actions.
The program also simulates the evolution of cognitive bias over a number of generations.
It was used to produce the simulation results for the paper "Learning leads to bounded rationality and the evolution of cognitive bias in public goods games" by Olof Leimar and John M. McNamra.

## System requirements

This program has been compiled and run on a Linux server with Ubuntu 18.04 LTS.
The C++ compiler was g++ version 7.4.0, provided by Ubuntu, with compiler flags for c++14.
The program can be run multithreaded using OpenMP, which speeds up execution times.
Most likely these instructions will work for many Linux distributions.
For single-threaded use, the program has also been compiled and run on macOS, using the Apple supplied Clang version of g++, but multithreaded use on macOS is unreliable and should be avoided.
On macOS, the supplied Makefile compiles an executable without OpenMP, in which case the program will run single-threaded.

## Installation guide

Install the repository from Github to a local computer.
There is a single directory pggsim for source code and executable, a subdirectory Data where input data and data files containing simulated populations are kept, and a subdirectory obj where object files generated during compilation are kept.

From the command line, with pggsim as current directory, give the command

`make release`

to compile and link an executable.
The name of the executable will be EvoProg.exe.
To remove the executable and and generated object files, give the command

`make clobber`

It is also possible to build a debug version of the executable (typically not needed), using the command

`make debug`

The name of the debug executable will be EvoProgDebug.exe.

## Demo: Example data files

To run the executable with input data from the file Run00.inp, located in the Data subdirectory, set pggsim as the current directory and give the command

`./EvoProg Data/Run00.inp`

The program will run a learning simulation, setting the allelic values at the three loci to the values given by the parameter all0 in the input file.
The diploid loci correspond to w0 (initial value of w), theta0 (initial value of theta), and d (value of cognitive bias).
The diploid values will be twice the allelic values.
The time to run using these input parameters is a few milliseconds.

The output population, at the end of learning, will be written to the file Data/Run00.txt.
This is a tab-separated text file, with column headings on the first line, and each following line gives allelic and trait values for one individual.
The file can be opened in Excel, or imported into a data frame in R, using a command like d00 <- read.delim("Run00.txt").

Next, for an evolutionary simulation, run the executable with input data from the file Run01.inp by giving the command

`./EvoProg Data/Run01.inp`

This illustrates how you can take an input population from one file, Data/Run00.txt in this case, and write the output population to another, Data/Run01.txt in this case (note that input and output population can use the same file, which will be overwritten by the new output).
The demo simulation runs over 1000 generations, which takes around 20 s on an average laptop.
In this simulation, a cognitive bias can evolve.
Note that the population is relatively small, 500 individuals, so the evolution will be influenced by genetic drift.

## Data files and R scripts for the figures in the paper

The figure pdf files, R scripts and data files used by the figure scripts have been deposited at the Dryad repository: xxxx.
The corresponding program input files and R scripts are found in this repository, as described below.

## Instructions for use

There are basically two ways to use the program.
The first is to simulate learning histories for each group, in a single generation, by using a script to repeatedly run the program for a single or a low number of rounds, working out and saving the learning statistics to a data file.
This is illustrated by figure 1B in the paper, as follows.
The script file Fig1b_run.R, in the pggsim directory, repeatedly runs the program with the Data/Run02_1.inp input file, and accumulates the successive learning statistics in the Data/Fig1b_data.txt file.
To run the script with R, you can for instance give the command

`Rscript Fig1b_run.R`

For this to work, a suitable starting Data/Run02_1.txt file must be prepared, containing the initial population.
Such a starting population can created for instance by running the program.
Figure 2 in the paper was also created in this way.

The second way of using the program is to run over a number of generations, letting the cognitive bias, and possibly also the starting values of w and theta, evolve.
This can either be done repeatedly using a script, as for figure 3A in the paper, or run once but for a large number of generations, as for figures 3B and 4.

### Recreating the simulation results in the paper

The R script files and parameter input files that were used to generate the results in the figures of the paper are supplied.

## License

The EvoProg program runs actor-critic learning simulations

Copyright (C) 2019  Olof Leimar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
