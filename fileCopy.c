//Seokho Han
//700657609
//CS4500
//File copy system using system call

/*
 =====
 file copy program assignment
 - Check source file exists or not
 - Check readable or not
 - Open source file and tell whether it is failed or not
 - Append or overwrite function
 =====
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#define BUFFSIZE 512
#define PERM 0644

int askUser();

int main (int argc, char* argv[])
{
    if (argc != 3) {
        printf("Try again =>> Usage: fileCopy sourcefile destinationfile\n");
        return -1;
    }
    
    //source file
    char* pathSource = argv[1];
    //destination file
    char* path = argv[2];
    int rval;
    
    char buffer[BUFFSIZE];
    int infile, outfile;
    int nread;
    //stat
    struct stat sb;
    
    /* Check file existence. */
    rval = access (pathSource, F_OK);
    if (rval == 0)
        printf ("%s exists\n", pathSource);
    else {
        if (errno == ENOENT)
            printf ("Error: %s\n", strerror(errno));
        else if (errno == EACCES)
            printf ("%s is not accessible\n", pathSource);
        return 0;
    }
    
    /* Check read access. */
    rval = access (pathSource, R_OK);
    if (rval == 0)
        printf ("%s is readable\n", pathSource);
    else
        printf ("Error: %s\n", strerror(errno));
    
    //Open file start
    //IF failed to open it
    infile = open(argv[1], O_RDONLY);
    if(infile < 0){
        printf("File open Error: %s\n", strerror(errno));
        return -2;
    }
    //Check destination file exists
    rval = access (path, F_OK);
    if(rval == 0)
    {
        printf("File Exists\n");
        int overWrite = askUser();
        //Append
        if(overWrite == 2)
            outfile = open(argv[2],  O_RDWR | O_APPEND);
        //Overwrite
        else if(overWrite == 1)
            outfile = open(argv[2], O_RDWR | O_TRUNC);
        //no no
        else
            return 0;
    }
    //If destination file does not exist
    else
    {
        //get the source file status using stat system call
        if (stat(argv[1], &sb) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }
        //print file status
        printf("\nFile type:               ");
        switch (sb.st_mode & S_IFMT) {
            case S_IFBLK:
                printf("block device\n");
                break;
            case S_IFCHR:
                printf("character device\n");
                break;
            case S_IFDIR:
                printf("directory\n");
                break;
            case S_IFIFO:
                printf("FIFO/pipe\n");
                break;
            case S_IFLNK:
                printf("symlink\n");
                break;
            case S_IFREG:
                printf("regular file\n");
                break;
            case S_IFSOCK:
                printf("socket\n");
                break;
            default:
                printf("unknown?\n");
                break;
        }
        printf("I-node number:            %ld\n", (long) sb.st_ino);
        printf("Mode:                     %lo (octal)\n",(unsigned long) sb.st_mode);
        printf("Link count:               %ld\n", (long) sb.st_nlink);
        printf("Ownership:                UID=%ld   GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);
        printf("Preferred I/O block size: %ld bytes\n",(long) sb.st_blksize);
        printf("File size:                %lld bytes\n",(long long) sb.st_size);
        printf("Blocks allocated:         %lld\n",(long long) sb.st_blocks);
        printf("Last status change:       %s", ctime(&sb.st_ctime));
        printf("Last file access:         %s", ctime(&sb.st_atime));
        printf("Last file modification:   %s\n", ctime(&sb.st_mtime));
        //end getting file status
        
        //create new file
        if ((outfile = creat(argv[2], PERM)) < 0) {
            close(infile);
            return -3;
        }
    }
	while((nread = read(infile, buffer, BUFFSIZE)) > 0)
            if(write(outfile, buffer, nread) < nread){
                close(infile);
                close(outfile);
                return -4;
            };

        close(infile);
        close(outfile);
  
    /* Check write access. */
    rval = access (pathSource, W_OK);
    if (rval == 0)
        printf ("%s is writable\n", pathSource);
    else if (errno == EACCES)
        printf ("%s is not writable (access denied)\n", pathSource);
    else if (errno == EROFS)
        printf ("%s is not writable (read-only filesystem)\n", pathSource);
    return 0;
}

//ask user whether overwrite and append or not
int askUser()
{
    int answer;
    printf("Overwrite? (1 = yes / 0 = no)");
    scanf("%d",&answer);
    if(answer)
    {
        printf("Overwrite\n");
        return 1;
    }
    else if(!answer)
    {
        printf("Append? (1 / 0)");
        scanf("%d", &answer);
        if(!answer)
        {
            printf("No, don't append\n");
            return 0;
        }else
        {
            printf("Append\n");
            return 2;
        }
    }
    return 0;
}
