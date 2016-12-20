
#include <utmpx.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
    //variables flag
    int pflag = 0;
    int sflag = 0;

  
    //variables uptime
    long secondes, minutes, hours, days, weeks, years;
    long valeurUptime;
    long minute = 60;
    long hour = minute * 60;
    long day = hour * 24;
    long week = 7 * day;
    long year = 365 * day;
    int lenUpime = 1000;
    char *strTime;
    char *strUptime;
    char *uptimeResult;
    
    //variable nowTime
    char *nowTime;
    int lenNow = 100;
    char strNow[lenNow];
    struct tm struNow;
    time_t now;
    

    //variables nb user
    struct utmpx *user_info;
    int nbUsers = 0;
    
    //variables getload
    int lenload = 1000;
    char *strload;
    char *load;

    //variables since
    int lenSince = 100;
    char *since;
    char strSince[lenSince]; 
    struct tm struStart;
    struct sysinfo struInfo; 
    time_t nowSince;
    time_t startSince;

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
    //Temps actuel & temps-P
    //
    //on va allouer de la place pour stocker notre donnée de temps
    strUptime = calloc(lenUpime, sizeof(char));
    if (strUptime == NULL)
        return EXIT_FAILURE;

    strTime = calloc(lenUpime, sizeof(char));
    if (strTime == NULL)
        return EXIT_FAILURE;

    if (sysinfo(&struInfo) == -1)
        return EXIT_FAILURE;

    years = valeurUptime / year;
    weeks = (valeurUptime % year) / week;
    hours = (valeurUptime % day) / hour;
    minutes = (valeurUptime % hour) / minute;
    valeurUptime = struInfo.uptime;
    // les semeines sont différents avec le pflag
    if (pflag) {
        days = (valeurUptime % week) / day;
        if (years) {
           // on va concatener la chaine qu'on veut afficher avec les bonnes infos de temps
            sprintf(strUptime, "%ld years,", years);
            strcat(strTime, strUptime);
        }
        if (weeks) {
            sprintf(strUptime, " %ld weeks,", weeks);
            strcat(strTime, strUptime);
        }
        if (days) {
            sprintf(strUptime, " %ld days,", days);
            strcat(strTime, strUptime);
        }
        if (hours) {
            sprintf(strUptime, " %ld hours,", hours);
            strcat(strTime, strUptime);
        }
        //on ne prends que les 2 premiers digit du long retourner par minute
        if (minutes) {
            sprintf(strUptime, " %02ld minutes", minutes);
            strcat(strTime, strUptime);
        }
    } else {
        days = (valeurUptime % year) / day;
        if (years) {
            sprintf(strUptime, "%ld years, ", years);
            strcat(strTime, strUptime);
        }
        if (days) {
            sprintf(strUptime, "%ld days, ", days);
            strcat(strTime, strUptime);
        }
        if (hours) {
            sprintf(strUptime, "%ld:", hours);
            strcat(strTime, strUptime);
        }
        if (minutes) {
            if (hours) 
                sprintf(strUptime, "%02ld", minutes);
            else
                sprintf(strUptime, "%02ld minutes", minutes);

            //on concatene le tout dans strTime
            strcat(strTime, strUptime);
        }
    }

    free(strUptime);
    //on copie le résultat dans uptimeResult
    uptimeResult = strdup(strTime);
    free(strTime);

    //on va vérifier que la str crée n'est pas vide
    if (uptimeResult == NULL) {
        perror(prog_name);
        exit(EXIT_FAILURE);
    }
    //Si on a pflag on va afficher la date bien formatée et on fini le programme ici en libérant la mémoire
    if (pflag) {
        printf("%s\n", uptimeResult);  
        exit(EXIT_SUCCESS);
        free(uptimeResult);
    }
    //temps actuel
    //
    //on prends le temps actuel
    now = time(NULL);
    if (now == -1)
        return EXIT_FAILURE;
    //on stock notre temps dans une tructure tm
    if (localtime_r(&now, &struNow) == NULL)
        return EXIT_FAILURE;
    //on choisi les éléments de la structure qu'on veut grace aux argument et on les inscrit sous forme de chaine de caractère
    if (strftime(strNow, lenNow, "%H:%M:%S", &struNow) < 0)
        return EXIT_FAILURE;
    //on fait une copie dans nowTime et on vrifie la valeur retournée en cas d'erreur
    nowTime = strdup(strNow);
    if (nowTime == NULL) {
        perror(prog_name);
        exit(EXIT_FAILURE);
    }
    //nbUser
    //
    //on va compter le nombre d'user (meme que poue la commande "Who")
    //on set le curseur au début de utempx
    setutxent();
    //on va vérifier si la structure n'est pas vide, si c'est vide ca renvoit NULL
    while ((user_info = getutxent()) != NULL) {
        //on filtre pour avoir les process normaux
        if (user_info->ut_type == 7)
            //on augmente le nombre d'user si on a passé le filtre 
            nbUsers++;
    }
    //Load
    //on va afficher la charge a 3 périodes de temps(1-10-15min)
    //
    //on alloue la place en mémoire
    strload = calloc(lenload, sizeof(char));
    if (strload == NULL)
        return EXIT_FAILURE;
    //on vérifie que la struct envoyée est bien une de type sysinfo
    if (sysinfo(&struInfo) == -1)
        return EXIT_FAILURE;
    //on va créer la string qu'on veut afficher en la retenant dans la chaine de caractère préalablement allouée "strload"
    //on affiche 2 digit apres la virgule  des float retournées par la structure, on les divise pour avoir un résultat "user friendly"
    sprintf(strload,"%.2f, %.2f, %.2f", struInfo.loads[0] / (float)(1 << SI_LOAD_SHIFT),struInfo.loads[1] / (float)(1 << SI_LOAD_SHIFT),struInfo.loads[2] / (float)(1 << SI_LOAD_SHIFT));
    //on clone la string dans la variable load
    load = strdup(strload);
    //on libère la mémoire
    free(strload);
    //on fait la vérification
    if (load == NULL) {
        perror(prog_name);
        exit(EXIT_FAILURE);
    }
     //Since
    //
    //si on a le flag since.
    if (sflag) {
    //on lui fournit une structure sysinfo
    if (sysinfo(&struInfo) == -1)
        return EXIT_FAILURE;
    //on prends le temps actuel
    nowSince = time(NULL);
    //on fait la différence entre la veleur stocké dans sysinfo de boot et le temps actuel
    startSince = nowSince - struInfo.uptime;
    //on stock le temps dans une structure tm de laquel on va pouvoir récuperer les informations du temps
    if (localtime_r(&startSince, &struStart) == NULL)
        return EXIT_FAILURE;
    //on place dans un string selon le format %Y-%m-%d %H:%M:%S"
    if (strftime(strSince, lenSince, "%Y-%m-%d %H:%M:%S", &struStart) < 0)
        return EXIT_FAILURE;
    //on fait une copie en mémoire cache de strSince dans la variable since 
    since = strdup(strSince);
    if (since == NULL) {
            perror(prog_name);
            exit(EXIT_FAILURE);
        }
    //Si il n'y a pas d'erreur on print le sice 
    printf("%s\n", since);
    free(since);
    exit(EXIT_SUCCESS);
    }

    //affiche les résultats 
    //
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

    return EXIT_SUCCESS;
}