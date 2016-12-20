#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <utmpx.h>

int main(int argc, char *argv[])
{
    //variables
    int pflag = 0;
    int sflag = 0;

    
    char *since;
    //variables since
    int lenSince = 20;

    char strSince[lenSince];
   
    time_t nowSince;

    time_t startSince;
  
    struct tm struStart;

    struct sysinfo struInfo;
    //variables uptime

    int str_len2 = 512;
    //char *strTime;
    char *strTime;

    char *strUptime;
    //char *str_res2;
    char *uptimeResult;
    long valeurUptime;
    //struct sysinfo struInfo;
    long minute = 60;
    long hour = minute * 60;
    long day = hour * 24;
    long week = 7 * day;
    long year = 365 * day;
    long secondes, minutes, hours, days, weeks, years;

    //variable nowTime
    char *nowTime;
    int lenNow = 100;
    char strNow[lenNow];
    time_t now;
    struct tm struNow;
////-> check
    //variables nb user
    int nbUsers = 0;
    struct utmpx *user_info;

    //variables getload

    int lenload = 512;
    char *strload;
    char *load;
    //struct sysinfo struInfo;

    //gestion d'erreurs
    char *prog_name;
    prog_name = argv[0];
    //flags ps

    int c;
    struct option options[] = 
    {
        {"pretty", no_argument, NULL, 'p'},
        {"since", no_argument, NULL, 's'},
        {0, 0, 0, 0}
    };


    while ((c = getopt_long(argc, argv,"ps", options, NULL)) != -1) {
        switch (c) {
            case 'p':
                pflag = 1;
                break;
            case 's':
                sflag = 1;
                break;
            default:
                abort();
        }
    }
    //Retourner since
    //a mettre a la fin
    if (sflag) {


    if (sysinfo(&struInfo) == -1)
        return EXIT_FAILURE;

    nowSince = time(NULL);
    if (nowSince == -1)
        return EXIT_FAILURE;

    startSince = nowSince - struInfo.uptime;

    if (localtime_r(&startSince, &struStart) == NULL)
        return EXIT_FAILURE;

    if (strftime(strSince, lenSince, "%Y-%m-%d %H:%M:%S", &struStart) < 0)
        return EXIT_FAILURE;


    since = strdup(strSince);//////////??????
    if (since == NULL) {
            perror(prog_name);
            exit(EXIT_FAILURE);
        }
    printf("%s\n", since);
    free(since);
    exit(EXIT_SUCCESS);
    }
////////////////////////////////////////////////////////


    /*int str_len2 = 512;
    char *strTime;
    char *strUptime;
    char *uptimeResult;
    long uptime;
    struct sysinfo struInfo;
    long minute = 60;
    long hour = minute * 60;
    long day = hour * 24;
    long week = 7 * day;
    long year = 365 * day;
    long secondes, minutes, hours, days, weeks, years;*/

    strUptime = calloc(str_len2, sizeof(char));
    if (strUptime == NULL)
        return EXIT_FAILURE;

    strTime = calloc(str_len2, sizeof(char));
    if (strTime == NULL)
        return EXIT_FAILURE;

    if (sysinfo(&struInfo) == -1)
        return EXIT_FAILURE;

    valeurUptime = struInfo.uptime;

    years = valeurUptime / year;
    weeks = (valeurUptime % year) / week;
    // les jours sont différents avec le pflag
    hours = (valeurUptime % day) / hour;
    minutes = (valeurUptime % hour) / minute;

    if (pflag) {
        days = (valeurUptime % week) / day;
        // years
        if (years) {
            sprintf(strUptime, "%ld years,", years);
            strcat(strTime, strUptime);
        }
        // Weeks
        if (weeks) {
            sprintf(strUptime, " %ld weeks,", weeks);
            strcat(strTime, strUptime);
        }
        // days
        if (days) {
            sprintf(strUptime, " %ld days,", days);
            strcat(strTime, strUptime);
        }
        // Hours
        if (hours) {
            sprintf(strUptime, " %ld hours,", hours);
            strcat(strTime, strUptime);
        }
        // Minutes
        if (minutes) {
            sprintf(strUptime, " %02ld minutes", minutes);
            strcat(strTime, strUptime);
        }
    } else {
        days = (valeurUptime % year) / day;
        // years
        if (years) {
            sprintf(strUptime, "%ld years, ", years);
            strcat(strTime, strUptime);
        }
        // days
        if (days) {
            sprintf(strUptime, "%ld days, ", days);
            strcat(strTime, strUptime);
        }
        // Hours
        if (hours) {
            sprintf(strUptime, "%ld:", hours);
            strcat(strTime, strUptime);
        }
        // Minutes
        if (minutes) {
            if (hours) 
                sprintf(strUptime, "%02ld", minutes);
            else
                sprintf(strUptime, "%02ld minutes", minutes);

            strcat(strTime, strUptime);
        }
    }

    free(strUptime);
    uptimeResult = strdup(strTime);
    free(strTime);

    if (uptimeResult == NULL) {
        perror(prog_name);
        exit(EXIT_FAILURE);
    }
    // a mettre a la fin
    if (pflag) {
        printf("%s\n", uptimeResult);
        free(uptimeResult);
        exit(EXIT_SUCCESS);
    }
    //////////////////////////////////////

    /*int lenNow = 100;
    char strNow[lenNow];
    time_t now_raw;
    struct tm struNow;*/

    now = time(NULL);
    if (now == -1)
        return EXIT_FAILURE;

    if (localtime_r(&now, &struNow) == NULL)
        return EXIT_FAILURE;

    if (strftime(strNow, lenNow, "%H:%M:%S", &struNow) < 0)
        return EXIT_FAILURE;

    nowTime = strdup(strNow);
    if (nowTime == NULL) {
        perror(prog_name);
        exit(EXIT_FAILURE);
    }
//////////////////////////////////////


    setutxent();

   // nbUsers = 0;
    while ((user_info = getutxent()) != NULL) {
        if (user_info->ut_type == 7)
            nbUsers++;
    }
    // nbUsers = myuptime_count_connected_users();
    if (nbUsers == -1) {
        perror(prog_name);
        exit(EXIT_FAILURE);
    }
 ///////////////////////////////////

    strload = calloc(lenload, sizeof(char));
    if (strload == NULL)
        return EXIT_FAILURE;

    if (sysinfo(&struInfo) == -1)
        return EXIT_FAILURE;

    sprintf(strload,"%.2f, %.2f, %.2f", struInfo.loads[0] / (float)(1 << SI_LOAD_SHIFT),struInfo.loads[1] / (float)(1 << SI_LOAD_SHIFT),struInfo.loads[2] / (float)(1 << SI_LOAD_SHIFT));

    load = strdup(strload);

    free(strload);

    if (load == NULL) {
        perror(prog_name);
        exit(EXIT_FAILURE);
    }


    //affiche les résultats 
    printf("%s ", nowTime);

    printf("up %s, ", uptimeResult);

    if (nbUsers>1)
    {
        printf("%d users, ", nbUsers);
    }
    else{
        printf("%d user, ", nbUsers);
    }
    printf("load average: %s\n", load);

    free(nowTime);
    free(uptimeResult);
    free(load);

    return EXIT_SUCCESS;
}