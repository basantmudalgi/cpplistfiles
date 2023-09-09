#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <fcntl.h>

void search_files(const char *dir,const char *file_pattern, regex_t re) {
  // Open the directory
  
    //printf("         opening the dir %s \n", dir);
  DIR *dp = opendir(dir);
  
  if (dp == NULL) {
    //printf("Could not open directory '%s'\n", dir);
    //return;
  } else {
    struct dirent *de;
    // Iterate over the files in the directory
    while ((de = readdir(dp)) != NULL) {
      // Get the file name
      char *file_name = de->d_name;
      // Check if the file matches the regular expression
      // Check if the file matches the file pattern
      
      if (strcmp(file_name, ".") != 0 && strcmp(file_name, "..") != 0) {
        // Get the status of the file
        struct stat st;

        size_t path_len = strlen(dir) + strlen(file_name) + 2;
        char *next_entry_path = (char *)malloc(path_len);
        if (next_entry_path == NULL) {
            // Handle allocation error
            perror("malloc");
            return;
        }
        sprintf(next_entry_path, "%s/%s", dir, file_name);
        
        stat(next_entry_path, &st);
        if (S_ISDIR(st.st_mode)) {    
          // The entry is a directory
          search_files(next_entry_path, file_pattern, re);
        } else if (fnmatch(file_pattern, file_name, 0) == 0) {
          FILE *fp = fopen(next_entry_path, "r");

          if (fp == NULL) {
            printf("Could not open file '%s : %s'\n", next_entry_path, file_name);
          } else {

            char buf[1024];
            while (fgets(buf, sizeof(buf), fp) != NULL) {
              // Check if the regex matches the file content
              regmatch_t match;
              int matched = regexec(&re, buf, 0, &match, 0);
              if (matched == 0 ) {
                // The regex matches the file content, print it
                printf("%s: %s\n", next_entry_path, buf);
              }
            }
            fclose(fp);
          }
        }
        free(next_entry_path);
      }
      
    }

    // Close the directory
    closedir(dp);
    
  
  }

  

}

int main(int argc, char *argv[]) {
  // Check the arguments
  if (argc != 3) {
    //printf("Usage: search <pattern> <regex>\n");
    return 1;
  }

  // Get the file pattern and regex
  char * file_pattern = argv[1];
  char * regex = argv[2];
  // Compile the regular expression
  regex_t re;
        
  regcomp(&re, regex, REG_EXTENDED);
  
  // Start the recursive search
  search_files(".", file_pattern, re);

  //printf("freeing the regex\n");
  regfree(&re);
  
  //printf("freeing the reg \n");  
  return 0;
}
