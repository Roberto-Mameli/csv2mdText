/*************************************************************************************
 *   -------------------------------------------                                     *
 *   csv to markdown text converter (csv2mdText)                                     *
 *   -------------------------------------------                                     *
 *   Copyright 2023 Roberto Mameli                                                   *
 *                                                                                   *
 *   Licensed under the Apache License, Version 2.0 (the "License");                 *
 *   you may not use this file except in compliance with the License.                *
 *   You may obtain a copy of the License at                                         *
 *                                                                                   *
 *       http://www.apache.org/licenses/LICENSE-2.0                                  *
 *                                                                                   *
 *   Unless required by applicable law or agreed to in writing, software             *
 *   distributed under the License is distributed on an "AS IS" BASIS,               *
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.        *
 *   See the License for the specific language governing permissions and             *
 *   limitations under the License.                                                  *
 *   -----------------------------------------------------------------------------   *
 *                                                                                   *
 *   FILE:        csv2mdText source file                                             *
 *   VERSION:     1.0.0                                                              *
 *   AUTHOR(S):   Roberto Mameli                                                     *
 *   PRODUCT:     csv2mdText tool                                                    *
 *   DESCRIPTION: This application parses a CSV input file and organizes information *
 *                contained in specified columns into a markdown file, based upon    *
 *                a given template                                                   *
 *   REV HISTORY: See updated Revision History in file Changelog.md                  *
 *                                                                                   *
 *************************************************************************************/


/**********************
 * Linux system files *
 **********************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


/***************
 * Definitions *
 ***************/
#define MAXFILENAMELEN   256    /* Maximum length for file names                             */
#define MAXLINELEN      2048    /* Maximum length for a single line of the CSV input file    */
#define MAXFIELDLEN     8192    /* Maximum length for a single field                         */
#define MAXFIELDS         64    /* Maximum number of fields                                  */

#define DEFSEPARATOR     ';'    /* Default CSV separator                                     */
#define DEFHEADER       true    /* By default we assume that the first CSV line is an header */
#define DEFPLACEHLDR     '$'    /* By default placeholders in the template are $<fieldno>    */
#define DEFCOMMENT       '#'    /* This is the character in the CSV that precedes a comment  */
#define DEFMULTILINE     '"'    /* Character enclosing fields spanning over multiple lines   */
#define DEFAPPEND      false    /* If true, the output md file is opened in append mode      */

#define UNDEFINED          0    /* Possible values for altSyntax variable (for args parsing) */
#define STANDARD           1
#define DECODEHDR          2


/********************
 * Type Definitions *
 ********************/
typedef char    filenameString[MAXFILENAMELEN+1];
typedef char    lineString[MAXLINELEN+1];
typedef char    fieldString[MAXFIELDLEN+1];


/********************
 * Global Variables *
 ********************/

/**********************
 * Internal Functions *
 **********************/
/*********************************************/
/* Print out the command format (short form) */
/*********************************************/
static void printUsage (void)
{
    printf ("Usage:\n\n");
    printf ("    csv2mdText [-n] [-a] [-s <separator>] [-p <placeholder>]\n");
    printf ("               [-r <remark>] [-c <chapter_md_template>]\n");
    printf ("               -i <csv_input_file> -o <md_output_file> -t <md_template>\n");
    printf ("\n");
    printf ("    csv2mdText -d <csv_input_file>\n");
    printf ("\n");
    printf ("    csv2mdText -h\n");
    printf ("\n");

    return;
}


