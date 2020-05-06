#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include <pthread.h>

#define LINEBUF_SIZE 1024

int chat_with_client(struct Calc *calc, int infd, int outfd);
void error(const char *c);
void *worker(void *arg);

struct ConnInfo {
  struct Calc *calc;
  int clientfd;
};

int keep_going;

int main(int argc, char **argv) {
   if (argc != 2) { error("Usage: ./arithserver <port>"); }
   int server_fd = open_listenfd(argv[1]);
   if (server_fd < 0) { error("Couldn’t open server socket\n"); }
   keep_going = 1;

   struct Calc *calc = calc_create();
   
   while (keep_going) {
     int client_fd = Accept(server_fd, NULL, NULL);
     if (client_fd < 0) {
       error("Error opening client connection.");
     }
     struct ConnInfo *info = malloc(sizeof(struct ConnInfo));
     info->calc = calc;
     info->clientfd = client_fd;
      
     pthread_t thr_id;
     if (pthread_create(&thr_id, NULL, worker, info) != 0) {
       error("pthread_create failed");
     }
   }

   calc_destroy(calc);
   close(server_fd); // close server socket
   return 0;
}


void *worker(void *arg) {
  struct ConnInfo *info = arg;
  pthread_detach(pthread_self());
  keep_going =
    chat_with_client(info->calc, info->clientfd, info->clientfd);
  
  close(info->clientfd);
  free(info);
  return NULL;
}

int chat_with_client(struct Calc *calc, int infd, int outfd) {
        rio_t in;
        char linebuf[LINEBUF_SIZE];

        /* wrap standard input (which is file descriptor 0) */
        rio_readinitb(&in, infd);

        /*                                                                      
         * Read lines of input, evaluate them as calculator expressions,        
         * and (if evaluation was successful) print the result of each          
         * expression.  Quit when "quit" command is received.                   
         */
        int done = 0;
        while (!done) {
	  ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
                if (n <= 0) {
		  /* error or end of input */
		  done = 1;
                } else if (strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0) {
		  /* quit command */
		  done = 1;

		} else if (strcmp(linebuf, "shutdown\n") == 0 ||
			   strcmp(linebuf, "shutdown\r\n") == 0) {
		  
		  return 0;
		  
                } else {
		  /* process input line */
		  
		  int result;
		  if (calc_eval(calc, linebuf, &result) == 0) {
		    /* expression couldn't be evaluated */
		    rio_writen(outfd, "Error\n", 6);
		  } else {
		    
		    /* output result */
		    int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n\
", result);
		    if (len < LINEBUF_SIZE) {
		      rio_writen(outfd, linebuf, len);
		    }
		  }
                }
        }
	return 1;
}

void error(const char *c) {
  printf("%s\n", c);
  exit(1);
}
