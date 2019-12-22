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
    par->n = false;
    par->m = NULL;
    par->rb = NULL;
    par->recordMax = 100;
}

size_t optionProcess( int argc, char **argv, struct paraHandler *par) {

    int iter = 0;
    int size = -1;
    int rbFlag = OFF;
    int mFlag = OFF;
    int gFlag = OFF;

    for( iter = 1; iter<argc; iter++) {
        size = strlen(argv[iter]);
        if(size>3) {
            if(strcmp(argv[iter-1],"-rb")==0)
                continue;
            else if( strcmp( argv[iter-1],"-m")==0)
                continue;
            else if(strcmp( argv[iter-1],"-g") == 0 )
                continue;
            else break;
        }
    } // find the boundary w/ size since parameter number won't be too long

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

        else if( strcmp(argv[index],"-n") == 0 ) {
            par->n = true;
        }

        else if( strcmp(argv[index], "-g") ==0 ) {
            int amount = 0;
            size_t digit = strlen(argv[index]);
            amount = atoi(argv[index+1]);
            par->recordMax = amount;
            gFlag = ON;

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

            else if(gFlag == ON) {
                gFlag = OFF;
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
    int bodyFlag = OFF;
    int fileIndex = -1;
    char *buffer = NULL;
    size_t len = 0;
    char *ptr = NULL;
    int rbFlag = OFF;
    int mFlag = OFF;
    int a = -2;
    int recordCheck = 0, outFlag = OFF;
    unsigned int matched = 0;
    int fileCount = 1, recordCount = 0, preCount, rcount = 0;
    struct Record *record = (struct Record*)malloc(sizeof(struct Record) * 1);

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
           // store into temperary
           if(recordCheck == ON) {
               bodyFlag = ON;
           }
           recordCheck = storeRecord(buffer, record, bodyFlag);
           if(recordCheck == DONE && outFlag == OFF)
           {
               bodyFlag = OFF;
               emptyTmp(record);
               recordCheck = 0;
           }
           if(recordCheck == DONE && outFlag == ON ) {
               preCount = fileCount;
               if(par->n == false) fileCount = output(record, recordCount, par->recordMax, fileCount, file[fileIndex], par); // add fileCount in function
               if(preCount != fileCount) {
                   recordCount = 0;
               }
               recordCount++;
               rcount++;
               outFlag = OFF;
               bodyFlag = OFF;
               recordCheck = 0;
               continue;
           }
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
               matched++;
               outFlag = ON;
               ptr += strlen(desire);
               ptr = mystrstr( ptr, desire, par->i);
           }
           //free area
       }
        free(buffer);
        buffer = NULL;

       fclose(fp[fileIndex]);
    }
    if(par->n == true) {
        printf("%s: %d\n", desire, matched);
        printf("matched reocrd: %d\n", rcount);
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

    if(par->n == true) {
        fprintf( stderr,"ERROR: boolean search not support parameter n\n");
        exit(EXIT_FAILURE);
    }

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
                    if( par->l == true && par->n == false) printf("%s: %s\n",file[fileIndex],line);
                    else if(par->n == false) printf("%s\n",line);
                    query.query[iter].match++;
                    break;
                }
            }
            free(line);
            line = NULL;
        }
        fclose(fp);
    }

    if(par->n == true) {
        printf("Statistic Result\n");
        for( int iter = 0; iter< query.num; iter++) {
            printf( "%s\nmatch: %d\n",  query.query[iter].word, query.query[iter].match);
        }
    }

    // free area
    for( int iter = 0; iter<query.num; iter++) {
        free(query.query[iter].word);
    }

    return 0;
}

