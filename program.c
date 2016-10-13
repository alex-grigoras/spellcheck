#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_WORDS 8000

//From https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
int levenshtein(char *s1, char *s2) {
    unsigned int x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int matrix[49][49];
    matrix[0][0] = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));

    return(matrix[s2len][s1len]);
}

int ReadDict(char* dictionary[], int frequencies[])
{
	FILE* f = fopen("dict.txt", "r");

	char word[50];
	int freq;
	int count = 0;
	while(fscanf(f, "%s %d\n", word, &freq) > 0)
	{
		dictionary[count] = (char*) malloc(strlen(word)+1);
		strcpy(dictionary[count], word);
		frequencies[count] = freq;
		count++;
	}

	return count;
}

int GetClosestDistance(char word[], char* dictionary[], int dictionarySize, int* index)
{
	int bestDist = INT_MAX;
	*index = -1;
	for(int i=0; i<dictionarySize; i++)
	{
		int dist = levenshtein(word, dictionary[i]);
		if(dist < bestDist)
		{
			bestDist = dist;
			*index = i;
		}
	}

	return bestDist;
}

struct sol
{
	int bestDistance;
	int wordCount;
	int frequency;
	int words[100];
};

bool IsCurrentBetter(sol currentSol, sol bestFoundSol)
{
	if(currentSol.bestDistance < bestFoundSol.bestDistance)
	{
		return true;
	}
	else if(currentSol.bestDistance == bestFoundSol.bestDistance)
	{
		if(currentSol.wordCount < bestFoundSol.wordCount)
		{
			return true;
		}
		else if(currentSol.wordCount == bestFoundSol.wordCount)
		{
			if(currentSol.frequency > bestFoundSol.frequency)
			{
				return true;
			}
		}
	}

	return false;
}

void main()
{
	char str[65] = "schaproblmcanbeeasilisolved";
	char* dictionary[MAX_WORDS];
	int frequencies[MAX_WORDS];
	sol sols[64][64];

	int dictSize = ReadDict(dictionary, frequencies);
	int len = strlen(str);

	for(int i=0; i<len; i++)
	{
		int index;
		int dist;
		int frequency;
		char* ii = (char*) malloc(i-i+1+1);
		strncpy(ii, str+i, i-i+1);
		*(ii+i-i+1) = '\0';
			
		sol currentSol;
		currentSol.bestDistance = GetClosestDistance(ii, dictionary, MAX_WORDS, &index);
		currentSol.frequency = frequencies[index];
		currentSol.wordCount = 1;
		currentSol.words[0] = index;

		sols[i][i] = currentSol;
	}


	for(int diag=1; diag<len; diag++)
	{
		for(int elem = 0; elem<len-diag; elem++)
		{
			int i = elem;
			int j = elem + diag;
			int minDist = INT_MAX, minWords, minFreq;
			
			sol bestFoundSol;
			bestFoundSol.bestDistance = INT_MAX;
			bestFoundSol.wordCount = INT_MAX;
			bestFoundSol.frequency = 0;
			int bestK;
			for(int k=i; k<j; k++)
			{
				sol currentSolution;
				currentSolution.bestDistance = sols[i][k].bestDistance + sols[k+1][j].bestDistance;
				currentSolution.wordCount = sols[i][k].wordCount + sols[k+1][j].wordCount;
				currentSolution.frequency = sols[i][k].frequency + sols[k+1][j].frequency;

				if(IsCurrentBetter(currentSolution, bestFoundSol))
				{
					bestFoundSol = currentSolution;
					bestK = k;
				}
			}

			int index;
			int dist;
			int frequency;
			char* ij = (char*) malloc(j-i+1+1);
			strncpy(ij, str+i, j-i+1);
			*(ij+j-i+1) = '\0';

			sol currentSol;
			currentSol.bestDistance = GetClosestDistance(ij, dictionary, MAX_WORDS, &index);
			currentSol.frequency = frequencies[index];
			currentSol.wordCount = 1;
			currentSol.words[0]=index;

			if(IsCurrentBetter(currentSol, bestFoundSol))
			{
				sols[i][j] = currentSol;
			}
			else
			{
				for(int x=0; x<sols[i][bestK].wordCount; x++)
				{
					bestFoundSol.words[x] = sols[i][bestK].words[x];
				}

				for(int x=0; x<sols[bestK+1][j].wordCount; x++)
				{
					bestFoundSol.words[x +sols[i][bestK].wordCount] = sols[bestK+1][j].words[x];
				}
				sols[i][j] = bestFoundSol;
			}
		}
	}

	sol bestSol = sols[0][len-1];
	for(int i=0; i<bestSol.wordCount; i++)
	{
		printf("%s ", dictionary[bestSol.words[i]]);
	}
	printf("\n");
	system("pause");

}

