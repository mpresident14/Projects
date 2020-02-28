/* The functions dealing with the tun/tap interface were provided in 
 * simpletun.c, whose details are shown below. It was downloaded from
 * http://www.cis.syr.edu/~wedu/seed/Labs_12.04/Networking/VPN/files/simpletun.c
 * /

/**************************************************************************
 * simpletun.c                                                            *
 *                                                                        *
 * A simplistic, simple-minded, naive tunnelling program using tun/tap    *
 * interfaces and TCP. Handles (badly) IPv4 for tun, ARP and IPv4 for     *
 * tap. DO NOT USE THIS PROGRAM FOR SERIOUS PURPOSES.                     *
 *                                                                        *
 * You have been warned.                                                  *
 *                                                                        *
 * (C) 2009 Davide Brini.                                                 *
 *                                                                        *
 * DISCLAIMER AND WARNING: this is all work in progress. The code is      *
 * ugly, the algorithms are naive, error checking and input validation    *
 * are very basic, and of course there can be bugs. If that's not enough, *
 * the program has not been thoroughly tested, so it might even fail at   *
 * the few simple things it should be supposed to do right.               *
 * Needless to say, I take no responsibility whatsoever for what the      *
 * program might do. The program has been written mostly for learning     *
 * purposes, and can be used in the hope that is useful, but everything   *
 * is to be taken "as is" and without any kind of warranty, implicit or   *
 * explicit. See the file LICENSE for further details.                    *
 *************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h> 
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>
#include <memory.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 

#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

/* buffer for reading from tun/tap interface, must be >= 1500 */
#define BUFSIZE 2000   
#define CLIENT 0
#define SERVER 1
#define PORT 55555

/* some common lengths */
#define IP_HDR_LEN 20
#define ETH_HDR_LEN 14
#define ARP_PKT_LEN 28

/* SSL stuff */
#define HOME "./"

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

#define KEYSIZE 16
#define IVKEYSIZE 2 * KEYSIZE
#define IVSIG 1
#define KEYSIG 2
#define BREAKSIG 3

int debug;
char *progname;

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. The caller     *
 *            needs to reserve enough space in *dev.                      *
 **************************************************************************/
int tun_alloc(char *dev, int flags) {

  struct ifreq ifr;
  int fd, err;

  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
    perror("Opening /dev/net/tun");
    return fd;
  }

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
    perror("ioctl(TUNSETIFF)");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);

  return fd;
}

/**************************************************************************
 * cread: read routine that checks for errors and exits if an error is    *
 *        returned.                                                       *
 **************************************************************************/
int cread(int fd, char *buf, int n){
  
  int nread;

  if((nread=read(fd, buf, n))<0){
    perror("Reading data");
    exit(1);
  }
  return nread;
}

/**************************************************************************
 * cwrite: write routine that checks for errors and exits if an error is  *
 *         returned.                                                      *
 **************************************************************************/
int cwrite(int fd, char *buf, int n){
  
  int nwrite;

  if((nwrite=write(fd, buf, n))<0){
    perror("Writing data");
    exit(1);
  }
  return nwrite;
}

/**************************************************************************
 * read_n: ensures we read exactly n bytes, and puts those into "buf".    *
 *         (unless EOF, of course)                                        *
 **************************************************************************/
int read_n(int fd, char *buf, int n) {

  int nread, left = n;

  while(left > 0) {
    if ((nread = cread(fd, buf, left))==0){
      return 0 ;      
    }else {
      left -= nread;
      buf += nread;
    }
  }
  return n;  
}

/**************************************************************************
 * do_debug: prints debugging stuff (doh!)                                *
 **************************************************************************/
void do_debug(char *msg, ...){
  
  va_list argp;
  
  if(debug){
	va_start(argp, msg);
	vfprintf(stderr, msg, argp);
	va_end(argp);
  }
}

/**************************************************************************
 * my_err: prints custom error messages on stderr.                        *
 **************************************************************************/
void my_err(char *msg, ...) {

  va_list argp;
  
  va_start(argp, msg);
  vfprintf(stderr, msg, argp);
  va_end(argp);
}

/**************************************************************************
 * usage: prints usage and exits.                                         *
 **************************************************************************/
