//
// Created by Ian Liu on 2019/10/30.
//

#include "utility.h"

void splitFile( FILE* fp) {

    if(fp == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    size_t fileSize = 0;
    size_t currentRead = 0;
    size_t fileNum = 6;
    size_t expectRead = 0;
    size_t readRound = 0;
    int iter = 1;
    char fileName[15] = {0};

    fseek( fp, 0L, SEEK_END);
    fileSize = ftell(fp);
    expectRead = fileSize / fileNum;
    fseek( fp, 0, SEEK_SET);

    while(currentRead < fileSize) {
        sprintf( fileName, "split%d.rec", iter);
        readRound = readFile( fp, expectRead, fileName);
        if(readRound==-1)
            return;
        currentRead += readRound;
        iter++;
    }
}

size_t readFile( FILE *fp, size_t expectRead, char *fileName) {
    size_t len = 0;
    char *line = NULL;
    int flag = OFF;
    size_t currentRead = 0;
    char check[] = "@body";
    int lineCheck = 0;
    FILE *op = fopen(fileName,"a+");


    while ((lineCheck = getline(&line,&len,fp))!=-1) {

        if(strncmp( line, check, sizeof(check)-1) == 0)
            flag = ON;
        else if(flag == ON && strcmp(line,"@\n") == 0 ) {
            flag = OFF;
            if(currentRead > expectRead) {
                fseek( fp, -2, SEEK_CUR);
                free(line);
                line = NULL;
                return currentRead;
            }
        }

        currentRead += strlen(line);
        fprintf( op, "%s", line);
        free(line);
        line = NULL;
    }

    fclose(op);
    if(lineCheck==-1) return -1;
    return currentRead;
}

void help() {
    fprintf( stderr, "usuage: rqrep [-i/-rb/-l/-m] [paraOption] [-queryOption] [string] [file ...]\n");
    exit(EXIT_SUCCESS);
}

char *mystrstr( char *buffer, char *desire, bool Imode) {
    if(Imode == true) return strcasestr( buffer, desire);
    return strstr( buffer, desire);
}

char *mystrtok( char *tar, char *seg) {
    static char *lasttoken = NULL;
    char *tmp = NULL;

    if( tar == NULL) {
        tar = lasttoken;
        if( tar == NULL) return NULL;
    }

    tmp = strpbrk( tar, seg);
    if(tmp) {
        lasttoken = tmp+1;
        *tmp ='\0';
    }
    else {
        lasttoken = NULL;
    }
    return lasttoken;
}

int mystrncmp( char *s1, char *s2, size_t len, struct paraHandler *par) {
    if(par->i == true) return strncasecmp( s1,s2, len);
    return strncmp( s1, s2, len);
}

void paraInit(struct paraHandler *par) {
    par->i = false;
    par->l = false;
    par->m = false;
    par->rb = NULL;
}

size_t optionProcess( int argc, char **argv, struct paraHandler *par) {

    int iter = 0;
    int size = -1;
    int rbFlag = OFF;
    int mFlag = OFF;

    if(argc==1) {

    }

    for( iter = 1; iter<argc; iter++) {
        size = strlen(argv[iter]);
        if(size>3) {
            if(strcmp(argv[iter-1],"-rb")==0)
                continue;
            else if( strcmp( argv[iter-1],"-m")==0)
                continue;
            else break;
        }
    } // find the boundary

    for(int index = 1; index < iter; index++) {
        if(strcmp(argv[index],"-i") == 0) {
            par->i = true;
        }

        else if(strcmp(argv[index],"-rb") == 0) {
            par->rb = (char*)calloc( strlen(argv[index+1]), sizeof(char));
            memmove( par->rb, argv[index+1], strlen(argv[index+1]));
            rbFlag = ON;
        }

        else if(strcmp(argv[index],"-l") == 0) {
            par->l = true;
        }

        else if(strcmp(argv[index],"-m") == 0) {
            par->m = (char*)calloc( strlen(argv[index+1]), sizeof(char));
            memmove( par->m, argv[index+1], strlen(argv[index+1]));
            mFlag = ON;
        }

        else {

            if(rbFlag == ON) {
                rbFlag = OFF;
                continue;
            }

            else if(mFlag == ON) {
                mFlag = OFF;
                continue;
            }

            fprintf( stderr, "ERROR: Unrecognize parameter %s\n", argv[index]);
            exit(EXIT_FAILURE);
        }
    }

    return iter;
}

int queryProcess( int argc, char **argv, size_t offset, struct queryHandler *qu) {

    int flag = OFF;
    int start = -1;
    int end = -1;

    if(strcmp(argv[offset],"-simple") == 0) {
        qu->queryNum = 0;
        qu->offsetFile = offset+2;
        qu->pattern = (char*)calloc( strlen(argv[offset+1])+3, sizeof(char));
        memmove( qu->pattern, argv[offset+1], strlen(argv[offset+1]));
    }

    else if(strcmp(argv[offset],"-field") == 0) {
        qu->queryNum = 1;
        qu->pattern = (char*)calloc( strlen(argv[offset+1])+3, sizeof(char));
        memmove( qu->pattern, argv[offset+1], strlen(argv[offset+1]));
        qu->offsetFile = offset+2;
    }

    else if(strcmp(argv[offset],"-bool") == 0) {
        qu->queryNum = 2;
        qu->pattern = (char*)calloc( strlen(argv[offset+1])+3, sizeof(char));
        memmove( qu->pattern, argv[offset+1], strlen(argv[offset+1]));
        qu->offsetFile = offset+2;
    }

    else if(strcmp(argv[offset],"-multi") == 0) {
        qu->queryNum = 3;
        qu->pattern = (char*)calloc( strlen(argv[offset+1])+3, sizeof(char));
        memmove( qu->pattern, argv[offset+1], strlen(argv[offset+1]));
        qu->offsetFile = offset+2;
    }

    else {
        fprintf(stderr,"ERROR: Unrecognize query %s\n",argv[offset]);
        exit(EXIT_FAILURE);
    }
    return 1;
}

char** fileProcess( int argc, char **argv, size_t start) {
    char **file = (char**)malloc(sizeof(char*)*(argc-start));
    for(int iter = start; iter<argc; iter++) {
        file[iter-start] = (char*)calloc(strlen(argv[iter]), sizeof(char));
        memmove(file[iter-start],argv[iter],strlen(argv[iter]));
    }
    return file;
}

int stringHandler( struct paraHandler *par, char **file, int fileNum, char *desire) {
    FILE **fp = (FILE**)malloc(sizeof(FILE*)*fileNum);
    FILE *op = fopen( "specTest", "a+");
    int fileIndex = -1;
    char *buffer = NULL;
    size_t len = 0;
    char *ptr = NULL;
    int rbFlag = OFF;
    int mFlag = OFF;
    int a = -2;

    /*============ open file ==============*/
    for(int iter = 0; iter<fileNum; iter++) {
        fp[iter] = fopen( file[iter], "r+");
        if(fp[iter] == NULL) {
            fprintf( stderr, "ERROR: Unable to open %s\n", file[iter]);
            exit(EXIT_FAILURE);
        }
    }
    /*========== start to parse file ========*/
    for(fileIndex = 0; fileIndex<fileNum; fileIndex++) {
       while( getline(&buffer, &len, fp[fileIndex])!= -1) {
           // section for handling rb
           if(par->rb!=NULL) {
               if (  rbFlag == OFF && (strncmp( buffer, par->rb, strlen(par->rb))) != 0 ) continue;
               else if( rbFlag ==OFF && strncmp( buffer, par->rb, strlen(par->rb)) == 0 ) rbFlag = ON;
               else if( rbFlag == ON && (strcmp( buffer,"@\n") == 0 )) {
                   rbFlag = OFF;
                   continue;
               }
           }
           // section for handling m
           if(par->m != NULL) {
               if( mFlag == OFF && strncmp( buffer, par->m, strlen(par->m)) != 0 ) continue;
               else if( strncmp( buffer, par->m, strlen(par->m)) == 0) mFlag = ON;
               else if( mFlag == ON && strncmp( buffer, "@", strlen("@")) == 0) {
                   mFlag = OFF;
                   continue;
               }

           }

           // string pattern matching
           ptr = mystrstr(buffer,desire, par->i);
           while(ptr!=NULL) {
               if(par->l == true) a = fprintf( stderr, "%s:\t%s", file[fileIndex], buffer);
               else fprintf( stderr, "%s", buffer);
               ptr += strlen(desire);
               ptr = mystrstr( ptr, desire, par->i);
           }
           //free area
           free(buffer);
           buffer = NULL;
       }
       fclose(fp[fileIndex]);
    }

    /*========== free area =============*/
    free(fp);

    return 1;
}

int boolHandler( struct paraHandler *par, char **file, int fileNum,  struct queryHandler *qu) {
    int patternNum = -1;
    int orCount = 0;
    int falseCount = 0;
    int rbflag = OFF, mflag = OFF;
    bool truth = false;
    size_t len = 0;
    char *ptr = NULL;
    char *head = qu->pattern;
    char *line = NULL;
    struct boolDS pool[BOOL_SIZE];
    FILE **fp = (FILE**)malloc(sizeof(FILE*)*fileNum);

    /*parse pattern to find desired pattern and relationshtip between them */
    ptr = strtok( qu->pattern, "|");
    while ( ptr!=NULL) {
        head = ptr;
        wordSave( head, &(pool[orCount]));
        orCount++;
        ptr = strtok( NULL, "|");
       // if( ptr == NULL) wordSave( head, &(pool[orCount]));
    }

    /* open file */
    for( int iter = 0; iter<fileNum; iter++) {
        fp[iter] = fopen( file[iter],"r+");
        if( fp[iter] == NULL) {
            fprintf( stderr, "ERROR Unable to open %s\n", file[iter]);
            exit(EXIT_FAILURE);
        }
    }
    /* parse file */
    for( int fileIndex = 0; fileIndex<fileNum; fileIndex++) {
        while( getline( &line, &len, fp[fileIndex])!= -1){
            // for process rb
            if( par->rb!=NULL ) {
                if( rbflag == OFF && strncmp( line, par->rb, strlen(par->rb))!= 0) continue;
                else if( rbflag == OFF && strncmp( line, par->rb, strlen(par->rb)) == 0) rbflag = ON;
                else if( rbflag == ON && strncmp( line, "@\n", strlen("@\n")) == 0) rbflag = OFF;
            }

            // for process m
            if( par->m != NULL ) { //remember to parse body
                if( mflag == OFF && strncmp( line, par->m, strlen(par->m)) != 0 ) continue;
                else if( mflag == OFF && strncmp( line, par->m, strlen(par->m))==0 ) mflag = ON;
                else if( mflag == ON && strncmp( line, "@", 1) == 0) {
                    mflag = OFF;
                    continue;
                }
            }

            falseCount = 0;
            // start to parse line to check or
            for(int iter = 0; iter<orCount; iter++) {
                // check and
                for( int index = 0; index<pool[iter].index; index++) {
                    ptr = mystrstr( line, pool[iter].pattern[index].word, par->i);
                    if (ptr == NULL && pool[iter].pattern[index].neg == false) {
                        truth = false;
                        for(int i = 0; i<index+1; i++) pool[iter].pattern[i].match = 0;
                        break;
                    }
                    if( ptr!=NULL && pool[iter].pattern[index].neg == true ) {
                        truth = false;
                        for(int i = 0; i<index+1; i++) pool[iter].pattern[i].match = 0;
                        break;
                    }
                    truth = true;
                    while( ptr != NULL && pool[iter].pattern[index].neg == false) {
                        pool[iter].pattern[index].match++;
                        ptr += strlen(pool[iter].pattern[index].word);
                        ptr = mystrstr( ptr, pool[iter].pattern[index].word, par->i);
                    }
                }
                //
                if( truth == false) {
                    falseCount++;
                    continue;
                }

                else {
                    for(int i =0; i<pool[iter].index; i++) {
                        printf( "%s: %d ",pool[iter].pattern[i].word,pool[iter].pattern[i].match);
                        pool[iter].pattern[i].match = 0;
                    }
                }
            }
            //
            if( falseCount == orCount) continue;
            if( par->l == true) printf( "\n%s: %s\n", file[fileIndex], line);
            else printf( "\n%s\n", line);
        }
        // close file
        fclose( fp[fileIndex]);
    }

    /* free area*/
    for( int loop = 0; loop<orCount+1; loop++) {
        for( int iter = 0; iter<pool[loop].index; iter++)
        {
            free(pool[loop].pattern[iter].word);
        }
    }
    free(fp);
    return 1;
}

//&&->infite loop

int wordSave( char *seg, struct boolDS *po) {
    char *ptr = seg;
    char *head = ptr;
    po->index = 0;

    if( (strstr( seg, "&&"))!=NULL) {
        fprintf( stderr, "ERROR: Invalid identifier");
        exit(EXIT_FAILURE);
    }

    ptr = mystrtok( seg, "&");
    while( ptr!=NULL) {
        po->pattern[po->index].word = (char*)calloc( strlen(head), sizeof(char));
        po->pattern[po->index].neg  = false;
        if( *(head) == '!' ) {
            po->pattern[po->index].neg = true;
            head += 1;
        }
        memmove( po->pattern[po->index].word, head, strlen(head));
        po->pattern[po->index].match = 0;
        po->index++;
        head = ptr;
        ptr = mystrtok( NULL, "&");
    }
    // last one  will not be saved in loop, or condition that have no & like apple & ba | kate
    po->pattern[po->index].word = (char*)malloc( sizeof(char)*strlen(head));
    po->pattern[po->index].neg  = false;
    if( *(head) == '!' ) {
        po->pattern[po->index].neg = true;
        head += 1;
    }
    memmove( po->pattern[po->index].word, head, strlen(head));
    po->pattern[po->index].match = 0;
    po->index++;

    return 1;
}

int fieldHandler( struct paraHandler *par, char **file, int fileNum, struct queryHandler *qu) {
    char *ptr = NULL;
    char *lptr = NULL;
    char *line = NULL;
    char *inptr = NULL;
    size_t len = 0;
    struct fieldDS query;
    FILE *fp = NULL;
    int flag = OFF;
    // dummy check
    ptr = strtok( NULL," "); // because the NULL wont pass back in first call since it stores in lasttoken
    if( (ptr = strtok( NULL," "))!=NULL ){
        fprintf( stderr, "ERROR: space is not allow in query\n");
        exit(EXIT_FAILURE);
    }
    // query process
    // get desire field
    ptr = strtok( qu->pattern, "+");
    while (ptr) {
        ptr = strtok( NULL, "+");
    }
    ptr = qu->pattern;
    inptr = ptr;
    query.num = 0;
    while( *ptr!='\0' ) {
        lptr = ptr+strlen(ptr)+1;
        ptr = strtok( ptr, "=");
        inptr+= strlen(ptr);
        *inptr = ':';
        // get desire word
        query.query[query.num].word = (char*)calloc( strlen(ptr)+1, sizeof(char));
        memmove( query.query[query.num].word, ptr, strlen(ptr)+1);
        query.query[query.num].match = 0;
        ptr = lptr;
        inptr = ptr;
        query.num++;
        if( query.num > BOOL_SIZE) {
            fprintf( stderr, "ERROR: too many query");
            exit(EXIT_FAILURE);
        }
    }

    // parse line
    for( int fileIndex = 0; fileIndex<fileNum; fileIndex++) {

        fp = fopen( file[fileIndex], "r+");
        if( fp == NULL) {
            fprintf( stderr, "ERROR: unable to open %s", file[fileIndex]);
            exit(EXIT_FAILURE);
        }

        while( getline( &line, &len, fp)!= -1 ) {
            if(par->rb != NULL) {
                if( flag == OFF && strncmp( line, par->rb, strlen(par->rb))!=0 ) continue;
                flag = ON;
                if( strcmp( line, "@\n") == 0) {
                    flag = OFF;
                    continue;
                }
            }
            for( int iter = 0; iter< query.num; iter++) {
                if( mystrncmp( line, query.query[iter].word, strlen(query.query[iter].word), par) == 0 ) {
                    if( par->l == true) printf("%s: %s\n",file[fileIndex],line);
                    else printf("%s\n",line);
                    query.query[iter].match++;
                    break;
                }
            }
            free(line);
            line = NULL;
        }
        fclose(fp);
    }

    printf("Statistic Result\n");
    for( int iter = 0; iter< query.num; iter++) {
        printf( "%s\nmatch: %d\n",  query.query[iter].word, query.query[iter].match);
    }
    // free area
    for( int iter = 0; iter<query.num; iter++) {
        free(query.query[iter].word);
    }

    return 0;
}

int multiHandler(struct paraHandler *par, char **file, int fileNum, struct queryHandler *qu) {
    int rbFlag = OFF, mFlag = OFF, index = 0;
    char *pattern[MAX_PAT];
    char *ptr = strtok(qu->pattern,",");
    char *line = NULL;
    unsigned int found[HASH_SIZE] = {0};
    unsigned int hv = 0;
    int size = 0;
    size_t len = 0;
    FILE *fp = NULL;
    /* ======== preprocess pattern ======= */
    while(ptr!=NULL) {
        pattern[index] = (char*)calloc( strlen(ptr)+3, sizeof(char));
        ptr = strtok( NULL, ",");
        index++;
    }

    /* ======= parse file ==========*/
    for(int i =0; i<fileNum; i++) {
        fp = fopen(file[i],"r+");
        while(getline( &line, &len, fp) != -1) {
            // process rb
            if( par-> rb != NULL ) {
                if( rbFlag == OFF && strncmp( par->rb, line, strlen(par->rb)) == 0 ) rbFlag = ON;
                else if( rbFlag == OFF ) continue;
                else if( rbFlag == ON && strcmp( line, "@\n") == 0 ) rbFlag = OFF;
            }
            // process m
            if( par->m != NULL) {
                if(mFlag == OFF && strncmp( par->m, line, strlen(par->m)) == 0) mFlag = ON;
                else if( mFlag == OFF ) continue;
                else if( mFlag == ON ) {
                    mFlag = OFF;
                    continue;
                }
            }
            // search
            strmStr( line, pattern, found );
            free(line);
            line = NULL;
        }
        fclose(fp);
    }
    /* ======= print result ====== */
    for( int i = 0; i<index; i++) {
        hv = 5381;
        size = strlen(pattern[i]);
        for( int j = 0; j< size; j++) {
            hv = ((hv<<5)+hv)+pattern[i][j];
        }
        hv = hv % HASH_SIZE;
        printf( "%s: %d\n", pattern[i], found[hv]);
    }
    /* ======= free area ======= */
    for( int i = 0; i<index; i++) {
        free(pattern[i]);
    }
    return 1;
}

int strmStr(char *source, char **pattern, unsigned int *found) {

}