int multiHandler(struct paraHandler *par, char **file, int fileNum, struct queryHandler *qu) {
    int rbFlag = OFF, mFlag = OFF, patternSize = 0;
    char *pattern[MAX_PAT];
    char *ptr = strtok(qu->pattern,",");
    char *line = NULL;
    unsigned int found[HASH_SIZE] = {0};
    unsigned int hv = 0;
    int size = 0;
    int foundNum = 0;
    size_t len = 0;
    FILE *fp = NULL;
    int recordCheck = 0, outFlag = OFF;
    int fileCount = 1, recordCount = 0, preCount, rcount = 0;
    struct Record *record = (struct Record*)malloc(sizeof(struct Record) * 1);
    int bodyFlag = OFF;

    /* ======== preprocess pattern ======= */
    while(ptr!=NULL) {
        pattern[patternSize] = (char*)calloc( strlen(ptr)+10, sizeof(char));
        memmove( pattern[patternSize], ptr, strlen(ptr));
        ptr = strtok( NULL, ",");
        patternSize++;
    }

    /* ======= parse file ==========*/
    for(int i =0; i<fileNum; i++) {
        fp = fopen(file[i],"r+");
        if(fp == NULL) {
            fprintf( stderr, "ERROR: Unable to open file\n");
            exit(EXIT_FAILURE);
        }
        while(getline( &line, &len, fp) != -1) {
            // store into temperary
            if(recordCheck == ON) {
                bodyFlag = ON;
            }
            recordCheck = storeRecord(line, record, bodyFlag);
            if(recordCheck == DONE && outFlag == OFF)
            {
                bodyFlag = OFF;
                emptyTmp(record);
                recordCheck = 0;
            }
            if(recordCheck == DONE && outFlag == ON ) {
                preCount = fileCount;
                if(par->n == false) fileCount = output(record, recordCount, par->recordMax, fileCount, file[i], par); // add fileCount in function
                if(preCount != fileCount) {
                    recordCount = 0;
                }
                recordCount++;
                rcount++;
                outFlag = OFF;
                bodyFlag = OFF;
                recordCheck = 0;
                continue;
            }
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
                else if( mFlag == ON  && strncmp( line, "@", 1) == 0) {
                    mFlag = OFF;
                    continue;
                }
            }
            // search
            if(par->i == true) {
                for(int k = 0; k< strlen(line); k++) {
                    line[k] = tolower(line[k]);
                }
            }
            foundNum = strmStr( line, pattern, found, patternSize);
            if(foundNum > 0 ) outFlag = ON;
            free(line);
            line = NULL;
        }


        fclose(fp);
    }
    fprintf(stderr,"finish searching\n");
    /* ======= print result ====== */
    if( par->n == true) {
        for( int i = 0; i<patternSize; i++) {
            hv = 5381;
            size = strlen(pattern[i]);
            for (int j = 0; j < size; j++) {
                hv = ((hv << 5) + hv) + pattern[i][j];
            }
            hv = hv % HASH_SIZE;
            printf("%s: %d\n", pattern[i], found[hv]);
        }
        printf("matched reocrd: %d\n", rcount);
    }

    /* ======= free area ======= */
    for( int i = 0; i<patternSize; i++) {
        free(pattern[i]);
    }
    free(record);
    return 1;
}

int strmStr(char *source, char **pattern, unsigned int *found, int patternSize) {
    int i = 0, j = 0, k = 0;
    int m = 2<<15,length = -1, b = 3;
    int currentFound = 0;
    int shift = 0;
    int shiftTable[HASH_SIZE];
    int len = 0;
    unsigned int hv = 0;
    struct Hash hashTable[HASH_SIZE];
    struct Hash *ptr, *nptr;

    char *tmp = NULL, *end = NULL, *cptr = NULL;


    /* ====== init =======*/
    for(i = 0; i <HASH_SIZE;i++) {
        hashTable[i].next = NULL;
    }

    /*====== preprcess ========*/
    // find minimum length m
    for( i = 0; i<patternSize; i++) {
        length = strlen(pattern[i]);
        if(length<m) {
            m = length;
        }
    }
    // init shift table & hash table
    for( i = 0; i<HASH_SIZE; i++) {
        shiftTable[i] = m-b+1;
    }
    // calculate pattern's shift accord to length b
    for( i = 0; i<patternSize; i++) {
        for( j = 0; j<m-b+1; j++) {
            hv = 5381;
            // calculate hashvalue
            for( k = 0; k<b; k++) {
                hv = ((hv<<5)+hv)+pattern[i][j+k];
            }
            hv = hv % HASH_SIZE;
            shift = (m-1)-(j+b-1);
            if(shift<shiftTable[hv]) shiftTable[hv] = shift;
            if(shift == 0) {
                ptr = &(hashTable[hv]);
                nptr = (struct Hash*)malloc(sizeof(struct Hash) * 1);
                memmove( nptr->str, pattern[i], strlen(pattern[i]));
                nptr->next = NULL;
                while(ptr->next!=NULL) ptr = ptr->next;
                ptr->next = nptr;
            }
        }
    }

    /*======= search ==========*/
    tmp = source;
    end = tmp+strlen(source);
    tmp += m-1;
    while( tmp < end ) {
        //calculate hash value
        hv = 5381;
        for( k = 0; k<3; k++) {
            hv = ((hv<<5)+hv)+*(tmp-b+1+k);
        }
        hv = hv%HASH_SIZE;
        shift = shiftTable[hv];
        if(shift==0) {
            ptr = &(hashTable[hv]);
            ptr = ptr->next; // first node not store information
            cptr = tmp-(m-1);
            while( ptr != NULL) {
                if( strncmp(ptr->str, cptr, (len = strlen(ptr->str))) == 0) {
                    hv = 5381;
                    for( k = 0; k < len; k++) {
                        hv = ((hv<<5)+hv)+ptr->str[k];
                    }
                    hv = hv % HASH_SIZE;
                    found[hv] = found[hv]+1;
                    currentFound++;
                    break;
                }
                ptr = ptr->next;
            }
            shift = 1;
        }
        tmp += shift;

    }

    /*======= free area ========*/
    for( i = 0; i<HASH_SIZE; i++) {
        ptr = &(hashTable[i]);
        if(ptr->next == NULL) continue;
        else ptr = ptr->next;
        while (ptr != NULL)
        {
            nptr = ptr->next;
            free(ptr);
            ptr = nptr;
        }

    }

    return currentFound;

}