void usage(void) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s -i <ifacename> [-s|-c <serverIP>] [-p <port>] [-u|-a] [-d]\n", progname);
  fprintf(stderr, "%s -h\n", progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "-i <ifacename>: Name of interface to use (mandatory)\n");
  fprintf(stderr, "-s|-c <serverIP>: run in server mode (-s), or specify server address (-c <serverIP>) (mandatory)\n");
  fprintf(stderr, "-p <port>: port to listen on (if run in server mode) or to connect to (in client mode), default 55555\n");
  fprintf(stderr, "-u|-a: use TUN (-u, default) or TAP (-a)\n");
  fprintf(stderr, "-d: outputs debug information while running\n");
  fprintf(stderr, "-h: prints this help text\n");
  exit(1);
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;
  int ciphertext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) {
  	ERR_print_errors_fp(stderr);
  	exit(1);
  }

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 128 bit AES (i.e. a 128 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
  	ERR_print_errors_fp(stderr);
  	exit(1);
  }

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1) {
  	ERR_print_errors_fp(stderr);
  	exit(1);
  }
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
  	ERR_print_errors_fp(stderr);
  	exit(1);
  } 
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;
  int plaintext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())){
  	ERR_print_errors_fp(stderr);
  	exit(1);
  }

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher. For AES this is 128 bits
   */
  if(EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
  	ERR_print_errors_fp(stderr);
  	exit(1);
  }

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1) {
  	ERR_print_errors_fp(stderr);
  	exit(1);
  }
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
  	ERR_print_errors_fp(stderr);
  	exit(1);
  }
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

/****************************************************************************
 * exchangeIvAndKey: Sets up secure TCP connection and exchanges IV and key *                                 					  *
 ****************************************************************************/
void exchangeIvAndKey(
	int cliserv, char* server_ip, int sd, int sigkey, int* ivkey_pipe)
{
	printf("Starting Exchange.\n");
	int err;
	SSL*     ssl;
	X509*    peer_cert;
	char*    str;
	char     buf [4096];

	// Define filenames
	char certf[11];
	char keyf[11];

	if (cliserv == CLIENT) {
		strncpy(certf, "client.crt", 11);
		strncpy(keyf, "client.key", 11);
	}
	else {
		strncpy(certf, "server.crt", 11);
		strncpy(keyf, "server.key", 11);
	}

	char cacert[] = "ca.crt";

	SSL_CTX* ctx;
	const SSL_METHOD *meth;

	SSLeay_add_ssl_algorithms();
	meth = cliserv == CLIENT ? SSLv23_client_method() : SSLv23_server_method();
	SSL_load_error_strings();
	ctx = SSL_CTX_new (meth);

	CHK_NULL(ctx);
	CHK_SSL(err);

	SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL); // Require server to have a cert
	SSL_CTX_load_verify_locations(ctx,cacert,NULL);

    
	if (SSL_CTX_use_certificate_file(ctx, certf, SSL_FILETYPE_PEM) <= 0) {
	  ERR_print_errors_fp(stderr);
	  exit(3);
	}
	if (SSL_CTX_use_PrivateKey_file(ctx, keyf, SSL_FILETYPE_PEM) <= 0) {
	  ERR_print_errors_fp(stderr);
	  exit(4);
	}

	if (!SSL_CTX_check_private_key(ctx)) {
	  fprintf(stderr,"Private key does not match the certificate public key\n");
	  exit(5);
	}

	/* ----------------------------------------------- */
	/* Now we have TCP connection. Start SSL negotiation. */

	ssl = SSL_new(ctx);                         
	CHK_NULL(ssl);    
	SSL_set_fd(ssl, sd);
	err = cliserv == CLIENT ? SSL_connect(ssl) : SSL_accept(ssl);                    
	CHK_SSL(err);

	/* Get the cipher - opt */

	printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

	/* Get certificate (note: beware of dynamic allocation) - opt */

	peer_cert = SSL_get_peer_certificate (ssl);       
	CHK_NULL(peer_cert);
	printf ("Peer certificate:\n");

	str = X509_NAME_oneline (X509_get_subject_name (peer_cert),0,0);
	CHK_NULL(str);
	printf ("\t subject: %s\n", str);
	OPENSSL_free (str);

	str = X509_NAME_oneline (X509_get_issuer_name  (peer_cert),0,0);
	CHK_NULL(str);
	printf ("\t issuer: %s\n", str);
	OPENSSL_free (str);

	X509_free (peer_cert);

	/* --------------------------------------------------- */
	/* DATA EXCHANGE - Send a message and receive a reply. */

	/* A 128 bit IV and 128 bit key */
	unsigned char iv_key[IVKEYSIZE];
	/* All zero array to compare when receiving break signal */
  unsigned char zero_arr[IVKEYSIZE] = {0};

	// Client generates iv_key and sends to server
	int sig = 0;
	while(1) {
		if (cliserv == CLIENT) {
			if (sig == 0) {
		  	if (RAND_bytes(iv_key, IVKEYSIZE) == 0) {
		  		ERR_print_errors_fp(stderr); 
		  		exit(1);
		  	}
		  }
		  else if (sig == IVSIG) {
		  	if (RAND_bytes(iv_key, KEYSIZE) == 0) {
		  		ERR_print_errors_fp(stderr); 
		  		exit(1);
		  	}
		  }
		  else if (sig == KEYSIG) {
		  	if (RAND_bytes(iv_key+KEYSIZE, KEYSIZE) == 0) {
		  		ERR_print_errors_fp(stderr); 
		  		exit(1);
		  	}
		  }
		  else if (sig == BREAKSIG) {
		  	memset(iv_key, 0, IVKEYSIZE);
		  }
		  else {
		  	printf("Unknown sig for exchangeIvAndKey\n");
		  	continue;
		  }
		  err = SSL_write (ssl, iv_key, IVKEYSIZE);  
			CHK_SSL(err);
			printf("Exchange complete\n");
			err = write(ivkey_pipe[1], iv_key, IVKEYSIZE);
			if (sig == BREAKSIG) {
				break;
			}
			sig = getMsg(sigkey); // Blocks here until client runs with -k opt
		}
		// Server reads iv_key
		else {
			err = SSL_read (ssl, iv_key, IVKEYSIZE); // Blocks here                  
			CHK_SSL(err);
			printf("Exchange complete\n");
			write(ivkey_pipe[1], iv_key, IVKEYSIZE);
			if (memcmp(iv_key, zero_arr, IVKEYSIZE) == 0) {
				break;
			}
		}
	}	

	/* Clean up. */
	if (cliserv == SERVER) {
		SSL_shutdown (ssl);  /* send SSL/TLS close_notify */
	}
	SSL_free (ssl);
	SSL_CTX_free (ctx);
}

