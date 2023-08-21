#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <dirent.h>
#include <fnmatch.h>

void search_files(const char *dir,const char *file_pattern, const char * regex) {
  // Compile the regular expression
  
  // Open the directory
  DIR *dp = opendir(dir);
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
    if (fnmatch(file_pattern, file_name, 0) == 0) {
      regex_t re;
      regcomp(&re, regex, REG_EXTENDED);
      
      int matched = regexec(&re, file_name, 0, NULL, 0);

      if (matched == 0) {
      // The file matches both the file pattern and the regular expression, print it
        printf("%s / %s\n", dir, file_name);
      }

      regfree(&re);
    }
    if (de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
      // The entry is a directory
      char *next_dir = (char *)malloc(strlen(dir) + 2 + 1);
      strcpy(next_dir, dir);
      strcat(next_dir, "/");
      strcat(next_dir, de->d_name);

      search_files(next_dir, file_pattern, regex);

      
      free(next_dir);
    }
  }

  // Close the directory
  closedir(dp);

  
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

  // Cleanup
  

  return 0;
}

