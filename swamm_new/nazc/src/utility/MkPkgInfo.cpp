
/** Nuritelecom AiMiR Project.
 *
 * Nuritelecom Package Info file generator
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "typedef.h"
#include "mcudef.h"
#include "CommonUtil.h"

void usage(char * fname)
{
    fprintf(stderr,"%s [-th] model hwver fwver fwbuild file\n", fname);
    fprintf(stderr,"  -t otatype : sensor, nzc, codi (default sensor)\n"); 
    fprintf(stderr,"  -h : display this message\n"); 
    fprintf(stderr,"\n"); 
    fprintf(stderr,"example:\n"); 
    fprintf(stderr," %s -t sensor AIDON 1.2 2.1 23 aidon_v21_b23.ebl\n", fname); 
    exit(1);
}

int GetMd5Sum(const char * file, char * sum)
{
    int     maxLine = 128;
    char    szLine[maxLine];
    char    szCmd[512];
    FILE    *fp = NULL;
    int     res = 1;

    sprintf(szCmd,"md5sum %s", file);

    if((fp = popen(szCmd, "r")) != NULL) {
        while(fgets(szLine, maxLine, fp) != NULL)
        {
            if(strlen(szLine)>32)
            {
                strncpy(sum, szLine, 32);
                res = 0;
                break;
            }
        }
        pclose(fp);
    }
    return res;
}

int mkPkgInfo(const char * model, BYTE otatype, WORD hw, WORD fw, WORD build, const char * file)
{
    struct  stat file_info;
    PACKAGEINFO pkg;
    char destfile[256];
    char * extptr;
    FILE * fp;
    int len;

    if(!model || !file)
    {
        fprintf(stderr,"invalid parameter\n");
        return 1;
    }

	if(stat(file, &file_info))
    {
        fprintf(stderr,"file access error : %s\n", file);
        return 2;
    }

    if(!S_ISREG(file_info.st_mode)) 
    {
        fprintf(stderr,"invalid file mode : %s\n", file);
        return 3;
    }

    memset(destfile, 0, sizeof(destfile));
    extptr = rindex(file, '.');

    sprintf(destfile,"%s%s", model, extptr ? extptr : "");

    if(CopyFile(file, destfile))
    {
        fprintf(stderr,"create model file fail : %s\n", model);
        return 4;
    }

    memset(&pkg, 0, sizeof(PACKAGEINFO));
    len = sizeof(pkg.szModel) <= strlen(model) ? sizeof(pkg.szModel) - 1 : strlen(model);
    if(GetMd5Sum(destfile, pkg.szChecksum))
    {
        fprintf(stderr, "get checksum error : %s\n", destfile);
        return 5;
    }

    pkg.nType = otatype;
    strncpy(pkg.szModel, model, len);
    pkg.nHwVersion = hw;
    pkg.nSwVersion = fw;
    pkg.nBuildVersion = build;

    sprintf(destfile, "%s.pkg", model);

    if((fp=fopen(destfile,"w"))==NULL)
    {
        fprintf(stderr, "create pkg file error : %s\n", destfile);
        return 6;
    }

    fwrite(&pkg, sizeof(PACKAGEINFO), 1, fp);

    fclose(fp);
    return 0;
}

int main(int argc, char * argv[])
{
    int  opt;
    int h, l;
    BYTE nType = OTA_TYPE_SENSOR;
    int hw, fw, build;

    while((opt=getopt(argc, argv, "ht:")) != -1) {
        switch(opt) {
            case 't': 
                if(!strncasecmp(optarg,"con",3))
                    nType = OTA_TYPE_CONCENTRATOR;
                else if(!strncasecmp(optarg,"coo",3))
                    nType = OTA_TYPE_COORDINATOR;
                break;
            default :
                usage(argv[0]);
        }
    }
    if(argc - optind < 5) usage(argv[0]);
    if(sscanf(argv[optind+1],"%d.%d", &h, &l) != 2) 
    {
        fprintf(stderr,"invalid format : %s\n", argv[optind+1]);
        usage(argv[0]);
    }
    hw = h; hw = (hw << 8) | l;
    if(sscanf(argv[optind+2],"%d.%d", &h, &l) != 2) 
    {
        fprintf(stderr,"invalid format : %s\n", argv[optind+2]);
        usage(argv[0]);
    }
    fw = h; fw = (fw << 8) | l;
    if(sscanf(argv[optind+3],"%d", &build) != 1) 
    {
        fprintf(stderr,"invalid format : %s\n", argv[optind+3]);
        usage(argv[0]);
    }

    if(mkPkgInfo(argv[optind], nType, (WORD)hw, (WORD)fw, (WORD)build, argv[optind+4]))
    {
        fprintf(stderr,"create package fail\n");
    }

    return 0;
}