/***************************************************/
/* Print out the complete command help (long form) */
/***************************************************/
static void printHelp (void)
{
    printf ("Usage:\n\n");
    printf ("    csv2mdText [-n] [-a] [-s <separator>] [-p <placeholder>]\n");
    printf ("               [-r <remark>] [-c <chapter_md_template>]\n");
    printf ("               -i <csv_input_file> -o <md_output_file> -t <md_template>\n");
    printf ("\n");
    printf ("    csv2mdText -d <csv_input_file>\n");
    printf ("\n");
    printf ("    csv2mdText -h\n");
    printf ("\n");
    printf ("Note Well: the instructions below refer to the first command layout reported above\n");
    printf ("(the third layout, i.e. the one with option -h only, provides simply this help).\n");
    printf ("\n");
    printf ("The purpose of this command is to create a markdown output file, formatted according\n");
    printf ("to a given template, with contents extracted from an input text file in csv format.\n");
    printf ("For the purpose, the command requires three mandatory parameters, listed below:\n");
    printf ("\n");
    printf ("    -i  this option specifies the input csv file (namely a text file with fields delimited\n");
    printf ("        by a proper separator (semicolon \";\" by default). The separator can be changed\n");
    printf ("        through option -s (see below).\n");
    printf ("\n");
    printf ("    -o  specifies the output file. By default the output file is overwritten by the tool\n");
    printf ("        (i.e. any previous content is deleted), but this behaviour can be changed by\n");
    printf ("        using option -a.\n");
    printf ("\n");
    printf ("    -t  is used to provide the template file in markdown format. This is a valid markdown\n");
    printf ("        file, which contains some placeholders associated to specific columns in the input\n");
    printf ("        csv file (e.g. \"$1\" stands for the content of the first column, \"$2\" represents\n");
    printf ("        the second column, and so on). The escape character used to identify a placeholder\n");
    printf ("        can be redefinned by means of option -p (see below).\n");
    printf ("\n");
    printf ("Basically, the tool works as follows. It scans the csv input file row-by-row, and it builds an\n");
    printf ("output markdown file, by concatenating multiple instances of the markdown template (-t), one\n");
    printf ("for each row of the input csv file. When adding a new instance of the template file to the\n");
    printf ("output under construction, it substitutes all placeholders with the corresponding value\n");
    printf ("extracted from the current row of the input csv file.\n");
    printf ("\n");
    printf ("Observe that when parsing the input csv file, empty lines and comments are skipped. A line is\n");
    printf ("considered empty if it contains only spaces, tabs and newlines. A line starting with a hash\n");
    printf ("character (""#"") is considered a comment. Please be aware that the presence of ""#"" in the\n");
    printf ("middle of a line is interpreted as a comment that terminates the line (i.e., all characters\n");
    printf ("following it are discarded). The character used to denote comments can be changed by option\n");
    printf ("-r (see below).\n");
    printf ("\n");
    printf ("By default, the first row in the input file is treated as an header and is discarded (e.g.\n");
    printf ("think to an excel table which is exported in csv format). This behaviour can be changed by\n");
    printf ("specifying the optional parameter -n (which stands for no header).  Please be aware that when\n");
    printf ("talking about the first line of the csv input file, we refer to the first line which is\n");
    printf ("neither empty nor a comment.\n");
    printf ("\n");
    printf ("The following optional parameters are also accepted (most of them have been already introduced\n");
    printf ("above):\n");
    printf ("\n");
    printf ("    -n  if -n is specified, the first line in the csv input file is not considered an header,\n");
    printf ("        rather a line that provides useful information to be added to the output file (i.e.\n");
    printf ("        it is not skipped).\n");
    printf ("\n");
    printf ("    -a  by specifying this option, we force the tool to open the output file in append mode\n");
    printf ("        (i.e. its content is not overwritten). It may be useful in case the resulting output\n");
    printf ("        shall be obtained by the concatenation of information from different csv input files\n");
    printf ("        with different templates.\n");
    printf ("\n");
    printf ("    -s  used to specify a different separator for the csv file other than the semicolon. The\n");
    printf ("        character specified with this option shall be enclosed by quotes (e.g. -s \"|\" for\n");
    printf ("        defining the pipe sign as separator).\n");
    printf ("\n");
    printf ("    -p  used to specify a placeholder for the template file different from the default dollar\n");
    printf ("        sign (\"$\"). The character specified with this option shall be enclosed by quotes\n");
    printf ("        (e.g. -s \"*\" for using the asterisk for placeholders).\n");
    printf ("\n");
    printf ("    -r  optional argument that allows to specify a character other than the hash for comments\n");
    printf ("        in the CSV file. The character specified here shall be enclosed by quotes (e.g. -r \"!\"\n");
    printf ("        for using the exclamation mark for comments).\n");
    printf ("\n");
    printf ("    -c  this option allows to define an optional markdown template for defining highest level\n");
    printf ("        chapter templates.\n");
    printf ("\n");
    printf ("Examples:\n");
    printf ("    csv2mdText -i ~/myInput.csv -o ~/myOutput.md -t ~/myTemplate.md\n");
    printf ("        Generates the markdown output file ~/myOutput.md by concatenating several instances of\n");
    printf ("        the markdown input template ~/myTemplate.md, one for each row of the input csv file;\n");
    printf ("        in doing so, information extracted from the input file are properly substituted to\n");
    printf ("        placeholders in the template. The output file is written from scratch (previous content\n");
    printf ("        is lost). The first valid line of the csv input file is skipped.\n");
    printf ("\n");
    printf ("    csv2mdText -n -i ~/myInput.csv -o -r \"!\" ~/myOutput.md -p \"&\" -t ~/myTemplate.md\n");
    printf ("        As above, with some differences. specifically, the first valid line of the csv input\n");
    printf ("        file is not skipped (option -n), comments within the input file are identified by the\n");
    printf ("        exclamation mark (not by hash), and placeholders in the template are represented by\n");
    printf ("        \"&\" and not by the dollar sign (\"$\").\n\n");

    return;

}


