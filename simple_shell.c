/*

  A simple shell program without too many features.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


void build_arg_array(char* clean_input,char* exec_args[50])
{
  int arg_count = 0;
  
  char* tok;
  tok = strtok (clean_input," ");
  while (tok != NULL)
    {
      exec_args[arg_count] = tok;
      arg_count++;
      tok = strtok (NULL, " ");
    }
  return;
}


void build_path_array(char* all_path, char* exec_paths[50],int* path_count)
{
  //we make path count start at 1 because 0 holds current working directory
  exec_paths[0]=(char*)"./";
  
  char* path_tok;
  path_tok = strtok (all_path,"$");
  while (path_tok != NULL)
    {
      exec_paths[(*path_count)] = path_tok;
      (*path_count)++;
      path_tok = strtok (NULL, "$");
    }
  return;
}


int search_path(char* exec_args[50], char* exec_paths[50], int path_count)
{
  int all_paths_index;
  for(all_paths_index = 0; all_paths_index < path_count; all_paths_index++)
    {
      char checking_path[1024];
      strcpy(checking_path,exec_paths[all_paths_index]);
      strcat(checking_path,exec_args[0]);
      execv(checking_path,exec_args);
    }
  // the command wasn't found. Do this.
  printf("Command not found.\n");
  return 0;
}


int main(int argc, char* argv[])
{
  char* history_list[1024];
  int history_counter=0;

  while (1==1)
    {
      printf("> ");

      // remove the newline at the end of the input
      char* raw_input=malloc(1024);
      char* clean_input=malloc(1024);
      fgets(raw_input,1024,stdin);
      strncpy(clean_input, raw_input, strlen(raw_input)-1);
      history_list[history_counter] = clean_input;
      history_counter++;

      if (strcmp(clean_input,(char*) "quit")==0)
	{
	  break;
	}
      else if (strcmp(clean_input,(char*) "history")==0)
	{
	  int i;
	  for(i=0;i<history_counter;i++)
	    {
	      printf("%s\n",history_list[i]);
	    }
	}
      else
	{
	  pid_t p;
	  p=fork();
	  if (p==0)
	    {
	      // build argument list and split up paths
	      char* exec_args[50];
	      build_arg_array(clean_input,exec_args);

	      char* exec_paths[50];
	      int path_count=1;
	      build_path_array(getenv("MYPATH"),exec_paths,&path_count);
	      
	      /// search all the directories in order.
	      search_path(exec_args, exec_paths, path_count);
	      return 0;
	    }
	  else if (p > 0)
	    {
	      wait(NULL);
	    }
	  else
	    {
	      perror("Could not fork.");
	    }
	}
    }
  return 0;
}
