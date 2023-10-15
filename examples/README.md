This directory contains the following files:
    - a couple of templates for building the output markdown file (files Template_*.md)
    - an example csv input file (Sample1.csv)
    - reference.docx which specifies formats to be used by pandoc to convert from .md to .docx

The document can be produced as follows. First, the markdown file shall be generated from the input csv file:

    csv2mdText  -n -i ./Sample1.csv -o Sample1.md -t Template_TC.md -c Template_Chapter.md
    
(note: omitting "-c Template_Chapter.md" from the command above produces a flat document structure with a set of sections non organized in higher level chapters). Note also that -n is specified, since this csv file does not contain an header row (i.e. the first line contains valid data).

From the output of the command above, the docx format can be produced through the open source pandoc command:

    pandoc -f markdown -t docx --data-dir=./ -s -o Sample1.docx Sample1.md

Please, be aware that the input markdown file shall be necesarily encoded in UTF-8 format (this is the only format accepted by pandoc). This could be not the case if the csv is obtained e.g from excel and the cells contains not strictly ASCII characters). If this is the case, the format can be converted by the following commands:

    file -bi Sample2.csv		(to check for the original format, e.g. text/plain; charset=iso-8859-1)
    iconv -f iso-8859-1 -t utf-8 -o Sample2_UTF-8.csv Sample2.csv	(to perform conversion, -f specifies the format found above)
    file -bi Sample2_UTF-8.csv		(to check the correct format)
    
Once the input has been converted, the previous commands can be used by specifying the converted file
