/*********************************
* GNU Zelda ROM Tool - Messaging *
*********************************/
#ifndef __GZRT_MESSAGE_H__
#define __GZRT_MESSAGE_H__

/* Macros - debugging */
#ifdef GZRT_DEBUG
# define DEBUG(x, ...)	gzrt_kprintf(x, ##__VA_ARGS__)
#endif

/* Constants - message */
#define MSG_INFO		(1 << 24)	/* Message is displaying info	   */
#define MSG_MESSAGE		(1 << 25)	/* Message is generic			   */
#define MSG_ERROR		(1 << 26)	/* Message is an error			   */
#define MSG_WARNING     (1 << 27)   /* Message is a warning            */

/* Message flags */
#define MSG_F_FATAL		(1 << 1)	/* Fatal error - quit on user OK   */
#define MSG_F_CMD		(1 << 2)	/* Print message to terminal	   */
#define MSG_F_ECHO		(1 << 3)	/* Echo message to terminal        */
#define MSG_F_STDOUT	(1 << 4)	/* Write message to stdout (see ^) */
#define MSG_F_STDERR	(1 << 5)	/* Write message to stderr (see ^) */

/* Message exit code */
#define MSG_EXIT(x)		(((x) & 0xFF) << 16)


/* Functions -- msg.c */
extern void gzrt_kappend ( char * mesg );
extern int gzrt_kprintf ( char * fmt, ... );
extern int gzrt_message ( int flags, char * title, char * fmt, ... );

#endif /* __GZRT_MESSAGE_H__ */
