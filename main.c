#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int	get_values(int *currLevel, int *maxStam, int *islandStam, int *currStam, int *xpLeft, int *stamPerRun, int *xpPerStam, int *timePerRun)
{
	printf("Insert your current Pirate Level (1-1999):\n");
	if (!scanf("%d", currLevel))
		return (0);
	printf("Insert your maximum stamina:\n");
	if (!scanf("%d", maxStam))
		return (0);
	printf("Insert bonus stamina earned from your Stamina Island (0-60):\n");
	if (!scanf("%d", islandStam))
		return (0);
	printf("Insert your current stamina:\n");
	if (!scanf("%d", currStam))
		return (0);
	printf("Insert the XP left to Level UP:\n");
	if (!scanf("%d", xpLeft))
		return (0);
	printf("Insert the total stamina 1 run costs:\n");
	if (!scanf("%d", stamPerRun))
		return (0);
	printf("Insert the amount of XP you earn per Stamina (XP received after a run / Stamina spent per run):\n");
	if (!scanf("%d", xpPerStam))
		return (0);
	printf("Insert the time you take to do 1 run (Add reward screen time etc, all in seconds, 0 if don't know):\n");
	if (!scanf("%d", timePerRun))
		return (0);
	return (1);
}

int	fill_levelTab(int *levelTab, FILE *stream)
{
	char	*s;
	int		i = 0;
	size_t	len = 0;

	if (!s)
		return (0);
	while (i < 1999 && getline(&s, &len, stream))
	{
		if (!s || !len)
			return (0);
		levelTab[i] = atoi(s);
		++i;
		free(s);
		s = 0;
	}
	free(s);
	return (1);
}

int	main(void)
{
	int		*levelTab = 0;
	int		currLevel = 0;
	int		maxStam = 0;
	int		islandStam = 0;
	int		currStam = 0;
	int		xpLeft = 0;
	int		stamPerRun = 0;
	int		xpPerStam = 0;
	int		timePerRun = 0;
	FILE	*stream = fopen("XPToNextLevel.txt", "r");

	if (!stream)
		return (write(2, "Can't open XPToNextLevel.txt\n", 29));
	if (!(levelTab = malloc(sizeof(int) * 1999)))
		return (write(2, "Can't Malloc levelTab\n", 22));
	if (!fill_levelTab(levelTab, stream))
	{
		free(levelTab);
		fclose(stream);
		return (write(2, "Can't fill levelTab\n", 20));
	}
	else
		write(1, "levelTab initialized\n", 21);
	fclose(stream);
	
	if (!get_values(&currLevel, &maxStam, &islandStam, &currStam, &xpLeft, &stamPerRun, &xpPerStam, &timePerRun))
	{
		free(levelTab);
		return (write(2, "Incorrect values\n", 17));
	}

	//Calculate LevelUps
	int	xpPerRun = stamPerRun * xpPerStam;
	int	totalTime = 0;
	int	runs = 0;
	int	xpGained = 0;
	int	totalRuns = 0;

	printf("\nP-lvl = %d: maxStam =\t%d, currStam =\t%d, XP->(%d/%d)",
			currLevel, maxStam, currStam, levelTab[currLevel - 1] - xpLeft, levelTab[currLevel - 1]);
	while (xpPerStam * currStam > xpLeft && currLevel < 1999)
	{
		runs = xpLeft / xpPerRun + 1;
		totalRuns += runs;
		xpGained = runs * xpPerRun;
		totalTime += timePerRun * runs;
		if (runs * stamPerRun > currStam)
		{
			printf("\nYou don't have enough to finish the last run to level up, but you technically have enough stamina to level up.\n");
			break ;
		}
		currStam -= runs * stamPerRun;
		currStam += maxStam;
		xpLeft = levelTab[currLevel] - (runs * xpPerRun - xpLeft);
		xpGained -= xpLeft;
		++currLevel;
		maxStam += currLevel % 2 == 0 ? 0 : 1;
		while (xpGained >= levelTab[currLevel])
		{
			xpGained -= levelTab[currLevel];
			currStam += maxStam;
			++currLevel;
			maxStam += currLevel % 2 == 0 ? 0 : 1;
			xpLeft = levelTab[currLevel] - xpGained;
		}
			// printf("\nxpleft %d\n", xpLeft);
		printf("\nP-lvl = \033[0;32m%4d\033[0m:\tRuns = \033[0;32m%2d\033[0m,    maxStam = \033[0;32m%d\033[0m,\
	currStam = \033[0;32m%5d\033[0m,    XP->(%5d/%d),   Time: %dh%dm%ds,\tTotal Time: \033[0;32m%d\033[0mh\033[0;32m%d\033[0mm\033[0;32m%d\033[0ms",
			currLevel, runs, maxStam, currStam, levelTab[currLevel - 1] - xpLeft, levelTab[currLevel - 1],
			timePerRun * runs / 3600, timePerRun * runs / 60 % 60, timePerRun * runs % 60,
			totalTime / 3600, totalTime / 60 % 60, totalTime % 60); // h:m:s
	}
	printf("\nTotal Runs: %d, Total Time: \033[0;32m%d\033[0mh\033[0;32m%d\033[0mm\033[0;32m%d\033[0ms\n",
		totalRuns, totalTime / 3600, totalTime / 60 % 60, totalTime % 60);
	if (xpPerStam * currStam < xpLeft)
		printf("You can't level up with your current stamina.\n");
	
	free(levelTab);
	return (0);
}