/**************************************************************************/
/* This function remove all double occurences of the multi line character */
/* from the string passed as first argument. It does not return anything  */
/**************************************************************************/
static void removeDoubleMultiLineChar (char *inputLine, char multiLine)
{
    /* Local Variables */
    char       *p, *q;
    lineString  dummyString = "";

    p = inputLine;
    q = dummyString;

    if (p==NULL)
        return;

    /* Remove all double occurences of multi line character */
    while (*p != '\0')
    {
        *q = *p;
        if ( (*p == multiLine) && (*(p+1) == multiLine) )
            p += 1;
        p += 1;
        q += 1;
    }   /* while (*p != '\0') */

    *q = '\0';
    strcpy (inputLine, dummyString);

    return;

}


/*********************************************************************************************/
/* Scan the current line read from input CSV file char-by-char and fills the fields[] array  */
/* This function is invoked by the main loop when in single line mode                        */
/* It exits when one of the following conditions occur:                                      */
/* - EOL is reached (in this case it returns false, i.e. it stays in single line mode)       */
/* - a delimiter is found at the beginning of the current field (in this case it returns     */
/*   true, i.e. it switches to multiline mode). Upon return, control is taken by the         */
/*   complementary function scanMultiLine()                                                  */
/* Remember that all double delimiters have been removed by removeDoubleMultiLineChar(), so  */
/* that we do not have to worry about that (i.e., if a delimiter is found within a field,    */
/* neither at the beginning nor at the end, but in the middle, it means that in the original */
/* line there was a double delimiter)                                                        */
/*********************************************************************************************/
static bool scanSingleLine (char **pp, char separator, char multiLine, int lineNo, int *fieldNo, fieldString fields[])
{
    /* This function is invoked from the main loop when we are not in multi line mode */
    /* p points to the char we are currently scanning */

    /* Local Variables */
    char   *q, *r, *p;
    int     len;
    bool    withinQuotes;

    /* Initialization */
    p = *pp;
    /* Go through the line char-by-char */
    while ( (p!=NULL) && (*p!='\0') )
    {
        len = strlen (p);
        if ( (q=strchr (p,separator)) != NULL)
            *q = '\0';
        if ( (r=strchr (p,multiLine)) == NULL )
        {   /* This line does not contain the multi line delimiter */
            strcpy (fields[*fieldNo],p);
            (*fieldNo) += 1;
            strcpy (fields[*fieldNo],"");
            if (q)
                p = q+1;
            else
                p += len;
            continue;
        }   /* if ( (r=strchr (p,multiLine)) == NULL ) */
        /* We reach this point only if this line contains the multi line */
        /* delimiter before the separator and/or the EOL (note that so   */
        /* far we are in no multi line mode) */
        if (q)  /* Restore the separator, otherwise the line will be truncated */
            *q = separator;
        if ( (r==p) || (*(r-1)=='\0') || (*(r-1)==separator) )
        {   /* Special case: the delimiter is the first char in p or is */
            /* preceded by a separator -> reset pointer p to the first  */
            /* char after the delimiter and switch to multi line mode   */
            strcpy (fields[*fieldNo],"");
            p = r+1;
            *pp = p;
            return (true);
        }
        /* We are here because there is at least a multiline delimiter */
        /* before the next separator (or the EOL). Go on char-by-char  */
        /* starting from current position (p) and consider as only     */
        /* separator characters that are not included in multi line    */
        /* as actual separators                                        */
        withinQuotes = false;   /* single line mode -> we start with withinQuotes = false */
        q = p;
        while ( (q!=NULL) && (*q!='\0') )
        {
            if (*q==multiLine)
                withinQuotes = withinQuotes?false:true;
            if ( (*q==separator) && (withinQuotes==false) )
            {   /* we have found an actual separator, i.e. not enclosed within delimiters */
                *q = '\0';
                strcpy (fields[*fieldNo],p);
                (*fieldNo) += 1;
                strcpy (fields[*fieldNo],"");
                p = q+1;
                *pp = p;
                return (false);
            }   /* if ( (*q==separator) && (withinQuotes==false) ) */
            q += 1;
        }   /* while ( (q!=NULL) && (*q!='\0') ) */
        strcpy (fields[*fieldNo],p);
        (*fieldNo) += 1;
        strcpy (fields[*fieldNo],"");
        p = q;

    }   /* while ( (p!=NULL) && (*p!='\0') ) */

    /* The line is finished without switching to multi line mode */
    *pp = p;
    return (false);
}


