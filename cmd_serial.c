#include <string.h>
#include <stdlib.h>
#include "mcal.h"
#include "microrl.h"
#include "console.h"
#include "optparse.h"
#include "rc_tx.h"






static void usage(const struct optparse_long *opts)
{
  unsigned char i=0;

  console_printf("options:\n");
  while(opts[i].longname != NULL)
  {
    console_printf("-%c --%s\n",opts[i].shortname, opts[i].longname);
    i++;
  }
}

//--------------------------------------------------------------------

static int do_send_raw(int argc, char** argv)
{
	if (argc != 2) {
		console_printf("send_raw [data]\n");
	} else {
	  //rc_tx_raw()
    int r = rc_tx_raw(argv[1], strlen(argv[1]));
	  if(r < 0) {
      console_printf("Failed decoding raw tx string\n");
	  } else {
	    console_printf("Sending %s\n", argv[1]);
	  }
	 // plctx_transmit(d);


	}
	return 0;
}

CONSOLE_CMD(send_raw, -1, 2,
	    do_send_raw, NULL, NULL,
	    "Send raw data to slave"
	    HELPSTR_NEWLINE "send_raw [data]\n"
);

//--------------------------------------------------------------------


#define DO_SEND_CMD_HELP_STR "send_cmd [group] [on | off]\n"
static int do_send_cmd(int argc, char** argv)
{
	if (argc!=3) {
		console_printf(DO_SEND_CMD_HELP_STR);
		return 0;
	}
  int group = atoi(argv[1]);
  if(group > 7) {
    console_printf("Group must be in range 0 to 7\n");
    return 0;
  }
  int on = 0;
  if(strcmp(argv[2], "on") == 0) {
    on = 1;
  }
  console_printf("Sending %s to slave group %d\n", on ? "On" : "Off", group);
  //command_send(group, on);
	return 0;
}

CONSOLE_CMD(send_cmd, -1, 3,
	    do_send_cmd, NULL, NULL,
	    "Send command to slave"
	    HELPSTR_NEWLINE DO_SEND_CMD_HELP_STR
);

//---------------------------------------------------------------------------------------------------------------
static int do_reboot(int argc, char** argv)
{
  MCAL_swReset();
  return 0;
}

CONSOLE_CMD(reboot, -1, 1,
	    do_reboot, NULL, NULL,
	    "Reboot system"
	    HELPSTR_NEWLINE "reboot"
);


//---------------------------------------------------------------------------------------------------------------
static int show_version(int argc, char** argv)
{
  console_printf("1.0.0\n");
  return 0;
}

CONSOLE_CMD(ver, -1, 1,
	    show_version, NULL, NULL,
	    "Show version"
	    HELPSTR_NEWLINE
);


//---------------------------------------------------------------------------------------------------------------
static int show_stat(int argc, char** argv)
{
  //console_printf("Total packets sent: %d\n", plctx_get_tx_tot());
  //console_printf("Zero cross detection delta time: %d\n", plctx_get_zcd_dt());
 // console_printf("Repeats left: %d\n", plctx_get_repeats_left());
  return 0;
}

CONSOLE_CMD(stat, -1, 1,
	    show_stat, NULL, NULL,
	    "Show statistics"
	    HELPSTR_NEWLINE
);


//------------------------------------------------------------------------------

static const struct optparse_long param_longopts[] = {
  {"help", 'h', OPTPARSE_NONE},
  {"repetitions", 'r', OPTPARSE_OPTIONAL},
  {NULL,   0,  0}
};


static int set_parameters(int argc, char** argv)
{
	int ch;
	if (argc==1) {
		usage(param_longopts);
		return 0;
	}
  struct optparse opt_parse;
	optparse_init(&opt_parse, argv);

  while ((ch = optparse_long(&opt_parse, param_longopts, NULL)) != -1) {
    switch (ch) {
      case 'h':
        usage(param_longopts);
        return 0;
        break;
      case 'r': {
          char *a = optparse_arg(&opt_parse);
          if(a != NULL) {
            //int v = atoi(a);
           // plctx_set_repeats(v);
          }
          //console_printf("Repetitions set to %d times\n",plctx_get_repeats());
        } break;
      case 'c':

        break;
      default:
        usage(param_longopts);
        break;
    }
  }
	return 0;
}

CONSOLE_CMD(param, -1, 3,
	    set_parameters, NULL, NULL,
	    "Modify parameters"
	    HELPSTR_NEWLINE "-r Set repetitions"
);

