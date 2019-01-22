#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <string.h>


void file_display(struct stat s) {
	/*
	 (1) mode (permissions), (2) number of links, 
	 (3) owner’s name (string, not numerical), 
	 (4) group name (string, not numerical), 
	 (5) size in bytes, (6) size in blocks, 
	 (7) last modification time, and (8) name
	*/

	printf("Mode: %lo (octal)\n", (unsigned long) s.st_mode);
	printf("Number of links: ld\n", (long) s.st_nlink);

	struct passwd *owner_name; 
	struct group *group_name;
	owner_name = getpwuid(s.st_uid);
	group_name = getgrgid(s.st_gid);

	printf("Owner's name: %s \n", owner_name->pw_name);
	printf("Group name: %s \n", group_name->gr_name);
	printf("File size: %lld bytes \n", (long long) s.st_size);
	printf("File size: %lld blocks \n", (long long) s.st_blocks);
	printf("Last modification time: %s", ctime(&s.st_mtime));
}


void directory_display(char *current_dir, int level) {
	/* directory stream*/
	DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(current_dir)))
        return;
    if (!(entry = readdir(dir)))
        return;

  	while((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) { // if it's a directory
            char path[1024];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", current_dir, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            printf("%*s[%s]\n", level*2, "", entry->d_name);
            directory_display(path, level + 1);
        }
        else
            printf("%*s- %s\n", level*2, "", entry->d_name);
    } 
    closedir(dir);
}


int main(int argc, char *argv[]) {

	struct stat sb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (stat(argv[1], &sb) == -1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}

	printf("\nInput File/Directory: %s:\n", argv[1]);

	if (sb.st_mode & S_IFREG) { file_display(sb); }
	if (sb.st_mode & S_IFDIR) { directory_display(argv[1], 0); }

}