/***********************************************************************************************/
/* Scan the current line read from input CSV file char-by-char and fills the fields[] array    */
/* This function is invoked by the main loop when in multi line mode                           */
/* It exits when one of the following conditions occur:                                        */
/* - EOL is reached (in this case it returns true, i.e. it stays in multi line mode)           */
/* - a delimiter is found at the end of the current field (in this case it returns false,      */
/*   i.e. it switches to single line mode). Upon return, control is taken by the complementary */
/*   function scanSingleLine()                                                                 */
/* Remember that all double delimiters have been removed by removeDoubleMultiLineChar(), so    */
/* that we do not have to worry about that (i.e., if a delimiter is found within a field,      */
/* neither at the beginning nor at the end, but in the middle, it means that in the original   */
/* line there was a double delimiter)                                                          */
/***********************************************************************************************/
static bool scanMultiLine (char **pp, char separator, char multiLine, int lineNo, int *fieldNo, fieldString fields[])
{
    /* This function is invoked from the main loop when we are in multi line mode */
    /* p points to the char we are currently scanning */

    /* Local Variables */
    char   *r, *p;
    int     len;
    bool    withinQuotes;

    /* Initialization */
    p = *pp;

    /* Go through the line char-by-char */
    while ( (p!=NULL) && (*p!='\0') )
    {
        len = strlen (p);
        if ( (r=strchr (p,multiLine)) == NULL )
        {   /* This line does not contain the multi line delimiter */
            strcat (fields[*fieldNo],p);
            strcat (fields[*fieldNo],"\n");
            p += len;
            *pp = p;
            return (true);
        }   /* if ( (r=strchr (p,multiLine)) == NULL ) */
        /* If we reach this point this line contains the multi line */
        /* delimiter. Check whether it ends the field or not        */
        if ( (*(r+1)=='\0') || (*(r+1)==separator) )
        {   /* The delimiter is the last char of the current field */
            /* preceded by a separator -> stop multi line mode     */
            *r = '\0';
            strcat (fields[*fieldNo],p);
            (*fieldNo) += 1;
            strcpy (fields[*fieldNo],"");
            if (*(r+1)==separator)
                p = r+2;
            else
                p = r+1;
            *pp = p;
            return (false);
        }
        /* The multi line delimiter is not the last character of this field   */
        /* It shall be necessarily in the middle                              */
        withinQuotes = true;    /* multi line mode -> we start with withinQuotes = true */
        r = p;
        while ( (r!=NULL) && (*r!='\0') )
        {
            if (*r==multiLine)
                withinQuotes = withinQuotes?false:true;
            if ( (*r==separator) && (withinQuotes==false) )
            {   /* we have found an actual separator, i.e. not enclosed within delimiters   */
                /* in this case we attach the text until the separator to the current field */
                /* then switch to single line mode */
                *r = '\0';
                if ( (r!=p)&&(*(r-1)==multiLine) )  /* if the char that precedes the separator */
                    *(r-1) = '\0';                  /* is the closing delimiter, eliminate it  */
                strcat (fields[*fieldNo],p);
                (*fieldNo) += 1;
                strcpy (fields[*fieldNo],"");
                p = r+1;
                *pp = p;
                return (false);
            }   /*if ( (*r==separator) && (withinQuotes==false) ) */
            r += 1;
        }   /* while ( (r!=NULL) && (*r!='\0') ) */
        strcat (fields[*fieldNo],p);
        strcat (fields[*fieldNo],"\n");
        p = r;

    }   /* while ( (p!=NULL) && (*p!='\0') ) */

    /* The line is finished without switching to single line mode */
    *pp = p;
    return (true);
}


