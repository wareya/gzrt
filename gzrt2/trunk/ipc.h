/************************************
* GZRT - Interprocess communication *
************************************/
#ifndef __GZRT_IPC_H__
#define __GZRT_IPC_H__

/* ----------------------------------------------
   Constants
   ---------------------------------------------- */

#define GZRT_IPC_MAGIC	0x05A5A540

/* ----------------------------------------------
   Structures
   ---------------------------------------------- */

/* Actions */
enum IPCAction
{
	/* Print a message */
	GZRT_IPC_MESSAGE = 1,
	
	/* Print a debug message */
	GZRT_IPC_KPRINTF,
	
	/* Application flags */
	GZRT_IPC_AF_GET,			/* Get all app flags */
	GZRT_IPC_AF_SET,			/* OR flags          */
	GZRT_IPC_AF_CLEAR,			/* AND flags         */
	
	/* Quit application */
	GZRT_IPC_EXIT,
};

/* An identification packet */
struct gzrtIPCident
{
	int   magic;
	pid_t pid;
};

/* Client */
struct gzrtClient
{
	pid_t pid;
	int   fd;
};

/* An IPC query */
struct gzrtIPCQuery
{
	int   code;	/* Code           */
	int   len;	/* Length of data */
	pid_t pid;  /* Sender         */
};

/* An IPC print message request */
struct gzrtIPCMesg
{
	int flags;
	int title_len;
	int message_len;
	char data[];
};

/* An IPC kprintf request */
struct gzrtIPCKmesg
{
	int message_len;
	char data[];
};

/* Set an application flag */
struct gzrtICPFlagSet
{
	int flag;
};

/* Clear an application flag */
struct gzrtICPFlagClear
{
	int flag;
};


/* ----------------------------------------------
   Functions              
   ---------------------------------------------- */

extern void gzrt_ipc_init ( void );
extern void gzrt_ipc_kill_clients ( void );


#endif /* __GZRT_IPC_H__ */
