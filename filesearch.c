#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <fcntl.h>

void search_files(const char *dir,const char *file_pattern, const char * regex) {
  // Compile the regular expression
  regex_t re;
        
  regcomp(&re, regex, REG_EXTENDED);
  // Open the directory
  
  int dir_fd = open(dir, O_RDONLY);
  if (dir_fd == -1) {
      perror("open");
  }

  DIR *dp = fdopendir(dir_fd);
  
  if (dp == NULL) {
    printf("Could not open directory '%s'\n", dir);
    return;
  }

  // Iterate over the files in the directory
  struct dirent *de;
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
          perror("malloc");
          // Handle allocation error
      }
      sprintf(next_entry_path, "%s/%s", dir, file_name);

      stat(next_entry_path, &st);
      if (S_ISDIR(st.st_mode)) {
          
        // The entry is a directory
        
        search_files(next_entry_path, file_pattern, regex);
      } else if (fnmatch(file_pattern, file_name, 0) == 0) {
            
        FILE *fp = fopen(next_entry_path, "r");

        if (fp == NULL) {
          printf("Could not open file '%s : %s'\n", next_entry_path, file_name);
          continue;
        }
        
        // Read the file content

        char buf[1024];
        char *bufCont = (char *)malloc(0 + 2 + 1);
        
        while (fgets(buf, sizeof(buf), fp) != NULL) {
          strcat(bufCont, buf); 
        }

        // Check if the file content matches the regular expression
        int matched = regexec(&re, bufCont, 0, NULL, 0);
        
        if (matched == 0) {
          // The file content matches the regular expression, print it
          printf("%s: %s\n", next_entry_path, buf);
          continue;
        }

        fclose(fp);
      }

      free(next_entry_path);
    }
    
  }

  // Close the directory
  closedir(dp);
  regfree(&re);
  
}

int main(int argc, char *argv[]) {
  // Check the arguments
  if (argc != 3) {
    printf("Usage: search <pattern> <regex>\n");
    return 1;
  }

  // Get the file pattern and regex
  char * file_pattern = argv[1];
  char * regex = argv[2];
  
  // Start the recursive search
  search_files(".", file_pattern, regex);
  return 0;
}