int output(struct Record *record, int recordCount, int recordMax, int fileCount, char *filename, struct paraHandler *par) {

    char fileName[50] = {0};
    FILE *op = NULL;
    if(recordCount >= recordMax) {
        fileCount++;
    }
    sprintf(fileName, "./tmp/tmp_%d.txt", fileCount);
    op = fopen(fileName, "a+");
    if(par->l == true) fwrite(fileName, sizeof(char), strlen(fileName), op);
    // section for output
    fwrite(record->rec, sizeof(char), strlen(record->rec), op);
    fwrite(record->url, sizeof(char), strlen(record->url), op);
    fwrite(record->mainTex, sizeof(char), strlen(record->mainTex), op);
    fwrite(record->untag, sizeof(char), strlen(record->untag), op);
    fwrite(record->siteCode, sizeof(char), strlen(record->siteCode), op);
    fwrite(record->urlCode, sizeof(char), strlen(record->urlCode), op);
    fwrite(record->title, sizeof(char), strlen(record->title), op);
    fwrite(record->size, sizeof(char), strlen(record->size), op);
    fwrite(record->keyword, sizeof(char), strlen(record->keyword), op);
    fwrite(record->image, sizeof(char), strlen(record->image), op);
    fwrite(record->fetch, sizeof(char), strlen(record->fetch), op);
    fwrite(record->post, sizeof(char), strlen(record->post), op);
    fwrite(record->ref, sizeof(char), strlen(record->ref), op);
    fwrite(record->bdm5, sizeof(char), strlen(record->bdm5), op);
    fwrite(record->lang, sizeof(char), strlen(record->lang), op);
    fwrite(record->ip, sizeof(char), strlen(record->ip), op);
    fwrite(record->botVert, sizeof(char), strlen(record->botVert), op);
    fwrite(record->time, sizeof(char), strlen(record->time), op);
    fwrite(record->body, sizeof(char), strlen(record->body), op);
    emptyTmp(record);
    fclose(op);
    return fileCount;
}

void check(char *fp, int *recorded) {
    FILE *file = fopen(fp, "r+");
    char parameters[50][1000] = {0};
    size_t len = 0;
    char *line = NULL, *check;
    int i = 0;
    *recorded = 0;


    if(file == NULL) {
        fprintf(stderr ,"Unable to open %s\n", fp);
        exit(EXIT_FAILURE);
    }

    while( getline( &line, &len, file) != -1) {
        if(line[0] != '@') continue;
        check = mystrtok(line, ":");
        if( check == NULL) continue;
        for( i = 0; i <= *recorded; i++) {
            if(strcmp(line, parameters[i]) == 0) {
                break;
            }
        }
        if( i == *recorded + 1) {
            *recorded += 1;
            memmove(parameters[*recorded], line, strlen(line));
        }
        free((line));
        line = NULL;
    }
    for(i = 0; i <= *recorded; i++) {
        fprintf(stderr, "%s\n", parameters[i]);
    }

}

