
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

char *p;
struct stat sb;
off_t len;

char* nextLine() {
    char *loc = p+len;
    for(; len < sb.st_size; len++)
        if(p[len]=='\n') {
            len++;
            break;
        }
    return loc;
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

    int res, count;
    char ch = 'Y';
    while(ch!='n' && ch!='N') {
        len = 0;
        int endReached = 0;
        while(!endReached) {
            char *line = nextLine();
            if( sscanf(line, "%d %d", &res, &count) > 0 ) {
                int need = 0;
                do {
                    printf("How many units of resource %d are needed: ", res);
                    scanf("%d", &need);
                    if(need>count)
                        printf("Only %d units are available. Please try a lower value.\n", count);
                } while(need>count);
                count -= need;
                printf("%d units of resource %d have been given to you.\n", need, res);
                char z[20];
                sprintf(z, "%d %d\n", res, count);
                for(int i=0; z[i]!='\0'; i++)
                    p[line-p+i] = z[i];
		//synchronize a file with a memory map
                if (msync (p, sb.st_size, MS_SYNC) == -1) {	
                    perror ("msync");
                }
                //printf("Resultant res: %d count: %d\n", res, count);
            } else {
                endReached = 1;
            }
        }
        printf("Press Y/y to Continue or N/n to Exit?: ");
        scanf("%1s", &ch);
    }

    if (munmap (p, sb.st_size) == -1) {	//unmap pages of memory
        perror ("munmap");
        return 1;
    }

    return 0;
}
