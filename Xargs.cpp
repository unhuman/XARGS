#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

#define LINESIZE	1024

bool getline(char *line)
{
	char c[2];
	c[0] = '\0'; c[1] = '\0';
	
	strcpy(line, "");

	if (feof(stdin))
		return false;
	while (c[0] = fgetchar())
	{
		if ((c[0] == 10) || (c[0] == 13))
			break;

		if (c[0] == -1)
			return false;

		strcat(line, c);
	}	
	return true;
}

char* ReplaceBraces(char* lineIn, char* replaceString, bool bSwapLTGT)
{
	unsigned int i = 0;
	static char lineOut[LINESIZE];
	strcpy(lineOut, "\0");
	
	for (i = 0; i <= strlen(lineIn); i++)
	{
		char currCh[2];
		currCh[0] = '\0'; currCh[1] = '\0';
		currCh[0] = lineIn[i];
		if (currCh[0] == '{')
		{
			char matchString[LINESIZE];
			strcpy (matchString, "\0");

			while ((currCh[0] = lineIn[++i]) != '}')
			{
				//If we emptied out the string and never found a match,
				//or encountered a '{' return NULL
				if ((currCh[0] == '\0') || (currCh[0] == '{'))
					return NULL;
				else 
					strcat(matchString, currCh);
			}
			//If there's no string to match, copy the entire replace string
			char *matchFound = strstr(replaceString, matchString);
			if (!strlen(matchString))
			{
				strcat(lineOut, replaceString);
			}
			else //copy only up to the string we found
			{
				char currChReplace[2];
				currChReplace[0] = '\0'; currChReplace[1] = '\0';

				for (unsigned int j = 0; j < strlen(replaceString); j++)
				{
					if (&replaceString[j] == matchFound)
					{
						break;
					}
					else
					{
						currChReplace[0] = replaceString[j];
						strcat(lineOut, currChReplace);
					}
				}
			}

		}
		else
		{			
			strcat(lineOut, currCh);
		}
	}

	if (bSwapLTGT)
	{
		for (i = 0; i < strlen(lineOut); i++)
		{
			if (lineOut[i] == '<')
				lineOut[i] = '[';
			if (lineOut[i] == '>')
				lineOut[i] = ']';
		}
	}

	return lineOut;
}

void main(int argc, char *argv[ ], char *envp[ ])
{
	char commandline[LINESIZE];
	char command[LINESIZE];	
	char input[LINESIZE];
	bool bQuiet = true;
	bool bSwapLTGT = true;	

	//Parse out command line parameters
	int iStart = 1;
	while ((iStart < argc) && (argv[iStart][0] == '-')) 
	{
		if ((!strnicmp(argv[iStart], "-v", LINESIZE)) ||
			(!strnicmp(argv[iStart], "/v", LINESIZE)))
		{			
			bQuiet = true;
		}
		if ((!strnicmp(argv[iStart], "-s", LINESIZE)) ||
			(!strnicmp(argv[iStart], "/s", LINESIZE)))
		{			
			bSwapLTGT = false;
		}
		iStart++;
	}

	//if there's no parameters after parsing, then bail out
	if (iStart >= argc)
	{
		printf("XARGS:  Runs a program reading information from stdin\n");
		printf("Format: XARGS [params] progname params\n");
		printf("        Replace a parameter with {} to substitute information read in\n");
		printf("        (for example, \"dir .. | xargs echo {}\")\n");
		printf("        NOTE: '<' and '>' will be replaced with '[' and ']'\n");
		printf("        -s option will suspend the '<' '>' switch (use at your own risk).\n");
		printf("        -v option for verbose mode\n");
		return;
	}

	
	do {
		strcpy(commandline, "");
		strcpy(command, "");		

		if (!getline(input))
			break;
		else {
			//Build up a command line
			for (int i = iStart; i < argc; i++)
			{
				//Add a space between items
				if (i > iStart)
					strcat(commandline, " ");

				strcat(commandline, argv[i]);
			}

			char* ReplacedString = ReplaceBraces(commandline, input, bSwapLTGT);
			if (ReplacedString == NULL)
			{
				printf("XARGS:  Nesting braces (ie '{{}}') not allowed\n");
				printf("        All opening braces '{' must have matching closing brace '}'\n");
				return;
			}
			else
			{
				strcpy(command, ReplacedString);
			}
		}

		//Only run if something is there
		if (strlen(command))
		{
			//Only output if we're not quiet
			if (!bQuiet)
				printf("Executing: %s\n", command);

			//Launch for each item
			int iRunRC = 0;
			iRunRC = _spawnlp(_P_WAIT, "command", "command", "/c", command, NULL, NULL);			

			if (iRunRC == -1)
				printf("ERROR: %s", command);
		}

	} while (!feof(stdin));
}