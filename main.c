#include "utility.h"

int main( int argc, char **argv ) {
    struct paraHandler par;
    struct queryHandler qu;
    char **file = NULL;
    size_t offset = -1;
    size_t fileNum  = -1;
    int *recorded = (int*)malloc(sizeof(int));

    /*
    FILE *fp = fopen("ettoday.rec","r+");
    splitFile(fp);
    fclose(fp);
    return 0;
     */




    /*=========== help message ============*/
    if(argc<4) {
        help();
    }

    /*============ init parameter ===============*/
    paraInit(&par);

    /*=============== process option file/query/file===========*/
    offset  = optionProcess( argc, argv, &par);
    queryProcess( argc, argv, offset, &qu);
    file = fileProcess( argc, argv, qu.offsetFile);
    fileNum = argc-qu.offsetFile;



    if(par.rb!= NULL && strcmp( par.rb, "@")== 0  ) {
        fprintf( stderr, "ERROR: @ is header implication, cannot use as begin field");
        exit(EXIT_FAILURE);
    }

    /*============== act accord to queryNUm============*/
    if(qu.queryNum == 0) stringHandler( &par, file, fileNum, qu.pattern);
    else if(qu.queryNum == 1) {
        if( par.m != NULL) {
            fprintf( stderr, "ERROR: Not logical to use -m and query field at the same time\n");
            exit(EXIT_FAILURE);
        }
        fieldHandler( &par, file, fileNum, &qu);
    }
    else if(qu.queryNum == 2) boolHandler( &par, file, fileNum, &qu);
    else if(qu.queryNum == 3) multiHandler( &par, file, fileNum, &qu);

    /*=============== free area ===================*/
    for( int iter = 0; iter< fileNum; iter++) {
        free(file[iter]);
        file[iter] = NULL;
    }

    free(par.rb);
    free(par.m);
    free(qu.pattern);
    free(file);

    return 0;
}