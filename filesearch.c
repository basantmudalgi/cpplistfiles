#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

void search_files(const char *dir,const char *file_pattern, const char * regex) {
  // Compile the regular expression
  regex_t re;
        
  regcomp(&re, regex, REG_EXTENDED);
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
    struct stat st;

    char *next_entry_path = (char *)malloc(strlen(dir) + 2 + 1);
    strcpy(next_entry_path, dir);
    strcat(next_entry_path, "/");
    strcat(next_entry_path, file_name);

    // Get the status of the file
    stat(next_entry_path, &st);
  
    if (S_ISDIR(st.st_mode)) {
      if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {     
          // The entry is a directory
          
          search_files(next_entry_path, file_pattern, regex);
      }
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
