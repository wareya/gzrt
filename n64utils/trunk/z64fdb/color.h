/***************************
* ZFDB: ANSI Color Support *
***************************/
#ifndef __ZFDB_COLOR_H__

#ifndef NO_ANSI_COLOR

#define TEXT_NORM       "\x1B[0m"
#define TEXT_RED        "\x1B[31m"
#define TEXT_BLUE       "\x1B[34m"
#define TEXT_GREEN      "\x1B[32m"
#define TEXT_CYAN       "\x1B[36m"
#define TEXT_BOLD       "\x1B[1m"
#define TEXT_BOLD_OFF   "\x1B[22m"
#define TEXT_ITALIC     "\x1B[4m"

#else /* !NO_ANSI_COLOR */

#define TEXT_NORM       
#define TEXT_RED        
#define TEXT_BLUE       
#define TEXT_GREEN      
#define TEXT_CYAN       
#define TEXT_BOLD       
#define TEXT_BOLD_OFF   
#define TEXT_ITALIC     

#endif

#endif /* __ZFDB_COLOR_H__ */
