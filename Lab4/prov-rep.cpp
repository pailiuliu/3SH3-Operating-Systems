
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

static int *parentDone;

char *p;
struct stat sb;
off_t len;

// To read next line from res.txt file
char* nextLine() {
    char *loc = p+len;
    for(; len < sb.st_size; len++)
        if(p[len]=='\n') {
            len++;
            break;
        }
    return loc;
}

// To display mincore() - page resident in memory related details
void displayMincore(char *addr, size_t length, int pageSize) {
    unsigned char *vec;
    int numPages, j;

    numPages = (length + pageSize - 1) / pageSize;
    vec = (unsigned char *) malloc(numPages);
    if (vec == NULL)
        perror("malloc");

    if (mincore(addr, length, vec) == -1)
        perror("mincore");

    printf("No of pages: %d\n", numPages);
    printf("Status of pages (* indicates resident in memory . indicates not):\n");

    for (j = 0; j < numPages; j++) {
        if (j % 64 == 0)
            printf("%s%10p: ", (j == 0) ? "" : "\n", addr + (j * pageSize));
        printf("%c", (vec[j] & 1) ? '*' : '.');
    }
    printf("\n");

    free(vec);
}

// Function called by child process
void childWork() {
    while(! *parentDone)  {
        printf("\n\n## Begin: stats ##\n");
        int pageSize;
        #ifndef _SC_PAGESIZE
            pageSize = getpagesize();   /* Prints page size */
        #else
            pageSize = sysconf(_SC_PAGESIZE);
        #endif
        printf("Page size: %d\n", pageSize);
        for(len=0; len < sb.st_size; len++)
            putchar(p[len]);
        printf("\n");
        
        displayMincore(p, sb.st_size, pageSize); /* prints page residents*/
        printf("## End: stats ##\n\n");
        int TenSec = 10;
        while(TenSec-- > 0) {
            if(*parentDone) break;
            sleep(1);
        }
    }
}

// Function called by parent process after forking child
void parentWork() { /* adds resources */
    int res, count;
    char ch = 'Y';
    while(ch!='n' && ch!='N') {
        len = 0;
        int endReached = 0;
        while(!endReached) {
            char *line = nextLine();
            if( sscanf(line, "%d %d", &res, &count) > 0 ) {
                int add = 0;
                do {
                    printf("NUmber of units of resource %d to be added: ", res);
                    scanf("%d", &add);
                    if(count+add>9)
                        printf("Try a lower value.\n", count);
                } while(count+add>9);
                count += add;
                printf("%d units of resource %d have been added by you.\n", add, res);
                char z[20];
                sprintf(z, "%d %d\n", res, count);
                for(int i=0; z[i]!='\0'; i++)
                    p[line-p+i] = z[i];
                if (msync (p, sb.st_size, MS_SYNC) == -1) {
                    perror ("msync");
                }
               
            } else {
                endReached = 1;
            }
        }
        printf("Press Y/y to Continue or N/n to Exit: ");
        scanf("%1s", &ch);
    }
}

int main ()
{
    int fd = open ("res.txt", O_RDWR);
    if (fd == -1) {
        perror ("could not open res.txt");
        return 1;
    }

    if (fstat (fd, &sb) == -1) {
        perror ("fstat");
        return 1;
    }

    if (!S_ISREG (sb.st_mode)) {
        fprintf (stderr, "res.txt is not a file\n");
        return 1;
    }

    p = (char*) mmap (0, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror ("mmap");
        return 1;
    }

    if (close (fd) == -1) {
        perror ("close");
        return 1;
    }

    // Initialise shared variable to zero before fork
    parentDone = (int*) mmap(0, sizeof *parentDone, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *parentDone = 0;

    // Create a child process
    pid_t pid = fork();

    if (pid == 0) {
        // child process
        childWork();
    }
    else if (pid > 0) {
        // parent process
        parentWork();
        printf("Press n/N to stop displaying stats at any time.\n");
        char ch = 'Y';
        while(ch!='n' && ch!='N') scanf("%1s", &ch);
        // This will trigger the child to exit
        *parentDone = 1;
        // Wait for child to exit
        wait(NULL);
        munmap(parentDone, sizeof *parentDone);
    }
    else {
        // fork failed
        printf("fork() failed!\n");
        return 1;
    }

    if (munmap (p, sb.st_size) == -1) {
        perror ("munmap");
        return 1;
    }

    return 0;
}
