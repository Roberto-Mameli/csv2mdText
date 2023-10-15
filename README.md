[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](code_of_conduct.md)

- [Disclaimer](#disclaimer)
- [Introduction](#introduction)
- [Installation of *csv2mdText* Tool](#installation-of-csv2mdtext-tool)
- [Usage of *csv2mdText* Tool](#usage-of-csv2mdtext-tool)
  - [First Command Layout](#first-command-layout)
  - [Second Command Layout](#second-command-layout)
  - [Third Command Layout](#third-command-layout)
- [Examples](#examples)


# Disclaimer
This software is licensed under the terms of the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at:

> [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

# Introduction
This tool can be used to produce a document in markdown format, by extracting information from a table available in CSV format and organizing it according to a given template.

It can be very useful to automate document production in all cases where the document structure is repetitive and the information are available in tabular format (e.g. description of a set of products or similar). 


# Installation of *csv2mdText* Tool
The tool can be either downloaded or cloned from the following [git repository](https://github.com/Roberto-Mameli/csv2mdText) onto the local machine.

Let's denote by *some_directory* the directory in the local filesystem that will be used to download and compile the tool:

> cd *some_directory*

To clone from github repository, issue the following command:

> git clone https://github.com/Roberto-Mameli/csv2mdText.git

Alternatively, if you have downloaded the zip file from github, then issue the following command:

> unzip csv2mdText.zip

In both cases, a new directory (*csv2mdText*) will be created into *some_directory*.

The tool can be compiled and installed by means of the following commands:

> cd *csv2mdText*
> 
> sudo make clean
> 
> sudo make all
> 
> sudo make install


Please be aware that *sudo* is not needed in case compilation is done by *root* user.

When the tool is installed, the executable file is copied into */usr/local/bin*, hence it is immediately available from any directory in the file system.

To uninstall the tool, simply go to the *csv2mdText* directory and type:

> cd *csv2mdText*
> 
> sudo make clean


# Usage of *csv2mdText* Tool
The tool admits 3 different layouts, reported below:

> csv2mdText [-n] [-a] [-s <separator>] [-p <placeholder>] [-r <remark>] [-c <chapter_md_template>] -i <csv_input_file> -o <md_output_file> -t <md_template>
> 
> csv2mdText -d <csv_input_file>
> 
> csv2mdText -h

## First Command Layout
Let's focus first on the first command layout reported above. The purpose of this command is to create a markdown output file, formatted according to a given template, with contents extracted from an input text file in csv format.

For the purpose, the command requires three mandatory parameters, listed below:

- *option -i*: specifies the input csv file (namely a text file with fields delimited by a proper separator (semicolon ";" by default). The separator can be changed through option *-s* (see below).
- *option -o*: specifies the output file. By default the output file is overwritten by the tool (i.e. any previous content is deleted), but this behaviour can be changed by using option *-a*.
- *option -t*: used to provide the template file in markdown format. This is a valid markdown file, which contains some placeholders associated to specific columns in the input csv file (e.g. *"$1"* stands for the content of the first column, *"$2"* represents the second column, and so on). The escape character used to identify a placeholder can be redefinned by means of option *-p* (see below).

Basically, the tool works as follows. It scans the csv input file row-by-row, and it builds an output markdown file, by concatenating multiple instances of the markdown template (*-t*), one for each row of the input csv file. When adding a new instance of the template file to the output file under construction, it substitutes all placeholders with the corresponding column values extracted from the current row of the input csv file.

Observe that when parsing the input csv file, empty lines and comments are skipped. A line is considered empty if it contains only spaces, tabs and newlines. A line starting with a hash character (#) is considered a comment. Please be aware that the presence of # in the middle of a line is interpreted as a comment that terminates the line (i.e., all characters following it are discarded). The character used to denote comments can be changed by option *-r* (see below).

By default, the first row in the input file is treated as an header and is discarded (e.g. think to an excel table which is exported in csv format, in which usually the first row contains column names). This behaviour can be changed by specifying the optional parameter *-n* (where *n* stands for *no header*). Please be aware that when talking about the first line of the csv input file, we refer to the first line which is neither empty nor a comment.

The following optional parameters are also accepted (most of them have been already introduced above):

- *option -n*: if *-n* is specified, the first line in the csv input file is not considered an header, rather a line that provides useful information to be added to the output file (i.e. it is not skipped)
- *option -a*: by specifying this option, we force the tool to open the output file in append mode (i.e. its content is not overwritten). It may be useful in case the resulting output shall be obtained by the concatenation of information from different csv input files with different templates
- *option -s*: is used to specify a different separator for the csv file other than the semicolon. The character specified with this option shall be enclosed by quotes (e.g. *-s "|"* for defining the pipe sign as separator)
- *option -p*: used to specify a placeholder for the template file different from the default dollar sign (*"$"*). The character specified with this option shall be enclosed by quotes (e.g. *-s "%"* for using the percentage for placeholders)
- *option -r*: optional argument that allows to specify a character other than the hash for comments in the CSV file. The character specified here shall be enclosed by quotes (e.g. *-r "!"* for using the exclamation mark for comments).
- *option -c*: this option allows to define an optional markdown template for defining highest level chapter templates. This is better explained in the comments.

## Second Command Layout
This layout is useful when the CSV Input file contains a first row with column names. In this case, issuing the fiollowing command:

> csv2mdText -d <csv_input_file>

we get the list of the input file columns along with the associated column number. This can be useful to associate the proper placeholder into the markdown template files. 


## Third Command Layout
This layout simply displays a detailed help on the command:

> csv2mdText -h

# Examples
As an example consider the following command:

> csv2mdText -i ~/myInput.csv -o ~/myOutput.md -t ~/myTemplate.md

It generates the markdown output file *~/myOutput.md* by concatenating several instances of the markdown input template *~/myTemplate.md*, one for each row of the input csv file *~/myInput.csv*. In doing so, information extracted from the input file are properly substituted to placeholders in the template, according to the following rule: *$1* is replaced by the content of the first column, *$2* by the second column, and so on. The output file is written from scratch (previous content
is lost) and the first valid line of the csv input file is considered an header and therefore skipped.

> csv2mdText -n -i ~/myInput.csv -r "!" -o ~/myOutput.md -p "&" -t ~/myTemplate.md

This command is very similar to the previous one, with some notable differences. Specifically, the first valid line of the csv input file is considered valid and used as all the others (option *-n*), comments within the input file are identified by the exclamation mark (not by hash), and placeholders in the template are represented by *"&"* and not by the dollar sign (*"$"*).

The *examples* subdirectory contains further sample files. Look at the [README](./examples/README.md) file inside the directory for detailed explanation.