int storeRecord(char *buffer, struct Record *record, int bodyFlag) {
    // remember to return recordCheck value
    // record check value meaning 0 = continue; 3 = ready to output; 1 = enter body
    if( strcmp(buffer, "@\n") == 0) return DONE;
    if( !strncmp(buffer, "@Gais_REC", strlen("@Gais_REC")) ) {
        memmove(record->rec, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@url", strlen("@url")) ){
        memmove(record->url, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@MainTextMD5", strlen("@MainTextMD5")) ){
        memmove(record->mainTex, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@UntagMD5", strlen("@UntagMD5")) ) {
        memmove(record->untag, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@SiteCode", strlen("@SiteCode")) ){
        memmove(record->siteCode, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@UrlCode", strlen("@UrlCode")) ){
        memmove(record->urlCode, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@title", strlen("@title")) ){
        memmove(record->title, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@Size", strlen("@Size")) ){
        memmove(record->size, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@keyword", strlen("@keyword")) ){
        memmove(record->keyword, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@image_links", strlen("@image_links")) ){
        memmove(record->image, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@post_time", strlen("@post_time")) ){
        memmove(record->post, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@Ref", strlen("@Ref")) ){
        memmove(record->ref, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@BodyMD5", strlen("@BodyMD5")) ){
        memmove(record->bdm5, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@Lang", strlen("@Lang")) ){
        memmove(record->lang, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@IP", strlen("@IP")) ){
        memmove(record->ip, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@body", strlen("@body")) ){
        record->body = (char*)malloc(sizeof(char) * ( strlen(buffer) + 3 ));
        memmove(record->body, buffer, strlen(buffer));
        return ON;
    }
    // deal w/ rest of body
    else if(bodyFlag == ON) {
        char *ptr = record->body;
        record->body = (char*)calloc( (strlen(ptr) + strlen(buffer) + 10), sizeof(char));
        memmove(record->body, ptr, strlen(ptr));
        strcat(record->body, buffer);
        free(ptr);
        return ON;
    }

    else if(!strncmp(buffer, "@Time", strlen("@Time")) ){
        memmove(record->time, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@botVer", strlen("@botVer")) ){
        memmove(record->botVert, buffer, strlen(buffer));
    }
    else if(!strncmp(buffer, "@Fetchtime", strlen("@Fetchtime")) ){
        memmove(record->fetch, buffer, strlen(buffer));
    }

    return 0;
}

void emptyTmp(struct Record *record) {

    memset(record->rec,'\0',strlen(record->rec));
    memset(record->url,'\0',strlen(record->url));
    memset(record->mainTex,'\0',strlen(record->mainTex));
    memset(record->untag,'\0',strlen(record->untag));
    memset(record->siteCode,'\0',strlen(record->siteCode));
    memset(record->urlCode,'\0',strlen(record->urlCode));
    memset(record->title,'\0',strlen(record->title));
    memset(record->size,'\0',strlen(record->size));
    memset(record->keyword,'\0',strlen(record->keyword));
    memset(record->image,'\0',strlen(record->image));
    memset(record->fetch,'\0',strlen(record->fetch));
    memset(record->post,'\0',strlen(record->post));
    memset(record->ref,'\0',strlen(record->ref));
    memset(record->bdm5,'\0',strlen(record->bdm5));
    memset(record->lang,'\0',strlen(record->lang));
    memset(record->ip,'\0',strlen(record->ip));
    memset(record->botVert,'\0',strlen(record-> botVert));
    memset(record->time,'\0',strlen(record->time));
    free(record->body);
    record->body = NULL;
}

bool delFile(const char *dir) {
    DIR *drip = opendir(dir);
    struct dirent *entry;
    char dirName[100]  ={0};
    int filecount = 0;

    while( (entry = readdir(drip) ) !=  NULL) {
        if(entry->d_type == DT_REG) {
            sprintf(dirName,"./tmp/%s",entry->d_name);
            filecount = remove(dirName);
        }
    }
    closedir(drip);
    return true;
}
