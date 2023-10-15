#####################################################################################
#   -------------------------------------------                                     #
#   csv to markdown text converter (csv2mdText)                                     #
#   -------------------------------------------                                     #
#   Copyright 2023 Roberto Mameli                                                   #
#                                                                                   #
#   Licensed under the Apache License, Version 2.0 (the "License");                 #
#   you may not use this file except in compliance with the License.                #
#   You may obtain a copy of the License at                                         #
#                                                                                   #
#       http://www.apache.org/licenses/LICENSE-2.0                                  #
#                                                                                   #
#   Unless required by applicable law or agreed to in writing, software             #
#   distributed under the License is distributed on an "AS IS" BASIS,               #
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.        #
#   See the License for the specific language governing permissions and             #
#   limitations under the License.                                                  #
#   -----------------------------------------------------------------------------   #
#                                                                                   #
#   FILE:        csv2mdText makefile                                                #
#   VERSION:     1.0.0                                                              #
#   AUTHOR(S):   Roberto Mameli                                                     #
#   PRODUCT:     csv2mdText tool                                                    #
#   DESCRIPTION: This application parses a CSV input file and organizes information #
#                contained in specified columns into a markdown file, based upon    #
#                a given template                                                   #
#   REV HISTORY: See updated Revision History in file Changelog.md                  #
#                                                                                   #
#####################################################################################

all:
	gcc ./src/csv2mdText.c -I./headers -L./lib -v -Wall -o ./bin/csv2mdText

install:
	cp -p ./bin/csv2mdText /usr/local/bin/

clean:
	rm ./bin/csv2mdText
	rm /usr/local/bin/csv2mdText