/* msg functions adapted from GeeksForGeeks */
  
// structure for message queue 
struct mesg_buffer { 
    long mesg_type; 
    int sig; 
} message; 
  
void sendMsg(key_t sigkey, char* remote_ip, int sig) 
{ 
    int msgid; 
  
    // msgget creates a message queue 
    // and returns identifier 
    msgid = msgget(sigkey, 0666 | IPC_CREAT); 
    message.mesg_type = 1; 
    message.sig = sig;
  
    // msgsnd to send message 
    msgsnd(msgid, &message, sizeof(message), 0); 
} 

int getMsg(key_t sigkey)
{
    int msgid; 
  
    // msgget creates a message queue 
    // and returns identifier 
    msgid = msgget(sigkey, 0666 | IPC_CREAT); 
    
   
    // msgrcv to receive message 
    msgrcv(msgid, &message, sizeof(message), 1, 0); // Blocks   
    
    // to destroy the message queue 
    msgctl(msgid, IPC_RMID, NULL); 
  
    return message.sig; 
} 

int main(int argc, char *argv[]) {
  
  int tap_fd, option;
  int flags = IFF_TUN;
  char if_name[IFNAMSIZ] = "";
  int header_len = IP_HDR_LEN;
  int maxfd;
  uint16_t nread, nwrite, pmsglength, pmaclength;
  char buffer[BUFSIZE];
  struct sockaddr_in local, remote;
  char remote_ip[16] = "";
  unsigned short int port = PORT;
  int sock_fd, net_fd, optval = 1;
  socklen_t remotelen;
  int cliserv = -1;    /* must be specified on cmd line */
  unsigned long int tap2net = 0, net2tap = 0;

  unsigned char mac[EVP_MAX_MD_SIZE], verify_mac[EVP_MAX_MD_SIZE];
	unsigned int mac_len, verify_mac_len;

	unsigned char ciphertext[EVP_MAX_MD_SIZE];
	unsigned int ciphertext_len;

  // For establishing the TCP connection
	int new_sd;
	int new_listen_sd;
	int err;
	struct sockaddr_in sa;
	struct sockaddr_in sa_serv;
	struct sockaddr_in sa_cli;
	size_t client_len;

	// ftok to generate unique key 
  key_t sigkey = ftok("simpletun", 65); 

  progname = argv[0];
  
  /* Check command line options */
  while((option = getopt(argc, argv, "i:sc:p:uahdxkb")) > 0){
    switch(option) {
      case 'd':
        debug = 1;
        break;
      case 'h':
        usage();
        break;
      case 'i':
        strncpy(if_name,optarg,IFNAMSIZ-1);
        break;
      case 's':
        cliserv = SERVER;
        break;
      case 'c':
        cliserv = CLIENT;
        strncpy(remote_ip,optarg,15);
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'u':
        flags = IFF_TUN;
        break;
      case 'a':
        flags = IFF_TAP;
        header_len = ETH_HDR_LEN;
        break;
      case 'x':
      	if (cliserv != CLIENT) {
      		fprintf(stderr, "-c option must come before -x.\n");
      		exit(1);
      	}
      	sendMsg(sigkey, remote_ip, IVSIG);
      	return 0;
      case 'k':
      	if (cliserv != CLIENT) {
      		fprintf(stderr, "-c option must come before -k.\n");
      		exit(1);
      	}
      	sendMsg(sigkey, remote_ip, KEYSIG);
      	return 0;
      case 'b':
      	if (cliserv != CLIENT) {
      		fprintf(stderr, "-c option must come before -b.\n");
      		exit(1);
      	}
      	sendMsg(sigkey, remote_ip, BREAKSIG);
      	return 0;
      default:
        my_err("Unknown option %c\n", option);
        usage();
    }
  }

  argv += optind;
  argc -= optind;

  if(argc > 0){
    my_err("Too many options!\n");
    usage();
  }

  if(*if_name == '\0'){
    my_err("Must specify interface name!\n");
    usage();
  }else if(cliserv < 0){
    my_err("Must specify client or server mode!\n");
    usage();
  }else if((cliserv == CLIENT)&&(*remote_ip == '\0')){
    my_err("Must specify server address!\n");
    usage();
  }

  /* initialize tun/tap interface */
  if ( (tap_fd = tun_alloc(if_name, flags | IFF_NO_PI)) < 0 ) {
    my_err("Error connecting to tun/tap interface %s!\n", if_name);
    exit(1);
  }

  do_debug("Successfully connected to interface %s\n", if_name);

  //---------------------------------------------------------------------
  /* Now set up secure TCP connection in new process and exchange iv and key */
	int ivkey_pipe[2];
	pid_t pid;

	if (pipe(ivkey_pipe) == -1) {
		fprintf(stderr, "Pipe Failed" ); 
	  return 1; 
	}

	pid = fork(); // Returns 0 in child, child pid in parent

	// FAILED
	if (pid < 0) {
		fprintf(stderr, "Fork Failed" ); 
	  exit(1); 
	}
	// CHILD (TCP)
	else if (pid == 0) {		
		if (cliserv == CLIENT) {
			new_sd = socket (AF_INET, SOCK_STREAM, 0);       
			CHK_ERR(new_sd, "socket");

		  memset (&sa, '\0', sizeof(sa));
		  sa.sin_family      = AF_INET;
		  sa.sin_addr.s_addr = inet_addr (remote_ip);   /* Server IP */
		  sa.sin_port        = htons     (55554);       /* Server Port number */
			  
		  err = connect(new_sd, (struct sockaddr*) &sa, sizeof(sa));                   
		  CHK_ERR(err, "connect");
		  printf ("Connected to %lx, port %x\n", (long unsigned) sa.sin_addr.s_addr, sa.sin_port);			
		}	
	
		else {
				new_listen_sd = socket (AF_INET, SOCK_STREAM, 0);   
				CHK_ERR(new_listen_sd, "socket");
		  
			  memset (&sa_serv, '\0', sizeof(sa_serv));
			  sa_serv.sin_family      = AF_INET;
			  sa_serv.sin_addr.s_addr = INADDR_ANY;
			  sa_serv.sin_port        = htons (55554);          /* Server Port number */
			  
			  if(setsockopt(new_listen_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0){
		      perror("setsockopt()");
		      exit(1);
		    }

			  err = bind(new_listen_sd, (struct sockaddr*) &sa_serv, sizeof (sa_serv));                   
			  CHK_ERR(err, "bind");
				     
			  /* Receive a TCP connection. */
			  err = listen (new_listen_sd, 5);                    
			  CHK_ERR(err, "listen");
			  
			  client_len = sizeof(sa_cli);
			  // Blocks here for iv and key updates
			  new_sd = accept (new_listen_sd, (struct sockaddr*) &sa_cli, &client_len);
			  CHK_ERR(new_sd, "accept");
			  close (new_listen_sd);

			  printf ("Connection from %lx, port %x\n", (long unsigned) sa_cli.sin_addr.s_addr, sa_cli.sin_port);			
		}

		close(ivkey_pipe[0]); 	// Close reading
		exchangeIvAndKey(cliserv, remote_ip, new_sd, sigkey, ivkey_pipe);
		close(ivkey_pipe[1]);   // Close writing
		close(new_sd);	

    exit(0);
	}	

	// PARENT (UDP)

	/* Separate iv and key */
	unsigned char iv_key[IVKEYSIZE];
	unsigned char iv[KEYSIZE];
	unsigned char key[KEYSIZE];

	close(ivkey_pipe[1]); // Close writing
	read(ivkey_pipe[0], iv_key, IVKEYSIZE);

	memcpy(iv, iv_key, KEYSIZE);
	memcpy(key, iv_key + KEYSIZE, KEYSIZE);

	// key[0] = 0xfa; // USE THIS TO DEMONSTRATE HMAC VERIFICATION / DECRYPTION
	// iv[0] = 0xfa; // USE THIS TO DEMONSTRATE DECRYPTION	

	int i;
	printf("iv:");
	for (i = 0; i < KEYSIZE; ++i) {
		printf(" %02x", iv[i]);
	}
	printf("\n");

	printf("key:");
	for (i = 0; i < KEYSIZE; ++i) {
		printf(" %02x", key[i]);
	}
	printf("\n");

//---------------------------------------------------------------------

  if ( (sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket()");
    exit(1);
  }

  if(cliserv==CLIENT){
    /* Client, try to connect to server */

    /* assign the destination address */
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr(remote_ip);
    remote.sin_port = htons(port);

    /* connection request */
    if (connect(sock_fd, (struct sockaddr*) &remote, sizeof(remote)) < 0){
      perror("connect()");
      exit(1);
    }

    if (sendto(sock_fd, buffer, BUFSIZE, 0, (struct sockaddr*) &remote, sizeof(remote)) < 0) {
    	perror("sendto()");
    	exit(1);
    }

    net_fd = sock_fd;
    do_debug("CLIENT: Connected to server %s\n", inet_ntoa(remote.sin_addr));
    
  } 
  else {
    /* Server, wait for connections */

    /* avoid EADDRINUSE error on bind() */
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0){
      perror("setsockopt()");
      exit(1);
    }
    
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(port);
    if (bind(sock_fd, (struct sockaddr*) &local, sizeof(local)) < 0){
      perror("bind()");
      exit(1);
    }
    
    /* wait for connection request */
    remotelen = sizeof(remote);
    memset(&remote, 0, remotelen);
    if (recvfrom(sock_fd, buffer, BUFSIZE, 0, (struct sockaddr*) &remote, &remotelen) < 0) {
    	perror("recvfrom()");
    	exit(1);
		}	

		if (connect(sock_fd, (struct sockaddr*) &remote, sizeof(remote)) < 0){
	     perror("connect()");
	     exit(1);
    }

		net_fd = sock_fd;

    do_debug("SERVER: Client connected from %s\n", inet_ntoa(remote.sin_addr));
  }

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);
  
  /* use select() to handle two descriptors at once */
  maxfd = (tap_fd > net_fd) ? tap_fd : net_fd;
  maxfd = (maxfd > ivkey_pipe[0]) ? maxfd : ivkey_pipe[0];

  int x_bytes;



  while(1) {
    int ret;
    fd_set rd_set;
    FD_ZERO(&rd_set);
    FD_SET(tap_fd, &rd_set); 
    FD_SET(net_fd, &rd_set);
    FD_SET(ivkey_pipe[0], &rd_set);

    // Blocks until tap_fd or net_fd or pipe has something to read
    ret = select(maxfd + 1, &rd_set, NULL, NULL, NULL);

    if (ret < 0 && errno == EINTR){
      continue;
    }
    if (ret < 0) {
      perror("select()");
      exit(1);
    }

    if (FD_ISSET(ivkey_pipe[0], &rd_set)) {
    	x_bytes = read(ivkey_pipe[0], iv_key, IVKEYSIZE);

			memcpy(iv, iv_key, KEYSIZE);
			memcpy(key, iv_key + KEYSIZE, KEYSIZE);

			// break tunnel if iv and key are both all zeroes
			int shouldBreak = 1;
			int i;
			printf("iv:");
			for (i = 0; i < KEYSIZE; ++i) {
				printf(" %02x", iv[i]);
				if (iv[i] != 0) {
					shouldBreak = 0;
				}
			}
			printf("\n");

			printf("key:");
			for (i = 0; i < KEYSIZE; ++i) {
				printf(" %02x", key[i]);
				if (key[i] != 0) {
					shouldBreak = 0;
				}
			}
			printf("\n");	

			if (shouldBreak) {
				break;
			}
		}	

    if(FD_ISSET(tap_fd, &rd_set)){
      /* data from tun/tap: just read it and write it to the network */
      
      nread = cread(tap_fd, buffer, BUFSIZE);

      tap2net++;
      do_debug("TAP2NET %lu: Read %d bytes from the tap interface\n", tap2net, nread);

      /* generate hmac based on plaintext in buffer and key and put into mac */
      HMAC(EVP_sha256(), key, KEYSIZE, buffer, nread, mac, &mac_len);

      /* encrypt message */
      ciphertext_len = encrypt(buffer, nread, key, iv, ciphertext);

      /* write msg length */
      pmsglength = htons(ciphertext_len);
      cwrite(net_fd, (char *)&pmsglength, sizeof(pmsglength));
      /* write msg length */
      pmaclength = htons(mac_len);
      cwrite(net_fd, (char *)&pmaclength, sizeof(pmaclength));
      /* write message */
      nwrite = cwrite(net_fd, ciphertext, ciphertext_len);
      /* write hmac */
      cwrite(net_fd, mac, mac_len);
      
      do_debug("TAP2NET %lu: Written %d bytes to the network\n", tap2net, nwrite);
    }

    if(FD_ISSET(net_fd, &rd_set)){
      /* data from the network: read it, and write it to the tun/tap interface. 
       * We need to read the length first, and then the packet */

      /* Read msg length */      
      nread = read_n(net_fd, (char *)&pmsglength, sizeof(pmsglength));
      if(nread == 0) {
        /* ctrl-c at the other end */
        break;
      }
      /* Read mac length */      
      read_n(net_fd, (char *)&pmaclength, sizeof(pmaclength));

      net2tap++;

      /* read msg */
      ciphertext_len = read_n(net_fd, ciphertext, ntohs(pmsglength));
      /* read mac */
      mac_len = read_n(net_fd, mac, ntohs(pmaclength));

      /* decrypt message */
      nread = decrypt(ciphertext, ciphertext_len, key, iv, buffer);

      /* Verify sender by computing hmac and comparing with received hmac */
      HMAC(EVP_sha256(), key, KEYSIZE, buffer, nread, verify_mac, &verify_mac_len);
      if (mac_len == verify_mac_len && memcmp(mac, verify_mac, mac_len) == 0) {
      	printf("Sender identity verified\n");
      }
      else {
      	printf("Unknown sender\n");
      	continue;
      }

      do_debug("NET2TAP %lu: Read %d bytes from the network\n", net2tap, ciphertext_len);

      /* now buffer[] contains a full packet or frame, write it into the tun/tap interface */ 
      nwrite = cwrite(tap_fd, buffer, nread);
      do_debug("NET2TAP %lu: Written %d bytes to the tap interface\n", net2tap, nwrite);
    }
  }
  close(ivkey_pipe[0]); // Close reading
  
  return 0;
}