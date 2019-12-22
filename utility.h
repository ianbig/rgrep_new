//
// Created by Ian Liu on 2019/10/30.
//

#ifndef PROJECT2_UTILITY_H
#define PROJECT2_UTILITY_H
#define _GNU_SOURCE
#define ON 1
#define OFF 0
#define BOOL_SIZE 100
#define MAX_PAT 100
#define STRING_MAX 1000
#define HASH_SIZE 256
#define DONE 3
#define STORAGE 500
#define W_SIZE 1000
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <dirent.h>
#include <time.h>

struct paraHandler {
    bool i; //incenstive
    char *rb; //record begin pattern
    bool l; //list mathced file
    bool n;
    char *m; //matching field
    int recordMax;
};

struct queryHandler {
    int queryNum; // 0 = simple; 1 = filed; 2 = boolean; 3 = multi-term
    size_t offsetFile;
    char *pattern;
};

struct wordRecord {
    char *word;
    int  match;
    bool neg;
};

struct boolDS {
    struct wordRecord pattern[BOOL_SIZE]; //save and DS
    int index;
};

struct fieldDS {
    struct wordRecord query[BOOL_SIZE];
    int num;
};

struct Hash {
    struct Hash *next;
    char str[STRING_MAX];
};

struct Record {
    char rec[10];
    char url[STORAGE];
    char mainTex[STORAGE];
    char untag[STORAGE];
    char siteCode[STORAGE];
    char urlCode[STORAGE]; //imp
    char title[W_SIZE]; // title
    char size[STORAGE];
    char keyword[W_SIZE];
    char image[STORAGE];
    char fetch[STORAGE];
    char post[STORAGE];
    char ref[STORAGE];
    char bdm5[STORAGE];
    char lang[STORAGE];
    char ip[STORAGE];
    char *body; // imp
    char botVert[STORAGE];
    char  time[STORAGE];
};

void check(char *fp, int *recorded);
void splitFile( FILE* fp);
size_t readFile( FILE* fp, size_t expectRead, char *fileName);
void help();
int output(struct Record *record, int recordCount, int recordMax, int fileCount, char *filename, struct paraHandler *par);
void paraInit(struct paraHandler *par);
size_t optionProcess( int argc, char **argv, struct paraHandler *par); // return query places
int queryProcess( int argc, char **argv, size_t offset, struct queryHandler *qu);
char** fileProcess( int argc, char **argv, size_t start);
int stringHandler( struct paraHandler *par, char **file, int fileNUm, char *desire);
int boolHandler( struct paraHandler *par, char **file, int fileNum,  struct queryHandler *qu);
char* mystrstr( char *buffer, char *desire, bool mode);
int wordSave( char *seg, struct boolDS *po);
char *mystrtok( char *tar, char *seg);
int fieldHandler( struct paraHandler *par, char **file, int fileNum, struct queryHandler *qu);
int mystrncmp( char *s1, char *s2, size_t len, struct paraHandler *par);
int multiHandler(struct paraHandler *par, char **file, int fileNum, struct queryHandler *qu);
int strmStr( char *source, char **pattern, unsigned int *found, int patternSize);
int storeRecord(char *buffer, struct Record *record, int bodyFlag);
void emptyTmp(struct Record *record);
bool delFile(const char *dir);
#endif //PROJECT2_UTILITY_H