/********************************************************************************************/
/* This function appends to the output file a new section, obtained by copy/paste of the    */
/* input markdown template, with all placeholder properly substitued by the content of the  */
/* fields obtained from parsing the current csv input file                                  */
/* Please note that the output file shall be opened by the calling function before entering */
/* appendCsvRow2Output(), while the input template is opened and closed directly here       */
/********************************************************************************************/
static void appendCsvRow2Output (FILE *outputMdFd, char *inputMdTemplate, char placeHolder, int fieldNo, fieldString fields[])
{
    /* Local Variables */
    FILE       *inputMdTemplateFd;
    lineString  currentLine;
    char       *p, *q;
    int         currentField;

    /* Open the markdown template file in read mode */
    if ( (inputMdTemplateFd=fopen(inputMdTemplate,"r"))==NULL )
    {
        printf ("Unable to open input Markdown Template... Aborting\n\n");
        exit (-1);
    }

    /* Append the content of the input template to the output */
    /* file substituting placeholders with the content of the */
    /* corresponding fields                                   */
    while ( fgets(currentLine,MAXLINELEN,inputMdTemplateFd) )
    {
        currentLine[strcspn(currentLine, "\r\n")] = '\0';   /* Remove trailing CR, LF, CRLF, LFCR, etc.    */
        p = currentLine;
        while ( (q=strchr(p,placeHolder))!=NULL )
        {
            *q = '\0';
            fprintf (outputMdFd,"%s",p);
            currentField = 0;
            q += 1;
            while ( ((*q)!='\0') && isdigit(*q) )
            {
                currentField = 10*currentField + (int)(*q -'0');
                q += 1;
            }   /* while ( ((*q)!='\0') && isdigit(*q) ) */

            if ( (currentField<=0) || (currentField>fieldNo+1) )
            {
                printf ("The template contains a placeholder (%c%d) that refers to a non-existing field... Aborting\n\n",placeHolder,currentField);
                exit (-1);
            }
            fprintf (outputMdFd,"%s",fields[currentField-1]);
            if ( *q=='\0')
                p = q;
            else
                p = q+1;
        }
        fprintf (outputMdFd,"%s\n",p);

    }   /* while ( fgets(currentLine,MAXLINELEN,inputMdTemplateFd) ) */

    fprintf (outputMdFd,"\n");
    fclose (inputMdTemplateFd);

    return;
}


