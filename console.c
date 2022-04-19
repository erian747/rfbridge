#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "blf.h"
#include "microrl.h"
#include "console.h"
#include "trace.h"
#include "cbuf.h"
#include "mcal.h"
#define OPTPARSE_IMPLEMENTATION
#include "optparse.h"
#include "usb_serial.h"

#define input_ringbuf_SIZE    32

static volatile struct
{
  uint8_t m_getIdx;
  uint8_t m_putIdx;
  char    m_entry[input_ringbuf_SIZE];
} input_ringbuf;



enum
{
  SIGNAL_INSERT_CHAR = BLF_FIRST_USER_SIG,
};


static BLFSemaphore *input_sem = 0;
static void *console_thread = NULL;


static void console_output(const char *s)
{
  usb_serial_write((const uint8_t *)s, strlen(s));
}


static char sprintbuf[512];

int console_printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int ret = vsnprintf(sprintbuf, sizeof(sprintbuf), fmt, ap);
  console_output(sprintbuf);
  va_end(ap);

  return ret;
}


static microrl_t rl;
#define prl (&rl)
static int console_locked = 0;


void console_insert(char c)
{
  if(CBUF_IsFull(input_ringbuf) == 0) {
    CBUF_Push(input_ringbuf, c);
    BLF_setSemaphore(input_sem);
  }
}


static int do_help(int argc, char** argv);

static const struct console_cmd  cmd_first  __attribute__((used, section(".console_firstcmd"))) = {
  .name = "help",
  .handler = do_help,
  .help = "Show this message",
  .required_args = -1,
  .maximum_args = -1,
} ;

static const struct console_cmd  cmd_last  __attribute__((used, section(".console_lastcmd"))) = {
};


#define FOR_EACH_CMD(iterator) for (iterator=&cmd_first; iterator<&cmd_last; iterator++)

static int do_help(int argc, char** argv)
{
  const struct console_cmd *cmd;
  console_printf("\n");
  FOR_EACH_CMD(cmd) {
    console_printf("%-10s - %s\n", cmd->name, cmd->help);
  }
  return 0;
}

static void sigint(void)
{
  const struct console_cmd *cmd;
  console_printf("\nINTERRUPT\n");
  FOR_EACH_CMD(cmd) {
    if (cmd->interrupt) {
      cmd->interrupt();
    }
  }
  //console_lock(0); /* Unlock console immediately */
}


static int execute (int argc, char * const* argv)
{
  const struct console_cmd *cmd;
  char *argv_cpy[_COMMAND_TOKEN_NMB+1];
  memset(argv_cpy, 0, sizeof(argv_cpy));
  int n;
  for(n = 0; n < argc; n++) {
    argv_cpy[n] = argv[n];
  }

  FOR_EACH_CMD(cmd) {
    if (strcasecmp(cmd->name, argv_cpy[0])==0) {
      if ((cmd->required_args != -1) && argc < cmd->required_args) {
        goto err_more_args;
      }
      if ((cmd->maximum_args != -1) && (argc > cmd->maximum_args)) {
        goto err_too_many_args;
      }
      cmd->handler(argc, argv_cpy);
      return 0;
    }
  }
  console_printf("\nCommand %s not found, type 'help' for a list\n", argv_cpy[0]);
  return 1;
err_more_args:
  console_printf("\nCommand %s requires at least %d args, %d given\n",
                 argv_cpy[0], cmd->required_args, argc);
  return 1;
err_too_many_args:
  console_printf("\nCommand %s takes a maximum of %d args, %d given\n",
                 argv_cpy[0], cmd->maximum_args, argc);
  return 1;
}

static char ** completion(int argc, char* const* argv)
{
#define COMPL_MAX_RESULTS	15
#define COMPL_BUF		200
  static char* noroom = "(completion: not enough memory)";
  static char complbuf [COMPL_BUF];
  static char* compl [COMPL_MAX_RESULTS + 1];
  static char* nocompl [] = { NULL };

  if (argc == 1) {
    const struct console_cmd *cmd;
    const char* part = argv[0];
    size_t partlen = strlen(part);

    int ncompl = 0;
    FOR_EACH_CMD(cmd)
    if (strncasecmp(cmd->name, part, partlen) == 0) {
      ncompl++;
    }
    int i = 0;
    size_t pos = 0;
    FOR_EACH_CMD(cmd)
    if (strncasecmp(cmd->name, part, partlen) == 0) {
      if (i == COMPL_MAX_RESULTS) {
        compl[i++] = noroom;
      } else if (i < COMPL_MAX_RESULTS) {
        const char* src = cmd->name + (i == 0 && ncompl > 1? partlen: 0);
        size_t srcsize = strlen(src) + 1;
        if (pos + srcsize <= COMPL_BUF) {
          compl[i++] = complbuf + pos;
          memcpy(complbuf + pos, src, srcsize);
          pos += srcsize;
        } else {
          compl[i++] = noroom;
        }
      }
    }
    compl[i] = NULL;

    return compl;
  }

  return nocompl;
}

// Thread event handler
static void console_handler(void *ctx, uint8_t sigNo, const void *evt)
{

  if(sigNo == SIGNAL_INSERT_CHAR) {
    MCAL_LOCK_IRQ();
    while(CBUF_IsEmpty(input_ringbuf) == 0) {
      char c = CBUF_Pop(input_ringbuf);
      MCAL_UNLOCK_IRQ();
      if (!console_locked || (c) == KEY_ETX) {
        microrl_insert_char (prl, c);
      }
      MCAL_LOCK_IRQ();
    }
    MCAL_UNLOCK_IRQ();
  }
}





void console_init(void)
{
  /* Microrl init */
  microrl_init(prl, console_output);
  microrl_set_execute_callback (prl, execute);
  microrl_set_sigint_callback(prl, sigint);
  microrl_set_complete_callback(prl, completion);

  CBUF_Init(input_ringbuf);

  console_thread = BLF_createSimpleThread(sizeof(*console_thread), 1, console_handler, 0);
  input_sem = BLF_createSemaphore(console_thread, SIGNAL_INSERT_CHAR, 0);
}