/***************************************************************************/
/* This is a function written for debug purposes and reused with -d option */
/***************************************************************************/
static void currentRowPrintFields (int fieldNo, fieldString fields[])
{
    int i;

    for (i=0; i<fieldNo; i++)
        printf ("Field %d:\t%s\n", i+1, fields[i]);
    printf ("\n");

    return;
}


/*****************
 * Main Function *
 *****************/
int main(int argc, char *argv[], char *envp[])
{
    /* Local Variables */
    int             i,
                    lineNo,
                    fieldNo,
                    chapterFieldNo,
                    altSyntax = UNDEFINED;
    filenameString  inputCsvFile = "",
                    inputMdTemplate = "",
                    outputMdFile = "",
                    inputMdChapterTemplate = "";
    lineString      currentLine;
    fieldString     fields[MAXFIELDS],
                    lastChapter = "";
    FILE           *inputCsvFd,
                   *outputMdFd,
                   *inputMdChapterTemplateFd;
    bool            skipHeader = DEFHEADER,
                    appendMode = DEFAPPEND,
                    firstLine,
                    multiLineOpen;
    char           *p,
                    separator = DEFSEPARATOR,
                    comment = DEFCOMMENT,
                    placeHolder = DEFPLACEHLDR,
                    multiLine = DEFMULTILINE;

    /* Preliminary Checks - Check Command Layout & Parse Input Parameters */
    if ( (argc==2) && (strcmp(argv[1],"-h")==0) )
    {
        /* Print complete help and exit */
        printHelp();
        exit (0);
    }

    if ( (argc<3)||(argc>17) )
    {
        printUsage();
        exit (-1);
    }
    for (i=1;i<argc;i++)
    {
        if (argv[i][0]!='-')
        {
            printUsage();
            exit (-1);
        }   /* if (argv[i][0]!='-') */
        switch (argv[i][1])
        {
            case 'd':
            {
                i +=1;
                if ( (altSyntax==STANDARD) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax = DECODEHDR;
                skipHeader = false;     /* reset in any case skipHeader flag */
                strcpy (inputCsvFile,argv[i]);
                break;
            }   /* case 'd': */
            case 'i':
            {
                i +=1;
                if ( (altSyntax==DECODEHDR) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                strcpy (inputCsvFile,argv[i]);
                break;
            }   /* case 'i': */
            case 'o':
            {
                i +=1;
                if ( (altSyntax==DECODEHDR) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                strcpy (outputMdFile,argv[i]);
                break;
            }   /* case 'o': */
            case 't':
            {
                i +=1;
                if ( (altSyntax==DECODEHDR) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                strcpy (inputMdTemplate,argv[i]);
                break;
            }   /* case 't': */
            case 'n':
            {
                if (altSyntax==DECODEHDR)
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                skipHeader = false;
                break;
            }   /* case 'n': */
            case 'a':
            {
                if (altSyntax==DECODEHDR)
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                appendMode = true;
                break;
            }   /* case 'a': */
            case 's':
            {
                i +=1;
                if ( (altSyntax==DECODEHDR) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                separator = argv[i][0];
                break;
            }   /* case 's': */
            case 'p':
            {
                i +=1;
                if ( (altSyntax==DECODEHDR) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                placeHolder = argv[i][0];
                break;
            }   /* case 'p': */
            case 'r':
            {
                i +=1;
                if ( (altSyntax==DECODEHDR) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                comment = argv[i][0];
                break;
            }   /* case 'r': */
            case 'c':
            {
                i +=1;
                if ( (altSyntax==DECODEHDR) || (i>=argc) )
                {
                    printUsage();
                    exit (-1);
                }
                altSyntax=STANDARD;
                strcpy (inputMdChapterTemplate,argv[i]);
                break;
            }   /* case 'c': */
            default:
            {   /* Unexpected option */
                printUsage();
                exit(-1);
            }   /* default */
        }   /* switch (argv[i][1]) */
    }

    /* Check that mandatory parameters have been specified */
    if (inputCsvFile[0]=='\0')
    {
        if (altSyntax==DECODEHDR)
            printf ("Missing mandatory input csv file (... -d <csv_input_file>)... Aborting\n\n");
        else
            printf ("Missing mandatory input csv file (... -i <csv_input_file>)... Aborting\n\n");
        exit (-1);
    }
    if ( (altSyntax==STANDARD) && (outputMdFile[0]=='\0') )
    {
        printf ("Missing mandatory output markdown file (... -o <md_output_file>)... Aborting\n\n");
        exit (-1);
    }
    if ( (altSyntax==STANDARD) && (inputMdTemplate[0]=='\0') )
    {
        printf ("Missing mandatory markdown teplate (... -t <md_template>)... Aborting\n\n");
        exit (-1);
    }

    /* Open Input and Output Files */
    if ( (inputCsvFd=fopen(inputCsvFile,"r"))==NULL )
    {
        printf ("Unable to open input CSV File... Aborting\n\n");
        exit (-1);
    }
    if (altSyntax==STANDARD)
    {
        if (appendMode)
            outputMdFd=fopen(outputMdFile,"a");
        else
            outputMdFd=fopen(outputMdFile,"w");
        if (outputMdFd==NULL)
        {
            printf ("Unable to open output Markdown File... Aborting\n\n");
            exit (-1);
        }
    }   /* if (altSyntax==STANDARD) */


    /* If a chapter markdown template has been specified, extract the corresponding placeholder */
    /* from it (we consider only the first in case that more than one are present in the file). */
    /* Save this value to the chapterFieldNo variable, that will be used in conjunction with    */
    /* lastChapter variable to print out the new chapter whenever needed                        */
    chapterFieldNo = 0;
    if ( (altSyntax==STANDARD) &&(inputMdChapterTemplate[0]!='\0') )
    {
        if ( (inputMdChapterTemplateFd=fopen(inputMdChapterTemplate,"r"))==NULL )
        {
            printf ("Unable to open the input Markdown Chapter Template... Aborting\n\n");
            exit (-1);
        }
        while (fgets(currentLine,MAXLINELEN,inputMdChapterTemplateFd))
        {
            if ( (p=strchr(currentLine,placeHolder))==NULL)
                continue;
            p += 1;
            while ( ((*p)!='\0') && isdigit(*p) )
            {
                chapterFieldNo = 10*chapterFieldNo + (int)(*p -'0');
                p += 1;
            }   /* while ( ((*p)!='\0') && isdigit(*p) ) */
            break;
        }   /* while (fgets(currentLine,MAXLINELEN,inputMdChapterTemplateFd)) */
        fclose (inputMdChapterTemplateFd);
    }   /* if ( (altSyntax==STANDARD) &&(inputMdChapterTemplate[0]!='\0') ) */

    /* Start Parsing CSV Input Line-by-Line */
    lineNo = 0;
    firstLine= true;
    multiLineOpen = false;
    while ( fgets(currentLine,MAXLINELEN,inputCsvFd) )
    {
        lineNo += 1;
        currentLine[strcspn(currentLine, "\r\n")] = '\0';   /* Remove trailing CR, LF, CRLF, LFCR, etc.    */
        removeDoubleMultiLineChar (currentLine,multiLine);  /* Remove double occurences of multi line char */

        p = currentLine;

        if (multiLineOpen==false)
        {   /* The line we just started parsing is not part of a multi line */
            fieldNo = 0;                        /* Reset the current field counter */
            while ( (*p==' ') || (*p=='\t') )   /* Skip leading spaces and tabs (if any) */
                p++;
            if ( (*p=='\0') || (*p==comment) )  /* Check whether this is an empty line or a */
                continue ;                      /* comment and, if so, skip this line       */

            if ( (firstLine) && (skipHeader) )
            {   /* skipHeader flag is enabled and this is the first valid line, so skip it */
                /* (note that if altSyntax==DECODEHDR, then skipHeader is forced to false, */
                /* so that we never enter here if option -d was specified on command line) */
                firstLine = false;
                continue;
            }   /* if ( (firstLine) && (skipHeader) ) */
            firstLine = false;
        }   /* if (multiLineOpen==false) */
        else
        {   /* here multiLineOpen==true */
            if ((p==NULL) || (*p=='\0'))
            {   /* Bug fixing - if the multiline starts with an empty line, it means that we */
                /* have to add this empty line in the current field, otherwise some markdown */
                /* format may not be properly reproduced in the output (e.g. bullets, etc.)  */
                strcat (fields[fieldNo],"\n");
            }   /* if (*p=='\0') */
        }   /* else if (multiLineOpen==false) */


        /* Here p points to the first valid character in the current csv input line */
        /* It may either be the first non-space character of a single line or the   */
        /* first character of a multi line (note that space characters are allowed  */
        /* at the beginning of a line that is part of a multi line)                 */
        /* Scan the line char-by-char until the end and handle it differently based */
        /* on being or not being within a multi line (observe that a multi line may)*/
        /* begin and end in the same line)                                          */

        while ( (p!=NULL) && (*p!='\0') )
        {
            if (multiLineOpen)
                multiLineOpen = scanMultiLine(&p,separator,multiLine,lineNo,&fieldNo,fields);
            else
                multiLineOpen = scanSingleLine(&p,separator,multiLine,lineNo,&fieldNo,fields);
        }   /* while ( (p!=NULL) && (*p!='\0') ) */

        /* The row is terminated. If there is no multi line ongoing, write fields just extracted */
        /* into the output file (using the template(s) and substituting placeholders). Observe   */
        /* that in case altSyntax==DECODEHDR, the behaviour is different, since curent fields    */
        /* are printed to screen and then the execution is terminated (i.e. this is done only    */
        /* once for the the first line)                                                          */
        if (multiLineOpen == false)
        {   /* The csv row is terminated and this is not a multi line */
            if (altSyntax==DECODEHDR)
            {   /* option -d was specified and we have just isolated into fields[] array */
                /* the content of the first valid line in the input csv file             */
                /* Print them once and exit                                              */
                currentRowPrintFields(fieldNo,fields);
                fclose (inputCsvFd);
                exit (0);
            }   /* if (decodeHeader) */

            if ( (chapterFieldNo>=1) && (chapterFieldNo<=fieldNo) && (strcmp(lastChapter,fields[chapterFieldNo-1])) )
            {   /* if a valid chapterFieldNo was extracted before from inputMdChapterTemplate file  */
                /* and in the current row this field changed with respect to the previous row, then */
                /* add a new chapter formatted according to the inputMdChapterTemplate              */
                appendCsvRow2Output (outputMdFd,inputMdChapterTemplate,placeHolder,fieldNo,fields);
                strcpy (lastChapter,fields[chapterFieldNo-1]);
            }
            appendCsvRow2Output (outputMdFd, inputMdTemplate, placeHolder,fieldNo,fields);
        }   /* if (multiLineOpen == false) */

    }   /* while ( fgets(currentLine,MAXLINELEN,inputCsvFd) ) */

    /* Processing terminated    */
    /* Close all files and exit */
    fclose (inputCsvFd);
    fclose (outputMdFd);

    exit (0);
}
